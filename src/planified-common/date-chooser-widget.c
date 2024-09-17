//
// Created by somebody on 15/07/24.
//
#include <gtk/gtk.h>
#include "common-widgets.h"


struct _CommonDateChooser {
    GtkEntry parent;

    GtkWidget *date_selector_popover;
    GtkWidget *date_chooser_calendar;
};

G_DEFINE_TYPE (CommonDateChooser, common_date_chooser, GTK_TYPE_ENTRY)


static void
common_date_chooser_dispose(GObject *gobject) {
    CommonDateChooser *self = COMMON_DATE_CHOOSER(gobject);

    gtk_widget_dispose_template(GTK_WIDGET(self), COMMON_DATE_CHOOSER_TYPE);

    G_OBJECT_CLASS (common_date_chooser_parent_class)->dispose(gobject);
}

static void
date_selected(GtkCalendar *calendar,
              gpointer user_data) {
    CommonDateChooser *self = COMMON_DATE_CHOOSER(user_data);
    GDateTime *selected_date = gtk_calendar_get_date(calendar);
    gtk_editable_set_text(GTK_EDITABLE(self), g_date_time_format(selected_date, "%d/%m/%Y"));
    g_date_time_unref(selected_date);

}

static void
open_date_picker(CommonDateChooser *self) {

    GDateTime *chosen_date = common_date_chooser_get_chosen_date(self);
    if (chosen_date != NULL) {
        gtk_calendar_select_day(GTK_CALENDAR(self->date_chooser_calendar), chosen_date);
        g_date_time_unref(chosen_date);
    } else {
        date_selected((GtkCalendar *) self->date_chooser_calendar, self);
    }
    gtk_popover_popup(GTK_POPOVER(self->date_selector_popover));

}

static void
common_date_chooser_init(CommonDateChooser *self) {
    gtk_widget_init_template(GTK_WIDGET(self));


    g_signal_connect_swapped(self, "icon-press", G_CALLBACK(open_date_picker),
                             self);
    g_signal_connect(self->date_chooser_calendar, "day-selected", G_CALLBACK(date_selected), self);

}

static void
common_date_chooser_class_init(CommonDateChooserClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/common/date-chooser-widget.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), CommonDateChooser, date_chooser_calendar);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), CommonDateChooser, date_selector_popover);

    GObjectClass *object_class = G_OBJECT_CLASS(class);

    object_class->dispose = common_date_chooser_dispose;
}


gboolean
common_date_chooser_is_date_chosen(CommonDateChooser *self) {
    return gtk_entry_get_text_length(GTK_ENTRY(self)) > 0;
}

/*
 * NULL is returned if no date is chosen, or entered date is invalid
 *
 * The caller of the function assumes ownership of the data, and is responsible for freeing it
 */
GDateTime *
common_date_chooser_get_chosen_date(CommonDateChooser *self) {

    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(self));
    char **bits = g_regex_split_simple("[^[:digit:]]", text, 0, 0);
    if (g_strv_length(bits) == 3) {
        return g_date_time_new_local((int) strtol(bits[2], NULL, 10),
                                     (int) strtol(bits[1], NULL, 10),
                                     (int) strtol(bits[0], NULL, 10), 0, 0, 0);
    }
    return NULL;
}


CommonDateChooser *
common_date_chooser_new() {
    return g_object_new(COMMON_DATE_CHOOSER_TYPE, NULL);
}

