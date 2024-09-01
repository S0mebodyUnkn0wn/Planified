//
// Created by somebody on 02/05/24.
//
#include "common.h"
#include <gtk/gtk.h>

/*
 * Moves row to index
 */
void gtk_list_box_move_row(GtkListBox *self, GtkWidget *row, int index) {
    // Grab GtkListBoxRow associated with timetableWidgetRow, move it to `index`
    GtkWidget *_row = GTK_WIDGET(gtk_widget_get_ancestor(GTK_WIDGET(row), GTK_TYPE_LIST_BOX_ROW));
    g_object_ref(_row); // Increase row's ref count, so it doesn't die when we remove it from the list box
    gtk_list_box_remove(GTK_LIST_BOX(self), _row);
    gtk_list_box_insert(GTK_LIST_BOX(self), _row, index);
    g_object_unref(_row); // Restore row's ref count

}

gchar* get_quantifier(int number) {
    switch ((number % 10) * (number < 10 || number >= 14)) {
        case 1:
            return "st:";
        case 2:
            return "nd:";
        case 3:
            return "rd:";
        default:
            return "th:";
    }
}
