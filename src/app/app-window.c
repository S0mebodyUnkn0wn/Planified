#include <app/app.h>
#include <gtk/gtk.h>

struct _PlanifiedAppWindow
{
	GtkApplicationWindow parent;
};

G_DEFINE_FINAL_TYPE(PlanifiedAppWindow, planified_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void
planified_app_window_init(PlanifiedAppWindow* self)
{
	gtk_widget_init_template(GTK_WIDGET(self));
	// gtk_window_set_child(GTK_WINDOW(self),GTK_WIDGET(entries));
	// GListStore* entry_store = g_list_store_new(PLANIFIED_TYPE_ENTRY);
	// PlanifiedEntry* test_task_entry = PLANIFIED_ENTRY(planified_task_new("test", "test desc"));
	// PlanifiedEntry* test_entry = PLANIFIED_ENTRY(planified_entry_new("test", "test desc"));
	// g_list_store_append(entry_store, test_task_entry);
	// g_list_store_append(entry_store, test_entry);
	// g_object_set(G_OBJECT(entries), "list-model", G_LIST_MODEL(entry_store), nullptr);
}

static void
planified_app_window_class_init(PlanifiedAppWindowClass* class)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),"/planified/app/app-window.ui");
}


PlanifiedAppWindow*
planified_app_window_new(PlanifiedApp* app)
{
	return g_object_new(PLANIFIED_TYPE_APP_WINDOW,
	                    "application", app,
	                    "title", "Planified",
	                    "default-height", 900,
	                    "default-width", 1400,
	                    NULL);
}
