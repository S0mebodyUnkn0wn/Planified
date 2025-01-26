#ifndef ENTRY_H
#define ENTRY_H
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PLANIFIED_TYPE_ENTRY planified_entry_get_type()
G_DECLARE_DERIVABLE_TYPE(PlanifiedEntry, planified_entry, PLANIFIED, ENTRY, GObject)

struct _PlanifiedEntryClass
{
	GObjectClass parent_class;
};

PlanifiedEntry* planified_entry_new(gchar* title, gchar* description);



G_END_DECLS

#endif //ENTRY_H
