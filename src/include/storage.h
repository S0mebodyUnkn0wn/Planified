//
// Created by somebody on 05/05/24.
//

#ifndef PLANIFIED_STORAGE_H
#define PLANIFIED_STORAGE_H

#include <sqlite3.h>
#include "common.h"

#define TASKS_NAME "tasks"
#define TASKS_SCHEMA \
        "task_id INTEGER PRIMARY KEY, \
        task TEXT, \
        deadline INT DEFAULT -1, \
        timeReq INT DEFAULT 0, \
        location TEXT, \
        schedule INT DEFAULT -1, \
        is_complete INT DEFAULT 0, \
        description TEXT, \
        plan_start INT DEFAULT -1, \
        plan_span INT DEFAULT -1, \
        tag_count INT DEFAULT 0\
        "

#define TAGS_NAME "tags"
#define TAGS_SCHEMA \
        "name TEXT PRIMARY KEY," \
        "color INTEGER DEFAULT 8947848"

#define TAG_ASSIGNMENTS_NAME "tag_assignments"
#define TAG_ASSIGNMENTS_SCHEMA \
        "tagname TEXT REFERENCES tags(name) ON DELETE CASCADE ON UPDATE CASCADE," \
        "taskid INTEGER REFERENCES tasks ON DELETE CASCADE"


int database_check_version(sqlite3 *handle);

int migrate_to_present(sqlite3 *handle);

int init_sqlite_db(sqlite3 **db_pointer, char *DB_FILEPATH);

int database_insert_task(sqlite3 *handle, PlanifiedTask *task, gint64 *rowid_ptr);

int database_update_task(sqlite3 *handle, PlanifiedTask *task, gint64 *rowid_ptr);

int database_delete_task(sqlite3 *handle, PlanifiedTask *task);

int database_delete_task_by_rowid(sqlite3 *handle, gint64 rowid);

int database_link_tag_to_task(sqlite3 *handle, PlanifiedTag *tag, PlanifiedTask *task);

int database_unlink_tag_from_task(sqlite3 *handle, PlanifiedTag *tag, PlanifiedTask *task);

int database_delete_tag(sqlite3 *handle, PlanifiedTag *tag);

int database_get_task_by_rowid(sqlite3 *handle, gint64 rowid);

int database_add_tag(sqlite3 *handle, PlanifiedTag *tag);

PlanifiedTag *
database_get_tag_by_name(sqlite3 *handle, gchar *tag_name);

gboolean database_check_for_tag(sqlite3 *handle, gchar *tag_name);

/*
 * Returns an array of `PlanifiedTag`s linked to `task`
 *
 * The caller of the function assumes ownership of the return value and is responsible for freeing it
 */
GListStore *
database_get_tags_of_task(sqlite3 *handle, PlanifiedTask *task);


/*
 * Returns an array of `PlanifiedTask`s linked to `tag`
 *
 * The caller of the function assumes ownership of the return value and is responsible for freeing it
 */
GPtrArray *
database_get_tasks_by_tag(sqlite3 *handle, PlanifiedTag *tag);


#endif //PLANIFIED_STORAGE_H
