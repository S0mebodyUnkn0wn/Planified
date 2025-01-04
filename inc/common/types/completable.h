#ifndef COMPLETABLE_H
#define COMPLETABLE_H
#include <gtk/gtk.h>


#define PLANIFIED_TYPE_COMPLETABLE planified_completable_get_type()

G_DECLARE_INTERFACE(PlanifiedCompletable, planified_completable, PLANIFIED, COMPLETABLE, GObject)

struct _PlanifiedCompletableInterface
{
	GTypeInterface parent;

};


#endif //COMPLETABLE_H
