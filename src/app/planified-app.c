#include <app/app.h> 
#include <gtk/gtk.h> 
#include <sqlite3.h>
#include <core/api.h>
#include <common/types/event.h>
struct _PlanifiedApp
{
	GtkApplication parent;

	sqlite3* db;
	GListStore* tasks;
};

G_DEFINE_TYPE(PlanifiedApp, planified_app, GTK_TYPE_APPLICATION)

static void
planified_app_startup(GApplication* app)
{
	G_APPLICATION_CLASS(planified_app_parent_class)->startup(app);
	//self test:
	planified_event_new();

}

static void
planified_app_init(PlanifiedApp* self)
{

}

static void
planified_app_activate(GApplication* app)
{
	G_APPLICATION_CLASS(planified_app_parent_class)->activate(app);
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
}

PlanifiedApp*
planified_app_new()
{
	return g_object_new(PLANIFIED_TYPE_APP,
	                    "application-id", "com.github.somebodyunknown.planified",
	                    "resource-base-path", "/planified",
	                    NULL);
}

int main(int argc, char **argv) {
	return g_application_run(G_APPLICATION(planified_app_new()), argc, argv);
}
