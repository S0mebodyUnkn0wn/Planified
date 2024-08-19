//
// Created by somebody on 02/05/24.
//

#ifndef PLANIFIED_COMMON_H
#define PLANIFIED_COMMON_H

#include <time.h>
#include <gtk/gtk.h>
#include <sqlite3.h>

/*
* Moves `row` to `index`
*/
void gtk_list_box_move_row(GtkListBox *self, GtkWidget *row, int index);


typedef time_t Timestamp;

#define PLANIFIED_TASK_TYPE (planified_task_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedTask, planified_task, PLANIFIED, TASK, GObject)

#define PLANIFIED_TAG_TYPE (planified_tag_get_type())

G_DECLARE_DERIVABLE_TYPE(PlanifiedTag, planified_tag, PLANIFIED, TAG, GObject)

struct _PlanifiedTagClass {
    GObjectClass parent_class;

    gpointer padding[6];
};

PlanifiedTag *
planified_tag_new(gchar *tag_name);

void planified_tag_set_color(PlanifiedTag *self, guint color);

guint planified_tag_get_color(PlanifiedTag *self);

const gchar *planified_tag_get_name(PlanifiedTag *self);

PlanifiedTag *
planified_tag_new_full(gchar *tag_name, guint tag_color);


long long int planified_task_get_rowid(PlanifiedTask *self);

gint64 planified_task_get_deadline(PlanifiedTask *self);

gint planified_task_get_timereq(PlanifiedTask *self);

char *planified_task_get_task_text(PlanifiedTask *self);

gint64 planified_task_get_schedule(PlanifiedTask *self);

char *planified_task_get_location(PlanifiedTask *self);

gchar *planified_task_get_description(PlanifiedTask *self);

gboolean planified_task_get_is_complete(PlanifiedTask *self);

GListStore *planified_task_get_tags(PlanifiedTask *self);

void planified_task_replace_tags_from_store(PlanifiedTask *self, GListStore *tags);

void planified_task_set_rowid(PlanifiedTask *self, gint64 rowid);

void planified_task_set_task_text(PlanifiedTask *self, gchar *task_text);

void planified_task_set_deadline(PlanifiedTask *self, gint64 deadline);

void planified_task_set_time_req(PlanifiedTask *self, gint time_req);

void planified_task_set_tags(PlanifiedTask *self, GListStore *tags);

void planified_task_set_location(PlanifiedTask *self, gchar *location);

void planified_task_set_schedule(PlanifiedTask *self, gint64 start_time);

void planified_task_set_is_complete(PlanifiedTask *self, gboolean is_complete);

void planified_task_set_description(PlanifiedTask *self, gchar *description);

PlanifiedTask *
planified_task_new(char *task_text,
                   long long int deadline,
                   int time_req,
                   GListStore *tags,
                   char *location,
                   long long int schedule,
                   bool is_complete,
                   long long int rowid,
                   char *description,
                   gint64 plan_start,
                   gint plan_span);

void planified_task_update_from_sqlite3(PlanifiedTask *self, sqlite3_stmt *fetch_task_stmt);

PlanifiedTask *
planified_task_new_from_sqlite(sqlite3_stmt *fetch_task_stmt);

PlanifiedTag *
planified_tag_new_from_sqlite(sqlite3_stmt *fetch_stmt);

#endif //PLANIFIED_COMMON_H
