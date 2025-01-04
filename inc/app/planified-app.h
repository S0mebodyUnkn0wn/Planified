//
// Created by somebody on 09/12/24.
//

#ifndef PLANIFIED_APP_H
#define PLANIFIED_APP_H

#include <gtk/gtk.h>
#include <sqlite3.h>

#define PLANIFIED_TYPE_APP (planified_app_get_type())

G_DECLARE_FINAL_TYPE (PlanifiedApp, planified_app, PLANIFIED, APP, GtkApplication)

PlanifiedApp *planified_app_new (void);
sqlite3* planified_app_get_handle (PlanifiedApp *app);

#endif //PLANIFIED_APP_H
