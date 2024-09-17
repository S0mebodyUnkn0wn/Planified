/**
 * Headers for derivable widget classes
 */
#ifndef PLANIFIED_DERIVABLE_WIDGETS_H
#define PLANIFIED_DERIVABLE_WIDGETS_H
#include "common.h"

#define PLANIFIED_TASK_CONTAINER_TYPE (planified_task_container_get_type())

G_DECLARE_DERIVABLE_TYPE(PlanifiedTaskContainer, planified_task_container, PLANIFIED, TASK_CONTAINER, GtkBox)

struct _PlanifiedTaskContainerClass {
    GtkBoxClass parent_class;

    void (*refresh_data) (GObject *_task, GParamSpec *pspec, gpointer _self);
    void (*self_destruct) (PlanifiedTaskContainer *self);

    gpointer padding[6];
};

void planified_task_container_delete_task(PlanifiedTaskContainer *self);

void planified_task_container_request_delete(PlanifiedTaskContainer *self);

PlanifiedTask *
planified_task_container_get_task(PlanifiedTaskContainer *self);

#endif //PLANIFIED_DERIVABLE_WIDGETS_H
