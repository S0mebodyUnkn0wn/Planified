#include <gtk/gtk.h>
#include "common-widgets.h"

struct _CommonEntry{
    GtkEntry parent;

    GtkPopoverMenu *suggestion_menu;
    GtkListView *suggestion_list;
};

G_DEFINE_FINAL_TYPE(CommonEntry, common_entry, GTK_TYPE_ENTRY)

static void
common_entry_init(CommonEntry *self){

}

static void
common_entry_class_init(CommonEntryClass *class){

}

GtkWidget *
common_entry_new(){
    return g_object_new(COMMON_ENTRY_TYPE,NULL);
}
