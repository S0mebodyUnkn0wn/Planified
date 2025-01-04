#include "app/app-window.h"
#include "app/planified-app.h"
#include "glib-object.h"
#include <app/app.h>
#include <gtk/gtk.h>

struct _PlanifiedAppWindow
{
	GtkApplicationWindow parent;
};

G_DEFINE_FINAL_TYPE(PlanifiedAppWindow, planified_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void
planified_app_window_init(PlanifeidAppWindow* self)
{
}

static void
planified_app_window_class_init(PlanifiedAppWindowClass* class)
{
}


PlanifiedAppWindow*
planified_app_window_new(PlanifiedApp* app)
{
	return g_object_new(PLANIFIED_TYPE_APP_WINDOW,
	                    "application", app,
	                    "default-height", 900,
	                    "default-width", 1400,
	                    NULL);
}
