//
// Created by somebody on 05/05/24.
//
#include "storage.h"
#include "common.h"
#include <sqlite3.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>

extern gchar *SCHEMA_VERSION;

int init_sqlite_db(sqlite3 **db_pointer, char *DB_FILEPATH) {
    int db_stat = sqlite3_open(DB_FILEPATH, db_pointer);
    if (db_stat != SQLITE_OK) {
        sqlite3_close(*db_pointer);
        return db_stat;
    }
    if (database_check_version(*db_pointer) < strtol(SCHEMA_VERSION, NULL, 10)) {
        migrate_to_present(*db_pointer);
    }
    sqlite3_exec(*db_pointer,
                 "PRAGMA foreign_keys = ON;",
                 NULL,
                 NULL,
                 NULL
    );
    sqlite3_exec(*db_pointer,
                 "CREATE TABLE IF NOT EXISTS "
                 TAGS_NAME
                 "("
                 TAGS_SCHEMA
                 ");",
                 NULL,
                 NULL,
                 NULL
    );

    sqlite3_exec(*db_pointer,
                 "CREATE TABLE IF NOT EXISTS "
                 TASKS_NAME
                 "("
                 TASKS_SCHEMA
                 ");",
                 NULL,
                 NULL,
                 NULL
    );
    sqlite3_exec(*db_pointer,
                 "CREATE TABLE IF NOT EXISTS "
                 TAG_ASSIGNMENTS_NAME
                 "("
                 TAG_ASSIGNMENTS_SCHEMA
                 ");",
                 NULL,
                 NULL,
                 NULL
    );
    // TODO: create a trigger to touch relevant task on tag modifications
    sqlite3_exec(*db_pointer,
                 "CREATE TRIGGER IF NOT EXISTS"
                 "tag_count_up AFTER INSERT ON tag_assignments"
                 "BEGIN"
                 "UPDATE tasks SET tag_count = tag_count+1 WHERE task_id=new.taskid;"
                 "END",
                 NULL,NULL,NULL);

    g_print("DB init successful\n");
//
    return db_stat;
}

int database_check_version(sqlite3 *handle) {
    sqlite3_stmt *check_version_stmt;
    sqlite3_prepare_v2(handle, "PRAGMA user_version", -1, &check_version_stmt, NULL);
    sqlite3_step(check_version_stmt);
    int version = sqlite3_column_int(check_version_stmt, 0);
    sqlite3_finalize(check_version_stmt);
    return version;
}

static void bind_stmt_from_task(sqlite3_stmt *statement, PlanifiedTask *task) {
    gchar *task_text = planified_task_get_task_text(task);
    gchar *location = planified_task_get_location(task);
    gchar *description = planified_task_get_description(task);

    g_print("%d", sqlite3_bind_text(statement, 1, task_text, -1, SQLITE_TRANSIENT));
    g_print("%d", sqlite3_bind_int64(statement, 2, DATETIME_TO_UNIX_S(planified_task_get_deadline(task))));
    g_print("%d", sqlite3_bind_int(statement, 3, planified_task_get_timereq(task)));
    g_print("%d", sqlite3_bind_text(statement, 4, location, -1, SQLITE_TRANSIENT));
    g_print("%d", sqlite3_bind_int64(statement, 5, DATETIME_TO_UNIX_S(planified_task_get_schedule(task))));
    g_print("%d", sqlite3_bind_int(statement, 6, (int) planified_task_get_is_complete(task)));
    g_print("%d", sqlite3_bind_text(statement, 7, description, -1, SQLITE_TRANSIENT));

//    g_free(task_text);
//    g_free(location);
//    g_free(description);
}

static void bind_stmt_from_tag(sqlite3_stmt *statement, PlanifiedTag *tag) {
    const gchar *name = planified_tag_get_name(tag);
    sqlite3_bind_text(statement, 1, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 2, (int) planified_tag_get_color(tag));
    g_free((void *) name);
}

int database_insert_task(sqlite3 *handle, PlanifiedTask *task, gint64 *rowid_ptr) {
    sqlite3_stmt *insert_statement;
    sqlite3_prepare_v2(handle,
                       "INSERT INTO tasks (task,deadline,timeReq,location,schedule,is_complete,description)"
                       "VALUES (?, ?, ?, ?, ?, ?, ?) RETURNING rowid;", -1, &insert_statement, NULL);


    bind_stmt_from_task(insert_statement, task);
    int status = sqlite3_step(insert_statement);
    if (rowid_ptr != NULL) {
        *rowid_ptr = sqlite3_column_int64(insert_statement, 0);
    }
    g_print("Insert status: %d\n", status);
    sqlite3_finalize(insert_statement);
    return status;
}

int database_update_task(sqlite3 *handle, PlanifiedTask *task, gint64 *rowid_ptr) {
    sqlite3_stmt *update_statement;
    sqlite3_prepare_v2(handle,
                       "UPDATE tasks "
                       "SET task = ?, deadline = ?, timeReq = ?, location = ?, schedule = ?, is_complete = ?, description = ?"
                       "WHERE ROWID == ? RETURNING rowid;",
                       -1, &update_statement, NULL);
//    g_print("Statement code: %d\n",err);
    bind_stmt_from_task(update_statement, task);
    sqlite3_bind_int64(update_statement, 8, planified_task_get_rowid(task));
//    g_print("Statement bound\n");
    int status = sqlite3_step(update_statement);
//    g_print("Statement execd (%d)\n",status);
    if (rowid_ptr != NULL) {
        *rowid_ptr = sqlite3_column_int64(update_statement, 0);
    }
    sqlite3_finalize(update_statement);
//    g_print("Edit status: %d\n",status);
    return status;
}

gboolean database_check_for_tag(sqlite3 *handle, gchar *tag_name) {
    sqlite3_stmt *check_stmt;
    sqlite3_prepare_v2(handle,
                       "SELECT count() FROM tags WHERE name=?;", -1, &check_stmt, NULL);
    sqlite3_bind_text(check_stmt, 1, tag_name, -1, SQLITE_STATIC);
    sqlite3_step(check_stmt);
    gboolean is_in = sqlite3_column_int(check_stmt, 0);
    sqlite3_finalize(check_stmt);
    return is_in;
}

PlanifiedTag *
database_get_tag_by_name(sqlite3 *handle, gchar *tag_name) {
    sqlite3_stmt *fetch_stmt;
    sqlite3_prepare_v2(handle,
                       "SELECT * FROM tags WHERE name=?;", -1, &fetch_stmt, NULL);
    sqlite3_bind_text(fetch_stmt, 1, tag_name, -1, SQLITE_STATIC);
    if (sqlite3_step(fetch_stmt) == SQLITE_ROW) {
        PlanifiedTag *tag = planified_tag_new_from_sqlite(fetch_stmt);
        return tag;
    } else return NULL;
}

int database_add_tag(sqlite3 *handle, PlanifiedTag *tag) {
    sqlite3_stmt *add_stmt;
    sqlite3_prepare_v2(handle,
                       "INSERT INTO tags (name,color)"
                       "VALUES (?,?);", -1, &add_stmt, NULL);
    bind_stmt_from_tag(add_stmt, tag);
    int status = sqlite3_step(add_stmt);
    sqlite3_finalize(add_stmt);
    return status;
}


int database_delete_tag(sqlite3 *handle, PlanifiedTag *tag) {
    sqlite3_stmt *delete_stmt;
    sqlite3_prepare_v2(handle,
                       "DELETE FROM tags WHERE name=?", -1, &delete_stmt, NULL);
    const gchar *tag_name = planified_tag_get_name(tag);
    sqlite3_bind_text(delete_stmt, 1, tag_name, -1, SQLITE_STATIC);
    int status = sqlite3_step(delete_stmt);
    sqlite3_finalize(delete_stmt);
    g_free((void *) tag_name);
    return status;
}


int database_link_tag_to_task(sqlite3 *handle, PlanifiedTag *tag, PlanifiedTask *task) {
    sqlite3_stmt *link_stmt;
    if (!database_check_for_tag(handle, planified_tag_get_name(tag))) {
        database_add_tag(handle, tag);
    };

    sqlite3_prepare_v2(handle,
                       "INSERT INTO tag_assignments (tagname,taskid)"
                       "VALUES (?,?);", -1, &link_stmt, NULL);
    const gchar *tag_name = planified_tag_get_name(tag);
    gint64 task_id = planified_task_get_rowid(task);
    sqlite3_bind_text(link_stmt, 1, tag_name, -1, SQLITE_STATIC);
    sqlite3_bind_int64(link_stmt, 2, task_id);
    int status = sqlite3_step(link_stmt);
//    g_print("status: %d, task_id %d\n",status,task_id);
//    g_print("%s",sqlite3_errmsg(handle));
    sqlite3_finalize(link_stmt);
    g_free((void *) tag_name);
    return status;
}


int database_unlink_tag_from_task(sqlite3 *handle, PlanifiedTag *tag, PlanifiedTask *task) {
    sqlite3_stmt *unlink_stmt;
    sqlite3_prepare_v2(handle,
                       "DELETE FORM tag_assignments WHERE (tagname==? AND taskid==?)", -1, &unlink_stmt, NULL);
    const gchar *tag_name = planified_tag_get_name(tag);
    gint64 task_id = planified_task_get_rowid(task);
    sqlite3_bind_text(unlink_stmt, 1, tag_name, -1, SQLITE_STATIC);
    sqlite3_bind_int64(unlink_stmt, 2, task_id);
    int status = sqlite3_step(unlink_stmt);
    sqlite3_finalize(unlink_stmt);
    g_free((void *) tag_name);
    return status;
}

/*
 * Returns an array of `PlanifiedTag`s linked to `task`
 *
 * The caller of the function assumes ownership of the return value and is responsible for freeing it
 */
GListStore *
database_get_tags_of_task(sqlite3 *handle, PlanifiedTask *task) {
    sqlite3_stmt *fetch_stmt;
    sqlite3_prepare_v2(handle, "SELECT * FROM tags "
                               "WHERE name IN ("
                               "SELECT tagname FROM tag_assignments WHERE taskid==?"
                               ")",
                       -1, &fetch_stmt, NULL);
    sqlite3_bind_int64(fetch_stmt, 1, planified_task_get_rowid(task));
    GListStore *tags = g_list_store_new(PLANIFIED_TAG_TYPE);
    g_print("Processing %s (%d)\n", planified_task_get_task_text(task), planified_task_get_rowid(task));
    while (sqlite3_step(fetch_stmt) == SQLITE_ROW) {
        g_print("\t!");
        PlanifiedTag *tag = planified_tag_new_from_sqlite(fetch_stmt);
        g_list_store_append(tags, tag);
        g_object_unref(tag);
    }
    g_print("\n");
    sqlite3_finalize(fetch_stmt);
    return tags;
}

/*
 * Returns an array of `PlanifiedTask`s linked to `tag`
 *
 * The caller of the function assumes ownership of the return value and is responsible for freeing it
 */
GPtrArray *
database_get_tasks_by_tag(sqlite3 *handle, PlanifiedTag *tag) {
    sqlite3_stmt *fetch_stmt;
    sqlite3_prepare_v2(handle, "SELECT * FROM tasks "
                               "WHERE id IN ("
                               "SELECT taskid FROM tag_assignments WHERE tagname==?"
                               ")",
                       -1, &fetch_stmt, NULL);
    const gchar *tag_name = planified_tag_get_name(tag);
    sqlite3_bind_text(fetch_stmt, 1, tag_name, -1, SQLITE_STATIC);
    GPtrArray *tasks = g_ptr_array_new_full(0, g_object_unref);

    while (sqlite3_step(fetch_stmt) == SQLITE_ROW) {
        PlanifiedTask *task = planified_task_new_from_sqlite(fetch_stmt);
        g_ptr_array_add(tasks, task);
    }
    sqlite3_finalize(fetch_stmt);
    g_free((void *) tag_name);
    return tasks;
}


int database_delete_task_by_rowid(sqlite3 *handle, gint64 rowid) {
    sqlite3_stmt *delete_statement;

    sqlite3_prepare_v2(handle, "DELETE FROM tasks WHERE ROWID==?;", -1, &delete_statement, NULL);
    sqlite3_bind_int64(delete_statement, 1, rowid);
    int status = sqlite3_step(delete_statement);
    sqlite3_finalize(delete_statement);
    g_print("deletion status: %d\n", status);
    return status;
}

int database_get_task_by_rowid(sqlite3 *handle, gint64 rowid) {
    sqlite3_stmt *select_stmt;

    sqlite3_prepare_v2(handle, "SELECT * FROM tasks WHERE ROWID==?;", -1, &select_stmt, NULL);
    sqlite3_bind_int64(select_stmt, 1, rowid);
    int status = sqlite3_step(select_stmt);
    sqlite3_finalize(select_stmt);
    return status;
}


int database_delete_task(sqlite3 *handle, PlanifiedTask *task) {
    return database_delete_task_by_rowid(handle, planified_task_get_rowid(task));
}
