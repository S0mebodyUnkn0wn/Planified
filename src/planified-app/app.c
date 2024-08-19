#include <gtk/gtk.h>
#include <sqlite3.h>
#include "app.h"
#include "widgets.h"
#include "storage.h"
//#include <adwaita.h>
#include "dialogs.h"
/* PlanifiedApp - a Subclass of GApplication with some custom handlers etc. */

//Path to the tasks database, will be changed to be a gsettings value later


typedef struct SQLiteHookData {
    void *user_data;
    int operation;
    char const *db_name;
    char const *table_name;
    sqlite3_int64 rowid;
} SQLiteHookData;

sqlite3 *get_handle(GtkWidget *self) {
    PlanifiedAppWindow *window = PLANIFIED_APP_WINDOW(
            gtk_widget_get_ancestor(GTK_WIDGET(self), PLANIFIED_APP_WINDOW_TYPE));
    g_assert(PLANIFIED_IS_APP_WINDOW(window));
    PlanifiedApp *app = PLANIFIED_APP(gtk_window_get_application(GTK_WINDOW(window)));
    g_assert(PLANIFIED_IS_APP(app));
    sqlite3 *handle = planified_app_get_db_handle(app);
    return handle;
}

PlanifiedApp *
get_planified_app(GtkWidget *self) {
    PlanifiedAppWindow *window = PLANIFIED_APP_WINDOW(
            gtk_widget_get_ancestor(GTK_WIDGET(self), PLANIFIED_APP_WINDOW_TYPE));
    g_assert(PLANIFIED_IS_APP_WINDOW(window));
    PlanifiedApp *app = PLANIFIED_APP(gtk_window_get_application(GTK_WINDOW(window)));
    g_assert(PLANIFIED_IS_APP(app));
    return app;
}


static void
preferences_activated(GSimpleAction *action,
                      GVariant *parameter,
                      gpointer app) {

    GtkWindow *win = gtk_application_get_active_window(app);
    GtkWidget *dialog = planified_preferences_dialog_new(win, app);
    gtk_window_present((GtkWindow *) dialog);
}

//Handler for app.quit
static void
quit_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer app) {
    g_application_quit(G_APPLICATION (app));
}

static GActionEntry app_entries[] =
        {
                {"newtask",     newtask_activated,     NULL, NULL, NULL},
                {"quit",        quit_activated,        NULL, NULL, NULL},
                {"preferences", preferences_activated, NULL, NULL, NULL},
        };


typedef enum {
    DATABASE = 1,
    TASK_ARRAY,
    N_PROPERTIES
} PlanifiedAppProperty;

typedef enum {
    TASK_ARRAY_UPDATED = 1,
    N_SIGNALS
} PlanifiedAppSignal;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};
static guint obj_signals[N_SIGNALS];

struct _PlanifiedApp {
    GtkApplication parent;

    sqlite3 *handle;
    GListStore *tasks;

    // Workaround for SQLites wierd callback implementation
    gint64 deferred_update_rowid;
    int deferred_update_operation;
};

G_DEFINE_TYPE(PlanifiedApp, planified_app, GTK_TYPE_APPLICATION);


gint
task_array_find_task_pos_by_rowid(GListStore *arr, gint64 rowid) {
    for (guint i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(arr)); i++) {
        PlanifiedTask *task = PLANIFIED_TASK(g_list_model_get_item(G_LIST_MODEL(arr), i));
        if (planified_task_get_rowid(task) == rowid) {
            g_object_unref(task);
            return (gint) i;
        }
        g_object_unref(task);
    }
    return -1;
}

/*
 * Change a single item of the task array to reflect the updated status of the database
 *
 * Made to work as a callback for sqlite3_update_hook;
 */
void
planified_app_update_task(SQLiteHookData *data) {
    PlanifiedApp *self = PLANIFIED_APP(data->user_data);
    gint64 rowid = data->rowid;
    const gchar *table_name = data->table_name;
    gint operation = data->operation;
    free(data);
    g_assert(PLANIFIED_IS_APP(self));
    sqlite3 *handle = self->handle;
    sqlite3_stmt *fetch_task_stmt;
    sqlite3_prepare_v2(handle,
                       "SELECT * FROM tasks WHERE ROWID=?;",
                       -1, &fetch_task_stmt, NULL);
    sqlite3_bind_int64(fetch_task_stmt, 1, rowid);
    sqlite3_step(fetch_task_stmt);
    PlanifiedTask *old_task = NULL;
    gint old_task_pos = task_array_find_task_pos_by_rowid(self->tasks, rowid);
    old_task = old_task_pos >= 0 ? PLANIFIED_TASK(g_list_model_get_item(G_LIST_MODEL(self->tasks), old_task_pos))
                                 : NULL;
    PlanifiedTask *new_task = NULL;
    GListStore *tags;
    switch (operation) {
        case SQLITE_DELETE:
            guint pos;
            g_list_store_find(self->tasks, old_task, &pos);
            g_list_store_remove(self->tasks, pos);
            break;
        case SQLITE_INSERT:
            new_task = planified_task_new_from_sqlite(fetch_task_stmt);
            tags = database_get_tags_of_task(handle, new_task);
            planified_task_set_tags(new_task, tags);
            g_list_store_append(self->tasks, new_task);
            g_object_unref(new_task);
            break;
        case SQLITE_UPDATE:
            new_task = old_task;
            planified_task_update_from_sqlite3(old_task, fetch_task_stmt);
            tags = database_get_tags_of_task(handle, new_task);
            planified_task_replace_tags_from_store(new_task, tags);
            g_list_model_items_changed((GListModel *) self->tasks, old_task_pos, 1, 1);
            g_object_unref(tags);
            break;
        default:
            g_printerr("ERROR: Unrecognized SQLite operation code: %d\n", operation);
    }
    if (G_IS_OBJECT(old_task))
        g_object_unref(old_task);
    sqlite3_finalize(fetch_task_stmt);
    g_signal_emit(self, obj_signals[TASK_ARRAY_UPDATED], 0, operation, new_task);
}

static void
planified_app_update_tags(SQLiteHookData *data) {
    PlanifiedApp *self = PLANIFIED_APP(data->user_data);
    gint64 rowid = data->rowid;
    const gchar *table_name = data->table_name;
    gint operation = data->operation;
    free(data);
    sqlite3 *handle = planified_app_get_db_handle(self);
    sqlite3_stmt *fetch_task_stmt;
    g_print("Preparing: %d ", sqlite3_prepare_v2(handle,
                                                 "SELECT * FROM tag_assignments WHERE ROWID=?;",
                                                 -1, &fetch_task_stmt, NULL));
    sqlite3_bind_int64(fetch_task_stmt, 1, rowid);
    const gchar *tag_name;
    gint64 task_id;
    int task_pos = -1;
    if (sqlite3_step(fetch_task_stmt) == SQLITE_ROW) {
        tag_name = (const gchar *) sqlite3_column_text(fetch_task_stmt, 0);
        task_id = sqlite3_column_int64(fetch_task_stmt, 1);
        task_pos = task_array_find_task_pos_by_rowid(self->tasks, task_id);

    }
    PlanifiedTask *task = 0 ? PLANIFIED_TASK(g_list_model_get_item(G_LIST_MODEL(self->tasks), task_pos))
                            : NULL;


    switch (operation) {
        case SQLITE_INSERT:
            GListStore *tags = database_get_tags_of_task(handle, task);
            planified_task_replace_tags_from_store(task, tags);
            g_list_model_items_changed((GListModel *) self->tasks, task_pos, 1, 1);
            break;
        case SQLITE_DELETE:

    }

    if (G_IS_OBJECT(task)) {
        g_object_unref(task);
    }

}

void
planified_app_schedule_task_update(void *app, int operation, char const *db_name, char const *table_name,
                                   sqlite3_int64 rowid) {
    if (strcmp(table_name, "tasks") == 0) {
        SQLiteHookData *data = malloc(sizeof(SQLiteHookData));
        data->user_data = app;
        data->operation = operation;
        data->db_name = db_name;
        data->table_name = table_name;
        data->rowid = rowid;

        g_timeout_add_once(100, (GSourceOnceFunc) planified_app_update_task, data);
    }

}

static void
sync_colors(GSettings *g_settings) {
    if (strcmp("prefer-dark", g_settings_get_string(g_settings, "color-scheme")) == 0) {
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", 1, NULL);
    } else {
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", 0, NULL);
    }
}

static void
planified_app_startup(GApplication *app) {
    G_APPLICATION_CLASS (planified_app_parent_class)->startup(app);


    g_action_map_add_action_entries(G_ACTION_MAP (app),
                                    app_entries, G_N_ELEMENTS (app_entries),
                                    app);


    PlanifiedApp *_app = PLANIFIED_APP(app);
    sqlite3 *handle;
    char *DB_FILEPATH = g_strconcat(g_get_home_dir(),"/.local/share/planified/data",NULL);
    int status = init_sqlite_db(&handle, DB_FILEPATH);
    g_print("STATUS: %d", status);
    _app->handle = handle;
    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(css, "/planified/style.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), (GtkStyleProvider *) css,
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    GSettings *g_settings = g_settings_new("org.gnome.desktop.interface");
    g_signal_connect_swapped(g_settings, "changed::color-scheme", G_CALLBACK(sync_colors), g_settings);
    sync_colors(g_settings);

    // Subscribe to SQL updates
    sqlite3_update_hook(handle, planified_app_schedule_task_update, app);

    sqlite3_stmt *fetch_task_stmt;
    sqlite3_prepare_v2(handle,
                       "SELECT * FROM tasks;",
                       -1, &fetch_task_stmt, NULL);
    while (sqlite3_step(fetch_task_stmt) == SQLITE_ROW) {
        PlanifiedTask *task = planified_task_new_from_sqlite(fetch_task_stmt);
        GListStore *tags = database_get_tags_of_task(handle, task);
        planified_task_set_tags(task, tags);
        g_list_store_append(_app->tasks, task);
        g_object_unref(task);
    }
    g_signal_emit(_app, obj_signals[TASK_ARRAY_UPDATED], 0, SQLITE_INSERT, NULL);

}

static void
planified_app_init(PlanifiedApp *app) {
    app->deferred_update_operation = -1;
    app->deferred_update_rowid = -1;
    app->tasks = g_list_store_new(PLANIFIED_TASK_TYPE);


}

static void
planified_app_activate(GApplication *app) {
    G_APPLICATION_CLASS (planified_app_parent_class)->activate(app);

    PlanifiedAppWindow *window;
    window = planified_app_window_new(PLANIFIED_APP(app));
    planified_app_window_setup(window);
    g_signal_emit(app, obj_signals[TASK_ARRAY_UPDATED], 0, SQLITE_INSERT, NULL);
    gtk_window_present(GTK_WINDOW(window));

}

static void
planified_app_shutdown(GApplication *app) {
    G_APPLICATION_CLASS (planified_app_parent_class)->shutdown(app);
    printf("Bye!\n");

}

static void
planified_app_set_property(GObject *object,
                           guint property_id,
                           const GValue *value,
                           GParamSpec *pspec) {
    PlanifiedApp *self = PLANIFIED_APP(object);

    switch ((PlanifiedAppProperty) property_id) {
        case DATABASE:
            if (self->handle) {
                sqlite3_close_v2(self->handle);
            }
            self->handle = g_value_get_pointer(value);
            break;
        case TASK_ARRAY:
            self->tasks = g_value_get_object(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
planified_app_get_property(GObject *object,
                           guint property_id,
                           GValue *value,
                           GParamSpec *pspec) {

    PlanifiedApp *self = PLANIFIED_APP(object);

    switch ((PlanifiedAppProperty) property_id) {
        case DATABASE:
            g_value_set_pointer(value, self->handle);
            break;
        case TASK_ARRAY:
            g_value_set_object(value, self->tasks);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
planified_app_class_init(PlanifiedAppClass *class) {
    G_APPLICATION_CLASS (class)->startup = planified_app_startup;
    G_APPLICATION_CLASS (class)->activate = planified_app_activate;
    G_APPLICATION_CLASS (class)->shutdown = planified_app_shutdown;

    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->set_property = planified_app_set_property;
    object_class->get_property = planified_app_get_property;

    obj_properties[DATABASE] =
            g_param_spec_pointer("database-handle",
                                 "Database handle",
                                 "A pointer to the sqlite3 object containing application's database, read only",
                                 G_PARAM_READABLE);
    obj_properties[TASK_ARRAY] =
            g_param_spec_pointer("task-array",
                                 "Task Array",
                                 "A pointer to a GListStore containing pointers to all `PlanifiedTask` objects available to the application, read only, automatically updates on database changes",
                                 G_PARAM_READABLE);

    g_object_class_install_properties(G_OBJECT_CLASS(class), N_PROPERTIES, obj_properties);

    obj_signals[TASK_ARRAY_UPDATED] =
            g_signal_newv("task-array-updated",
                          G_TYPE_FROM_CLASS (object_class),
                          G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                          NULL /* closure */,
                          NULL /* accumulator */,
                          NULL /* accumulator data */,
                          NULL /* C marshaller */,
                          G_TYPE_NONE /* return_type */,
                          2     /* n_params */,
                          (GType[]) {
                                  G_TYPE_INT,           // SQL operation
                                  PLANIFIED_TASK_TYPE   // modified_task
                          }
            );

}

GListStore *
planified_app_get_tasks(PlanifiedApp *self) {
    return self->tasks;
}


PlanifiedApp *
planified_app_new() {
    return g_object_new(PLANIFIED_APP_TYPE,
                        "application-id", "com.github.somebodyunknown.planified",
                        "resource-base-path", "/planified",
                        NULL);
}

// Getter for PlanifiedApp.db_handle (sqlite3 object, used for manipulating the database via sqlite)
sqlite3 *planified_app_get_db_handle(PlanifiedApp *app) {
    g_assert(PLANIFIED_IS_APP(app));
    return app->handle;
}

int main(int argc, char **argv) {
    return g_application_run(G_APPLICATION(planified_app_new()), argc, argv);
}
