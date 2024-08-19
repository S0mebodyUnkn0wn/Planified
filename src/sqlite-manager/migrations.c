
#include "storage.h"
#include "common.h"
#include <sqlite3.h>
#include <gtk/gtk.h>

const gchar *SCHEMA_VERSION = "3"; //HAS TO BE A NON-NEGATIVE INTEGER

int migrate_to_present(sqlite3 *handle) {
    int old_version = database_check_version(handle);
    sqlite3_exec(handle, "BEGIN", NULL, NULL, NULL);
    g_print("Outdated database schema detected (version %d). Starting migration to current version (%s)\n",old_version,SCHEMA_VERSION);

    int panic = 0;
    switch (old_version + 1) {
        /*
         * Below follows a list of actions required to migrate to each of database versions.
         *
         * Migration actions to migrate from version `x` to version `x+1` should be put under
         * case `x+1`:
         *
         * Case statements should not terminate with break
         */
        case 1:
            panic = sqlite3_exec(handle,"ALTER TABLE tasks DROP COLUMN tags;",NULL,NULL,NULL);

            if (panic!=SQLITE_OK)
                break;
        case 2:
            panic = sqlite3_exec(handle,"ALTER TABLE tasks Drop COLUMN is_event;",NULL,NULL,NULL);
            if (panic!=SQLITE_OK)
                break;
            panic = sqlite3_exec(handle,"ALTER TABLE tasks ADD COLUMN description TEXT;",NULL,NULL,NULL);
            if (panic!=SQLITE_OK)
                break;
        case 3:
            panic = sqlite3_exec(handle,"ALTER TABLE tasks ADD COLUMN plan_start INT;",NULL,NULL,NULL);
            if (panic!=SQLITE_OK)
                break;
            panic = sqlite3_exec(handle,"ALTER TABLE tasks ADD COLUMN plan_span INT;",NULL,NULL,NULL);
            if (panic!=SQLITE_OK)
                break;
            panic = sqlite3_exec(handle,"ALTER TABLE tasks ADD COLUMN tag_count INT;",NULL,NULL,NULL);
            if (panic!=SQLITE_OK)
                break;

    }
    if (panic != 0) {
        g_printerr("Failed to migrate database from version %d to version %d!\n"
                   "Error code: %d\n"
                   "To retain you data the migration was cancelled. "
                   "Please migrate manually or delete your database file (backup your data first!)"
                   "so an up-to-date one can be created\n");
        sqlite3_exec(handle, "ROLLBACK", NULL, NULL, NULL);
        sqlite3_close_v2(handle);
        return panic;
    }
    gchar set_version[32] = "PRAGMA user_version=";
    strcat(set_version, SCHEMA_VERSION);
    sqlite3_exec(handle, set_version, NULL, NULL, NULL);
    sqlite3_exec(handle, "COMMIT", NULL, NULL, NULL);
    g_print("Migrated successfully!\n");
    return SQLITE_OK;
}
