//
// Created by somebody on 09/12/24.
//

#ifndef ENTRY_CONTAINER_H
#define ENTRY_CONTAINER_H
#include <gtk/gtk.h>
#include "common/types/types.h"


G_BEGIN_DECLS

#define PLANIFIED_TYPE_ENTRY_CONTAINER planified_entry_container_get_type()
G_DECLARE_DERIVABLE_TYPE(PlanifiedEntryContainer, planified_entry_container, PLANIFIED, ENTRY_CONTAINER, GtkBox)

struct _PlanifiedEntryContainerClass
{
	GtkBoxClass parent_class;
};

PlanifiedEntryContainer*
planified_entry_container_new(void);

void
planified_entry_container_set_entry(PlanifiedEntryContainer* self, PlanifiedEntry* entry);

G_END_DECLS
#endif //ENTRY_CONTAINER_H
