#ifndef COMPLETABLE_H
#define COMPLETABLE_H
#include <gtk/gtk.h>
#include <common/types/enums.h>

G_BEGIN_DECLS

#define PLANIFIED_TYPE_COMPLETABLE planified_completable_get_type()
G_DECLARE_INTERFACE(PlanifiedCompletable, planified_completable, PLANIFIED, COMPLETABLE, GObject)

struct _PlanifiedCompletableInterface
{
	GTypeInterface parent;

	// bool (*is_completed)(PlanifiedCompletable* completable);
	// void (*set_completed)(PlanifiedCompletable* completable, bool completed);
};

// bool
// planified_completable_is_completed(PlanifiedCompletable* completable);
// void planified_completable_set_completed(PlanifiedCompletable* completable, bool completed);

G_END_DECLS

#endif //COMPLETABLE_H
