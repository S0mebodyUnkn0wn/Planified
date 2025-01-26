#ifndef ENTRY_LIST_H
#define ENTRY_LIST_H
#include <gtk/gtk.h>
#include "app/widgets/containers/entry-container.h"
#include "common/types/types.h"

G_BEGIN_DECLS

#define PLANIFIED_TYPE_ENTRY_LIST planified_entry_list_get_type ()
G_DECLARE_FINAL_TYPE(PlanifiedEntryList, planified_entry_list, PLANIFIED, ENTRY_LIST, GtkBox)

PlanifiedEntryList* planified_entry_list_new(void);

G_END_DECLS

#endif //ENTRY_LIST_H
