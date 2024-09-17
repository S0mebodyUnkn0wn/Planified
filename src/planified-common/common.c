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

// y m d h m s ms
gint g_date_time_compare_fixed(GDateTime *a, GDateTime *b, PlanifiedComparisonPrecision precision) {
    guint c = 1;
    int res = 0;
    while (res == 0 && c <= precision) {
        int diff;
        switch ((PlanifiedComparisonPrecision) c) {
            case PLANIFIED_COMPARISON_PRECISION_YEAR:
                diff = (g_date_time_get_year(a) - g_date_time_get_year(b));
                break;
            case PLANIFIED_COMPARISON_PRECISION_MONTH:
                diff = (g_date_time_get_month(a) - g_date_time_get_month(b));
                break;
            case PLANIFIED_COMPARISON_PRECISION_DAY:
                diff = (g_date_time_get_day_of_month(a) - g_date_time_get_day_of_month(b));
                break;
            case PLANIFIED_COMPARISON_PRECISION_HOUR:
                diff = (g_date_time_get_hour(a) - g_date_time_get_hour(b));
                break;
            case PLANIFIED_COMPARISON_PRECISION_MINUTE:
                diff = (g_date_time_get_minute(a) - g_date_time_get_minute(b));
                break;
            case PLANIFIED_COMPARISON_PRECISION_SECOND:
                diff = (g_date_time_get_second(a) - g_date_time_get_second(b));
                break;
            case PLANIFIED_COMPARISON_PRECISION_MICROSECOND:
                diff = (g_date_time_get_microsecond(a) - g_date_time_get_microsecond(b));
                break;
            default:
                diff = 0;
        }
        res = diff == 0 ? 0 : (diff > 0 ? 1 : -1);
        c++;
    }
    return res;
}
