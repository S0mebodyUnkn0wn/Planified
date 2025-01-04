#ifndef EVENT_H
#define EVENT_H
#include <gtk/gtk.h>
#include <common/types/entry.h>
#include <common/types/plannable.h>

G_BEGIN_DECLS

#define PLANIFIED_TYPE_EVENT planified_event_get_type()
G_DECLARE_DERIVABLE_TYPE(PlanifiedEvent, planified_event, PLANIFIED, EVENT, PlanifiedEntry)

struct _PlanifiedEventClass
{
	PlanifiedEntryClass parent_class;


};

PlanifiedEvent* planified_event_new(void);

G_END_DECLS

#endif //EVENT_H
