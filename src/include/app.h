/**
 * Headers for PlanifiedApp and PlanifiedAppWindow and their methods/functions
 */

#ifndef PLANIFIED_APP_H
#define PLANIFIED_APP_H

#include <gtk/gtk.h>
#include <sqlite3.h>
//#include <adwaita.h>

GtkWidget *
build_timetable_view();

sqlite3 *get_handle(GtkWidget *self);

#define PLANIFIED_APP_TYPE (planified_app_get_type ())

G_DECLARE_FINAL_TYPE (PlanifiedApp, planified_app, PLANIFIED, APP, GtkApplication)

PlanifiedApp *planified_app_new(void);

sqlite3 *planified_app_get_db_handle(PlanifiedApp *app);

#define PLANIFIED_APP_WINDOW_TYPE (planified_app_window_get_type ())

G_DECLARE_FINAL_TYPE (PlanifiedAppWindow, planified_app_window, PLANIFIED, APP_WINDOW, GtkApplicationWindow)

PlanifiedAppWindow *planified_app_window_new(PlanifiedApp *app);

void planified_app_window_open(PlanifiedAppWindow *win,
                               GFile *file);

GListStore *
planified_app_get_tasks(PlanifiedApp *self);

PlanifiedApp *
get_planified_app(GtkWidget *self);

void
planified_app_window_setup(PlanifiedAppWindow *self);

#endif //PLANIFIED_APP_H
