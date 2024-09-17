/**
 * Headers for commonly used widgets that can be used outside of planified.
 * Basically everything that defined in planified-common and is a widget
 */

#ifndef PLANIFIED_COMMON_WIDGETS_H
#define PLANIFIED_COMMON_WIDGETS_H
#include <gtk/gtk.h>

// CLASS DECLARATIONS

#define COMMON_DATE_CHOOSER_TYPE (common_date_chooser_get_type())

G_DECLARE_FINAL_TYPE (CommonDateChooser, common_date_chooser, COMMON, DATE_CHOOSER, GtkEntry)


#define COMMON_CALENDAR_TYPE (common_calendar_get_type())

G_DECLARE_FINAL_TYPE(CommonCalendar, common_calendar, COMMON, CELNDAR, GtkWidget)


#define COMMON_ENTRY_TYPE (common_entry_get_type())

G_DECLARE_FINAL_TYPE(CommonEntry, common_entry, COMMON, ENTRY, GtkEntry )

#define COMMON_CONFIRM_DIALOG_TYPE (common_confirm_dialog_get_type())

G_DECLARE_FINAL_TYPE(CommonConfirmDialog, common_confirm_dialog, COMMON, CONFIRM_DIALOG, GtkWindow)


// METHOD DECLARATIONS

// CommonDateChooser

CommonDateChooser *
common_date_chooser_new();

gboolean
common_date_chooser_is_date_chosen(CommonDateChooser *self);

GDateTime *
common_date_chooser_get_chosen_date(CommonDateChooser *self);

// CommonEntry

GtkWidget *
common_entry_new();

// CommonConfirmDialog

CommonConfirmDialog *
common_confirm_dialog_new(GtkWindow *win, gchar *cancel_text, gchar *confirm_text, gchar *title_text,
                          gchar *details_text);

void
common_confirm_dialog_connect_confirm_callback(CommonConfirmDialog *self, GCallback callback, gpointer user_data);

void
common_confirm_dialog_set_destructive_action(CommonConfirmDialog *self, gboolean is_destructive);


#endif //PLANIFIED_COMMON_WIDGETS_H
