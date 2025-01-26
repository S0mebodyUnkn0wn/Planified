#ifndef TASK_H
#define TASK_H
#include <gtk/gtk.h>
#include <common/types/completable.h>
#include <common/types/planable.h>
#include <common/types/entry.h>

G_BEGIN_DECLS

#define PLANIFIED_TYPE_TASK planified_task_get_type()
G_DECLARE_DERIVABLE_TYPE(PlanifiedTask, planified_task, PLANIFIED, TASK, PlanifiedEntry)

struct _PlanifiedTaskClass
{
	PlanifiedEntryClass parent_class;
};

PlanifiedTask* planified_task_new(gchar* name, gchar* description);

G_END_DECLS

#endif //TASK_H
