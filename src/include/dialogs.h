/*
 * Headers for classes representing different dialog windows
 */

#ifndef PLANIFIED_DIALOGS_H
#define PLANIFIED_DIALOGS_H

#define PLANIFIED_NEWENTRY_DIALOG_TYPE (planified_newentry_dialog_get_type())

G_DECLARE_FINAL_TYPE (PlanifiedNewentryDialog, planified_newentry_dialog, PLANIFIED, NEWENTRY_DIALOG, GtkApplicationWindow)


#define PLANIFIED_PLAN_DIALOG_TYPE (planified_plan_dialog_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedPlanDialog, planified_plan_dialog, PLANIFIED, PLAN_DIALOG, GtkApplicationWindow )


// PlanifiedNewentryDialog Methods:

PlanifiedNewentryDialog *
planified_newentry_dialog_new(PlanifiedAppWindow *win, GtkApplication *app);

PlanifiedNewentryDialog *
planified_newentry_dialog_editmode_new(PlanifiedAppWindow *win, GtkApplication *app, PlanifiedTask *task);

void
newtask_activated(GSimpleAction *action,
                  GVariant *parameter,
                  gpointer app);


// PlanifiedPlanDialog Methods:

PlanifiedPlanDialog *
planified_plan_dialog_new(PlanifiedAppWindow *win, GtkApplication *app, PlanifiedTask *task);

void
planified_plan_dialog_set_start_date(PlanifiedPlanDialog *self, GDateTime *date);

void
planified_plan_dialog_set_end_date(PlanifiedPlanDialog *self, GDateTime *date);

void
planified_plan_dialog_activated(GSimpleAction *action,
                                GVariant *parameter,
                                gpointer task_widget);
#endif //PLANIFIED_DIALOGS_H
