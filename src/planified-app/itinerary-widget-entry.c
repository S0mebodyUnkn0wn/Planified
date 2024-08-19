#include <gtk/gtk.h>
#include "widgets.h"
#include "derivable-widgets.h"
#include "app.h"
#include "storage.h"
#include "dialogs.h"

typedef enum {
    START_TIME = 1,
    N_PROPERTIES
} PlanifiedItineraryWidgetEntryProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};

struct _PlanifiedItineraryWidgetEntry {
    PlanifiedTaskContainer parent;

    GtkWidget *task_name;
    GtkWidget *task_description;

    gint64 time;

};

G_DEFINE_TYPE(PlanifiedItineraryWidgetEntry, planified_itinerary_widget_entry, PLANIFIED_TASK_CONTAINER_TYPE);

static void
self_destruct(PlanifiedTaskContainer *self) {
    g_print("Destruct called for IWE\n");
    PLANIFIED_TASK_CONTAINER_CLASS(planified_itinerary_widget_entry_parent_class)->self_destruct(self);

    if (PLANIFIED_IS_ITINERARY_WIDGET_ENTRY(self)) {
        GtkGrid *grid = (GtkGrid *) gtk_widget_get_ancestor((GtkWidget *) self, GTK_TYPE_GRID);
        if (grid == NULL) {
            g_print("Grid is null\n");
        }
        else {
            g_assert(GTK_IS_GRID(grid));
            gtk_grid_remove(grid, (GtkWidget *) self);
        }
    }
}

static void
refresh_data(GObject *_task, GParamSpec *pspec, gpointer _self) {
//    if (!PLANIFIED_IS_ITINERARY_WIDGET_ENTRY(_self)) return;
    PlanifiedItineraryWidgetEntry *self = PLANIFIED_ITINERARY_WIDGET_ENTRY(_self);
    char *task_title = planified_task_get_task_text(planified_task_container_get_task(PLANIFIED_TASK_CONTAINER(self)));
    char *task_desc = planified_task_get_description(planified_task_container_get_task(PLANIFIED_TASK_CONTAINER(self)));
    gtk_label_set_label(GTK_LABEL(self->task_name), task_title);
    gtk_label_set_label(GTK_LABEL(self->task_description),task_desc);
    gtk_widget_set_visible((GtkWidget *) self->task_description, strlen(task_desc) != 0);

//    g_free(task_title);
}


static void
planified_itinerary_widget_entry_set_property(GObject *object,
                                              guint property_id,
                                              const GValue *value,
                                              GParamSpec *pspec) {
    PlanifiedItineraryWidgetEntry *self = PLANIFIED_ITINERARY_WIDGET_ENTRY(object);

    switch ((PlanifiedItineraryWidgetEntryProperty) property_id) {
        case START_TIME:
            self->time = g_value_get_int64(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            return;
    }
}


static void
planified_itinerary_widget_entry_get_property(GObject *object,
                                              guint property_id,
                                              GValue *value,
                                              GParamSpec *pspec) {

    PlanifiedItineraryWidgetEntry *self = PLANIFIED_ITINERARY_WIDGET_ENTRY(object);

    switch ((PlanifiedItineraryWidgetEntryProperty) property_id) {
        case START_TIME:
            g_value_set_int64(value, self->time);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
planified_itinerary_widget_entry_dispose(GObject *gobject) {
    g_print("Dispose started\n");

    gtk_widget_dispose_template(GTK_WIDGET(gobject), PLANIFIED_ITINERARY_WIDGET_ENTRY_TYPE);
    G_OBJECT_CLASS(planified_itinerary_widget_entry_parent_class)->dispose(gobject);

}

static void
planified_itinerary_widget_entry_init(PlanifiedItineraryWidgetEntry *widget) {
    gtk_widget_init_template(GTK_WIDGET(widget));
    gtk_widget_add_css_class(GTK_WIDGET(widget), "timetable_entry");
}

static void
planified_itinerary_widget_entry_class_init(PlanifiedItineraryWidgetEntryClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/planified/itinerary-widget-entry.ui");

    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedItineraryWidgetEntry, task_name);
    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedItineraryWidgetEntry, task_description);

    GObjectClass *object_class = G_OBJECT_CLASS (class);
    object_class->set_property = planified_itinerary_widget_entry_set_property;
    object_class->get_property = planified_itinerary_widget_entry_get_property;
    object_class->dispose = planified_itinerary_widget_entry_dispose;

    PLANIFIED_TASK_CONTAINER_CLASS(class)->refresh_data = refresh_data;
    PLANIFIED_TASK_CONTAINER_CLASS(class)->self_destruct = self_destruct;

    obj_properties[START_TIME] =
            g_param_spec_int64("start-time",
                               "Start Time",
                               "The time this slot is started at",
                               0,
                               INT64_MAX,
                               0,
                               G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);


    g_object_class_install_properties(G_OBJECT_CLASS(class), N_PROPERTIES, obj_properties);
}

/**
 * Creates new PlanifiedItineraryWidgetEntry
 *
 * @note The widget's lifetime is bound to the lifetime of the task it represents
 */
PlanifiedItineraryWidgetEntry *
planified_itinerary_widget_entry_new(PlanifiedTask *task, gint64 time) {
    PlanifiedItineraryWidgetEntry *self = g_object_new(PLANIFIED_ITINERARY_WIDGET_ENTRY_TYPE,
                                                       "task", task,
                                                       "start-time", time,
                                                       NULL);
//    refresh_data(NULL, NULL, self);
    return self;
}

PlanifiedTask *
planified_itinerary_widget_entry_get_task(PlanifiedItineraryWidgetEntry *self) {
    return planified_task_container_get_task(PLANIFIED_TASK_CONTAINER(self));
}
