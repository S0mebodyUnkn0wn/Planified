#ifndef DBSHEMA_H
#define DBSHEMA_H

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

#endif //DBSHEMA_H
