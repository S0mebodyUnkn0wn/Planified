#include <app/app.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <core/api.h>
#include <common/types/event.h>

struct _PlanifiedApp
{
	GtkApplication parent;

	sqlite3* db;
	GListModel* data_list_model;
};

typedef enum
{
	APP_DATA_LIST_MODEL = 1,
	N_PROPERTIES
} PlanifiedAppProperty;

static GParamSpec* obj_properties[N_PROPERTIES] = {nullptr,};

G_DEFINE_TYPE(PlanifiedApp, planified_app, GTK_TYPE_APPLICATION)

static void
planified_app_startup(GApplication* app)
{
	G_APPLICATION_CLASS(planified_app_parent_class)->startup(app);
	GtkCssProvider* css = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(css, "/planified/app/style.css");
	gtk_style_context_add_provider_for_display(gdk_display_get_default(),GTK_STYLE_PROVIDER(css),
	                                           GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

static void
planified_app_set(GObject* object,
                  guint property_id,
                  const GValue* value,
                  GParamSpec* pspec)
{
	PlanifiedApp* self = PLANIFIED_APP(object);
	switch ((PlanifiedAppProperty)property_id)
	{
	case APP_DATA_LIST_MODEL:
		if (self->data_list_model != nullptr)
			g_object_unref(self->data_list_model);
		self->data_list_model = g_value_get_object(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void
planified_app_get(GObject* object,
                  guint property_id,
                  GValue* value,
                  GParamSpec* pspec)
{
	PlanifiedApp* self = PLANIFIED_APP(object);
	switch ((PlanifiedAppProperty)property_id)
	{
	case APP_DATA_LIST_MODEL:
		g_value_set_object(value, self->data_list_model);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}


static void
planified_app_init(PlanifiedApp* self)
{
	self->data_list_model = G_LIST_MODEL(g_list_store_new(PLANIFIED_TYPE_ENTRY));
	g_list_store_append(G_LIST_STORE(self->data_list_model),
	                    planified_task_new("Task A", "A task created in planified_app_init for testing purposes"));
}

static void
planified_app_activate(GApplication* app)
{
	G_APPLICATION_CLASS(planified_app_parent_class)->activate(app);
	PlanifiedAppWindow* window = planified_app_window_new(PLANIFIED_APP(app));
	gtk_window_present(GTK_WINDOW(window));
}

static void
planified_app_shutdown(GApplication* app)
{
	G_APPLICATION_CLASS(planified_app_parent_class)->shutdown(app);
}

static void
planified_app_class_init(PlanifiedAppClass* class)
{
	G_APPLICATION_CLASS(class)->startup = planified_app_startup;
	G_APPLICATION_CLASS(class)->activate = planified_app_activate;
	G_APPLICATION_CLASS(class)->shutdown = planified_app_shutdown;

	G_OBJECT_CLASS(class)->set_property = planified_app_set;
	G_OBJECT_CLASS(class)->get_property = planified_app_get;

	obj_properties[APP_DATA_LIST_MODEL] =
		g_param_spec_object("data-model",
		                    "Data Model",
		                    "The list model containing event and task data",
		                    G_TYPE_LIST_MODEL,
		                    G_PARAM_READABLE);
}

PlanifiedApp*
planified_app_new()
{
	return g_object_new(PLANIFIED_TYPE_APP,
	                    "application-id", "com.github.somebodyunknown.planified",
	                    "resource-base-path", "/planified",
	                    NULL);
}

int main(int argc, char** argv)
{
	return g_application_run(G_APPLICATION(planified_app_new()), argc, argv);
}
