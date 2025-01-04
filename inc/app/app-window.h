//
// Created by somebody on 09/12/24.
//

#ifndef APP_WINDOW_H
#define APP_WINDOW_H
#include "app/planified-app.h"
#include <gtk/gtk.h>

#define PLANIFIED_TYPE_APP_WINDOW (planified_app_window_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedAppWindow, planified_app_window, PLANIFIED, APP_WINDOW, GtkApplicationWindow)

void 
planified_app_window_setup(GtkApplicationWindow *self);

PlanifiedAppWindow 
planified_app_window_new(PlanifiedApp *app);



#endif //APP_WINDOW_H
