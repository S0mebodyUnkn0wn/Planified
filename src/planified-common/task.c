//
// Created by somebody on 02/05/24.
// Set of methods for task
//
#include <glib.h>
#include <stdlib.h>
#include "common.h"
#include "storage.h"

typedef enum {
    TASK_TEXT = 1,
    DEADLINE,
    TIME_REQ,
    TAGS,
    LOCATION,
    SCHEDULE,
    IS_COMPLETE,
    ROW_ID,
    DESCRIPTION,
    PLAN_START,
    PLAN_SPAN,
    TAG_COUNT,
    N_PROPERTIES
} PlanifiedTaskProperty;

typedef enum {
    TASK_DELETED = 1,
    N_SIGNALS
} PlanifiedTaskSignal;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};
static guint obj_signals[N_SIGNALS];


struct _PlanifiedTask {
    GObject parent;

    gchararray task_text;
    GDateTime *deadline;
    gint time_req;
    GListStore *tags;
    gchararray location;
    GDateTime *schedule;
    gboolean is_complete;
    gint64 rowid;
    gchararray description;
    GDateTime *plan_start;
    gint plan_span;
    gint tag_count;
};

G_DEFINE_TYPE(PlanifiedTask, planified_task, G_TYPE_OBJECT)

static void
planified_task_set_property(GObject *object,
                            guint property_id,
                            const GValue *value,
                            GParamSpec *pspec) {
    PlanifiedTask *self = PLANIFIED_TASK(object);

    switch ((PlanifiedTaskProperty) property_id) {
        case TASK_TEXT:
            g_free(self->task_text);
            self->task_text = g_value_dup_string(value);
            break;
        case DEADLINE:
            if (self->deadline)
                g_date_time_unref(self->deadline);
            self->deadline = g_value_get_pointer(value);
            break;
        case TIME_REQ:
            self->time_req = g_value_get_int(value);
            break;
        case TAGS:
            g_free(self->tags);
            self->tags = g_value_get_object(value);
            break;
        case LOCATION:
            g_free(self->location);
            self->location = g_value_dup_string(value);
            if (self->location == NULL) {
                self->location = "";
            }
            break;
        case SCHEDULE:
            if (self->schedule)
                g_date_time_unref(self->schedule);
            self->schedule = g_value_get_pointer(value);
            break;
        case IS_COMPLETE:
            self->is_complete = g_value_get_boolean(value);
            break;
        case ROW_ID:
            self->rowid = g_value_get_int64(value);
            break;
        case DESCRIPTION:
            g_free(self->description);
            self->description = g_value_dup_string(value);
            if (self->description == NULL) {
                self->description = "";
            }
            break;
        case PLAN_START:
            if (self->plan_start)
                g_date_time_unref(self->plan_start);
            self->plan_start = g_value_get_pointer(value);
            break;
        case PLAN_SPAN:
            self->plan_span = g_value_get_int(value);
            break;
        case TAG_COUNT:
            self->tag_count = g_value_get_int(value);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            return;
    }

}

static void
planified_task_get_property(GObject *object,
                            guint property_id,
                            GValue *value,
                            GParamSpec *pspec) {
    PlanifiedTask *self = PLANIFIED_TASK(object);

    switch ((PlanifiedTaskProperty) property_id) {
        case TASK_TEXT:
            g_value_set_string(value, self->task_text);
            break;
        case DEADLINE:
            g_value_set_pointer(value, self->deadline);
            break;
        case TIME_REQ:
            g_value_set_int(value, self->time_req);
            break;
        case TAGS:
            g_value_take_object(value, self->tags);
            break;
        case LOCATION:
            g_value_set_string(value, self->location);
            break;
        case SCHEDULE:
            g_value_set_pointer(value, self->schedule);
            break;
        case IS_COMPLETE:
            g_value_set_boolean(value, self->is_complete);
            break;
        case ROW_ID:
            g_value_set_int64(value, self->rowid);
            break;
        case DESCRIPTION:
            g_value_set_string(value, self->location);
            break;
        case PLAN_START:
            g_value_set_pointer(value, self->plan_start);
            break;
        case PLAN_SPAN:
            g_value_set_int(value, self->plan_span);
            break;
        case TAG_COUNT:
            g_value_set_int(value, self->tag_count);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
planified_task_dispose(GObject *gobject) {
//    g_print("Called dispose for task \"%s\"\n", planified_task_get_task_text((PlanifiedTask *) gobject));
    g_signal_emit(gobject, obj_signals[TASK_DELETED], 0);
    PlanifiedTask *self = PLANIFIED_TASK(gobject);
    if (self->deadline)
        g_date_time_unref(self->deadline);
    if (self->plan_start)
        g_date_time_unref(self->plan_start);
    if (self->schedule)
        g_date_time_unref(self->schedule);

    G_OBJECT_CLASS (planified_task_parent_class)->dispose(gobject);
}

static void
planified_task_init(PlanifiedTask *self) {

}

static void
planified_task_class_init(PlanifiedTaskClass *class) {


    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->set_property = planified_task_set_property;
    object_class->get_property = planified_task_get_property;
    object_class->dispose = planified_task_dispose;

    //Init properties, see https://docs.gtk.org/gobject/class.ParamSpecGType.html?q=g_param_spec
    obj_properties[TASK_TEXT] =
            g_param_spec_string("task-text",
                                "PlanifiedTask text",
                                "Text of the task",
                                NULL,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    obj_properties[DEADLINE] =
            g_param_spec_pointer("deadline",
                                 "Deadline",
                                 "The pointer to GDateTime of the deadline of the task",
                                 G_PARAM_READWRITE);
    obj_properties[TIME_REQ] =
            g_param_spec_int("time-req",
                             "Time requirement",
                             "How much time will completing the task take (in minutes)?",
                             -1,
                             INT_MAX,
                             -1,
                             G_PARAM_READWRITE);
    obj_properties[TAGS] =
            g_param_spec_object("tags",
                                "Tags",
                                "GListStore of tags associated with the task",
                                G_TYPE_LIST_STORE,
                                G_PARAM_READWRITE);
    obj_properties[LOCATION] =
            g_param_spec_string("location",
                                "Location",
                                "Location associated with the task",
                                "",
                                G_PARAM_READWRITE);
    obj_properties[SCHEDULE] =
            g_param_spec_pointer("schedule",
                                 "Schedule",
                                 "The pointer to GDateTime of the exact time the task is scheduled to start (NULL if task is unscheduled)",
                                 G_PARAM_READWRITE);
    obj_properties[IS_COMPLETE] =
            g_param_spec_boolean("is-complete",
                                 "Is complete",
                                 "Is the tasked marked as completed?",
                                 false,
                                 G_PARAM_READWRITE);

    obj_properties[ROW_ID] =
            g_param_spec_int64("rowid",
                               "Row id",
                               "SQLite row id this task is recorded at",
                               INT64_MIN,
                               INT64_MAX,
                               0,
                               G_PARAM_READWRITE);
    obj_properties[DESCRIPTION] =
            g_param_spec_string("description",
                                "Description",
                                "Description of the task",
                                "",
                                G_PARAM_READWRITE);
    obj_properties[PLAN_START] =
            g_param_spec_pointer("plan-start",
                                 "Plan Start",
                                 "The pointer to GDateTime of the first date the task is planned for (NULL if task is unplanned)",
                                 G_PARAM_READWRITE);
    obj_properties[PLAN_SPAN] =
            g_param_spec_int("plan-span",
                             "Plan Span",
                             "How many days following `plan-start` is allocated for the task (if set to -1: until deadline or 0 days if the deadline is not set)",
                             -1,
                             INT_MAX,
                             -1,
                             G_PARAM_READWRITE);
    obj_properties[TAG_COUNT] =
            g_param_spec_int("tag-count",
                             "Tag Count",
                             "How many tags is linked to this task",
                             0,
                             INT_MAX,
                             0,
                             G_PARAM_READABLE);

    g_object_class_install_properties(G_OBJECT_CLASS(class), N_PROPERTIES, obj_properties);

    obj_signals[TASK_DELETED] =
            g_signal_newv("task-deleted",
                          G_TYPE_FROM_CLASS (object_class),
                          G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                          NULL /* closure */,
                          NULL /* accumulator */,
                          NULL /* accumulator data */,
                          NULL /* C marshaller */,
                          G_TYPE_NONE /* return_type */,
                          0     /* n_params */,
                          NULL /* param_types */);

}

char *planified_task_get_task_text(PlanifiedTask *self) {
    return self->task_text;
}

long long int planified_task_get_rowid(PlanifiedTask *self) {
    return self->rowid;
}

GDateTime *planified_task_get_deadline(PlanifiedTask *self) {
    return self->deadline;
}

gint planified_task_get_timereq(PlanifiedTask *self) {
    return self->time_req;
}

GDateTime *planified_task_get_schedule(PlanifiedTask *self) {
    return self->schedule;
}

GDateTime *planified_task_get_plan_start(PlanifiedTask *self) {
    return self->plan_start;
}

gint planified_task_get_plan_span(PlanifiedTask *self) {
    return self->plan_span;
}

gint planified_task_get_tag_count(PlanifiedTask *self) {
    return self->tag_count;
}

char *planified_task_get_location(PlanifiedTask *self) {
    return self->location;
}

gboolean planified_task_get_is_complete(PlanifiedTask *self) {
    return self->is_complete;
}

gchar *planified_task_get_description(PlanifiedTask *self) {
    return self->description;
}

GListStore *
planified_task_get_tags(PlanifiedTask *self) {
    return self->tags;
}


void planified_task_set_task_text(PlanifiedTask *self, gchar *task_text) {
    if (g_strcmp0(planified_task_get_task_text(self), task_text) == 0)
        return;
    g_object_set(G_OBJECT(self), "task-text", task_text, NULL);
}

void planified_task_set_deadline(PlanifiedTask *self, GDateTime *deadline) {
    if (planified_task_get_deadline(self) == deadline)
        return;
    g_object_set(self, "deadline", deadline, NULL);
}

void planified_task_set_time_req(PlanifiedTask *self, gint time_req) {
    if (planified_task_get_timereq(self) == time_req)
        return;
    g_object_set(G_OBJECT(self), "time-req", time_req, NULL);
}

void planified_task_set_tags(PlanifiedTask *self, GListStore *tags) {
    g_object_set(G_OBJECT(self), "tags", tags, NULL);
}

void planified_task_replace_tags_from_store(PlanifiedTask *self, GListStore *tags) {
    g_list_store_remove_all(self->tags);
    for (guint i = 0; i < g_list_model_get_n_items((GListModel *) tags); i++) {
        PlanifiedTag *tag = g_list_model_get_item((GListModel *) tags, i);
        g_list_store_append(self->tags, tag);
        g_object_unref(tag);
    }
}

void planified_task_set_location(PlanifiedTask *self, gchar *location) {
    if (g_strcmp0(planified_task_get_location(self), location) == 0)
        return;
    g_object_set(G_OBJECT(self), "location", location, NULL);
}

/*
 * Takes ownership of start_time
 */
void planified_task_set_schedule(PlanifiedTask *self, GDateTime *start_time) {
    if (planified_task_get_schedule(self) == start_time)
        return;
    g_object_set(self, "schedule", start_time, NULL);
}

/*
 * Sets rowid of the task.
 *
 * This method should only be used for unbound tasks
 */
void planified_task_set_rowid(PlanifiedTask *self, gint64 rowid) {
    if (planified_task_get_rowid(self) == rowid)
        return;
    g_object_set(G_OBJECT(self), "rowid", rowid, NULL);
}

void planified_task_set_plan_start(PlanifiedTask *self, GDateTime *plan_start) {
    if (planified_task_get_plan_start(self) == plan_start)
        return;
    g_object_set(self, "plan-start", plan_start, NULL);
}

void planified_task_set_plan_span(PlanifiedTask *self, gint plan_span) {
    if (planified_task_get_plan_span(self) == plan_span)
        return;
    g_object_set(G_OBJECT(self), "plan-span", plan_span, NULL);
}

void planified_task_set_is_complete(PlanifiedTask *self, gboolean is_complete) {
    if (planified_task_get_is_complete(self) == is_complete)
        return;
    g_object_set(G_OBJECT(self), "is_complete", is_complete, NULL);
}

void planified_task_set_description(PlanifiedTask *self, gchar *description) {
    if (g_strcmp0(planified_task_get_description(self), description) == 0)
        return;
    g_object_set(G_OBJECT(self), "description", description, NULL);
}

/*
 * Returns the most relevant date associated with this task, puts corresponding property name into `prop`
 *
 * Relevancy order:
 * Schedule
 * Plan_start
 * Deadline
 * NULL
 *
 */
GDateTime *planified_task_get_most_relevant_date(PlanifiedTask *self, gchar **prop) {
    if (self->schedule != NULL) {
        *prop = "schedule";
        return self->schedule;
    }
    if (self->plan_start != NULL) {
        *prop = "plan-start";
        return self->plan_start;
    }
    if (self->deadline != NULL) {
        *prop = "deadline";
        return self->deadline;
    }
    *prop = NULL;
    return NULL;
}

PlanifiedTask *
planified_task_new(char *task_text,
                   GDateTime *deadline,
                   int time_req,
                   GListStore *tags,
                   char *location,
                   GDateTime *schedule,
                   bool is_complete,
                   long long int rowid,
                   char *description,
                   GDateTime *plan_start,
                   gint plan_span) {

    return g_object_new(PLANIFIED_TASK_TYPE,
                        "task-text", task_text,
                        "deadline", deadline,
                        "time-req", time_req,
                        "tags", tags,
                        "location", location,
                        "schedule", schedule,
                        "is-complete", is_complete,
                        "rowid", rowid,
                        "description", description,
                        "plan-start", plan_start,
                        "plan-span", plan_span,
                        NULL);

}

void planified_task_update_from_sqlite3(PlanifiedTask *self, sqlite3_stmt *fetch_task_stmt) {
    planified_task_set_task_text(self, sqlite3_column_text(fetch_task_stmt, 1));
    planified_task_set_deadline(self, MAKE_DATETIME(sqlite3_column_int64(fetch_task_stmt, 2)));
    planified_task_set_time_req(self, sqlite3_column_int(fetch_task_stmt, 3));
    planified_task_set_location(self, sqlite3_column_text(fetch_task_stmt, 4));
    planified_task_set_schedule(self, MAKE_DATETIME(sqlite3_column_int64(fetch_task_stmt, 5)));
    planified_task_set_is_complete(self, sqlite3_column_int(fetch_task_stmt, 6));
    planified_task_set_description(self, sqlite3_column_text(fetch_task_stmt, 7));
    planified_task_set_plan_start(self, MAKE_DATETIME(sqlite3_column_int64(fetch_task_stmt, 8)));
    planified_task_set_plan_span(self, sqlite3_column_int(fetch_task_stmt, 9));
    self->tag_count = sqlite3_column_int(fetch_task_stmt, 10);
}

PlanifiedTask *
planified_task_new_from_sqlite(sqlite3_stmt *fetch_task_stmt) {
    PlanifiedTask *self = g_object_new(
            PLANIFIED_TASK_TYPE,
            "rowid", sqlite3_column_int64(fetch_task_stmt, 0),
            "task-text", sqlite3_column_text(fetch_task_stmt, 1),
            "deadline", MAKE_DATETIME(sqlite3_column_int64(fetch_task_stmt, 2)),
            "time-req", sqlite3_column_int(fetch_task_stmt, 3),
            "location", sqlite3_column_text(fetch_task_stmt, 4),
            "schedule", MAKE_DATETIME(sqlite3_column_int64(fetch_task_stmt, 5)),
            "is-complete", sqlite3_column_int(fetch_task_stmt, 6),
            "description", sqlite3_column_text(fetch_task_stmt, 7),
            "plan-start", MAKE_DATETIME(sqlite3_column_int64(fetch_task_stmt, 8)),
            "plan-span", sqlite3_column_int(fetch_task_stmt, 9),
            NULL
    );
    self->tag_count = sqlite3_column_int(fetch_task_stmt, 10);
    return self;
}
