//
// Created by somebody on 29/07/24.
//

#ifndef PLANIFIED_DIALOGS_H
#define PLANIFIED_DIALOGS_H

#define NEWTASK_DIAG_TYPE (newtask_diag_get_type())

G_DECLARE_FINAL_TYPE (NewTaskDiag, newtask_diag, NEWTASK, DIAG, GtkApplicationWindow)


// NewTaskDiag Methods:

NewTaskDiag *
newtask_diag_new(PlanifiedAppWindow *win, GtkApplication *app);

NewTaskDiag *
newtask_diag_editmode_new(PlanifiedAppWindow *win, GtkApplication *app, PlanifiedTask *task);

void
newtask_activated(GSimpleAction *action,
                  GVariant *parameter,
                  gpointer app);
#endif //PLANIFIED_DIALOGS_H
