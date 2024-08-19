#include "common-widgets.h"
#include <gtk/gtk.h>

struct _CommonConfirmDialog {
    GtkWindow parent;

    GtkLabel *details_label;
    GtkButton *cancel_button;
    GtkButton *confirm_button;

    gboolean destructive_action;

    gulong confirm_callback;
    gulong cancel_callback;
};

typedef enum {
    CANCEL_TEXT = 1,
    CONFIRM_TEXT,
    TITLE_TEXT,
    DETAILS_TEXT,
    DESTRUCTIVE_ACTION,
    N_PROPERTIES
} CommonConfirmDialogProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};

G_DEFINE_FINAL_TYPE(CommonConfirmDialog, common_confirm_dialog, GTK_TYPE_WINDOW);

static void
common_confirm_dialog_set_property(GObject *object,
                                   guint property_id,
                                   const GValue *value,
                                   GParamSpec *pspec) {
    CommonConfirmDialog *self = COMMON_CONFIRM_DIALOG(object);


    switch ((CommonConfirmDialogProperty) property_id) {
        case (CANCEL_TEXT):
            if (g_value_get_string(value) != NULL)
                gtk_button_set_label(self->cancel_button, g_value_get_string(value));
            break;
        case (CONFIRM_TEXT):
            if (g_value_get_string(value) != NULL)
                gtk_button_set_label(self->confirm_button, g_value_get_string(value));
            break;
        case (TITLE_TEXT):
            if (g_value_get_string(value) != NULL)
                gtk_window_set_title((GtkWindow *) self, g_value_get_string(value));
            break;
        case (DETAILS_TEXT):
            if (g_value_get_string(value) != NULL)
                gtk_label_set_label(self->details_label, g_value_get_string(value));
            break;
        case (DESTRUCTIVE_ACTION):
            self->destructive_action = g_value_get_boolean(value);
            if (self->destructive_action)
                gtk_widget_add_css_class((GtkWidget *) self->confirm_button, "destructive-action");
            else
                gtk_widget_remove_css_class((GtkWidget *) self->confirm_button, "destructive-action");
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            return;
    }

}

static void
common_confirm_dialog_get_property(GObject *object,
                                   guint property_id,
                                   GValue *value,
                                   GParamSpec *pspec) {
    CommonConfirmDialog *self = COMMON_CONFIRM_DIALOG(object);

    switch ((CommonConfirmDialogProperty) property_id) {
        case (CANCEL_TEXT):
            g_value_set_string(value, gtk_button_get_label(self->cancel_button));
            break;
        case (CONFIRM_TEXT):
            g_value_set_string(value, gtk_button_get_label(self->confirm_button));
            break;
        case (TITLE_TEXT):
            g_value_set_string(value, gtk_window_get_title((GtkWindow *) self));
            break;
        case (DETAILS_TEXT):
            g_value_set_string(value, gtk_label_get_label(self->details_label));
            break;
        case (DESTRUCTIVE_ACTION):
            g_value_set_boolean(value, self->destructive_action);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
common_confirm_dialog_dispose(GObject *gobject) {

    G_OBJECT_CLASS (common_confirm_dialog_parent_class)->dispose(gobject);
}


static void
common_confirm_dialog_init(CommonConfirmDialog *self) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_default_size((GtkWindow *) self, 400, -1);
    gtk_window_set_child((GtkWindow *) self, box);
    GtkWidget *titlebar = gtk_header_bar_new();
    gtk_window_set_titlebar((GtkWindow *) self, titlebar);
    self->details_label = (GtkLabel *) gtk_label_new("details");
    gtk_label_set_wrap(self->details_label, TRUE);
    gtk_label_set_use_markup(self->details_label, TRUE);
    gtk_widget_set_hexpand((GtkWidget *) self->details_label, TRUE);
    gtk_label_set_natural_wrap_mode(self->details_label, GTK_NATURAL_WRAP_WORD);
    gtk_label_set_wrap_mode(self->details_label, PANGO_WRAP_WORD);
    gtk_label_set_justify(self->details_label, GTK_JUSTIFY_CENTER);
    gtk_widget_add_css_class((GtkWidget *) self->details_label, "confirm-dialog-details");
    gtk_widget_add_css_class((GtkWidget *) self->details_label, "details_text");

    gtk_box_append((GtkBox *) box, (GtkWidget *) self->details_label);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    self->cancel_button = (GtkButton *) gtk_button_new_with_label("Cancel");
    self->confirm_button = (GtkButton *) gtk_button_new_with_label("Confirm");
    gtk_box_append((GtkBox *) buttons, (GtkWidget *) self->cancel_button);
    gtk_widget_add_css_class((GtkWidget *) self->cancel_button, "button-row");
    gtk_widget_add_css_class((GtkWidget *) self->cancel_button, "confirm-dialog-button");
//    gtk_widget_add_css_class((GtkWidget *) self->cancel_button, "leftmost");
    gtk_box_append((GtkBox *) buttons, (GtkWidget *) self->confirm_button);
    gtk_widget_add_css_class((GtkWidget *) self->confirm_button, "button-row");
    gtk_widget_add_css_class((GtkWidget *) self->confirm_button, "confirm-dialog-button");
//    gtk_widget_add_css_class((GtkWidget *) self->confirm_button, "rightmost");
    gtk_box_append((GtkBox *) box, buttons);
    gtk_widget_set_vexpand((GtkWidget *) self->details_label, TRUE);
    gtk_widget_set_hexpand(buttons, TRUE);
    gtk_widget_set_hexpand((GtkWidget *) self->cancel_button, TRUE);
    gtk_widget_set_hexpand((GtkWidget *) self->confirm_button, TRUE);
//    gtk_window_set_default_widget((GtkWindow *) self, (GtkWidget *) self->cancel_button);
    g_signal_connect_swapped(self->confirm_button,"clicked",G_CALLBACK(gtk_window_close),self);
    g_signal_connect_swapped(self->cancel_button,"clicked",G_CALLBACK(gtk_window_close),self);
}

void
common_confirm_dialog_set_destructive_action(CommonConfirmDialog *self, gboolean is_destructive) {
    g_object_set(self, "destructive-action", is_destructive, NULL);
}

/*
 * user_data will be the only argument passed to the callback function
 */
void
common_confirm_dialog_connect_confirm_callback(CommonConfirmDialog *self, GCallback callback, gpointer user_data) {
    if (self->confirm_callback != 0)
        g_signal_handler_disconnect(self, self->confirm_callback);
    self->confirm_callback = g_signal_connect_swapped(self->confirm_button,"clicked",callback, user_data);
}

static void
common_confirm_dialog_class_init(CommonConfirmDialogClass *class) {


    GObjectClass *object_class = G_OBJECT_CLASS(class);
    object_class->dispose = common_confirm_dialog_dispose;
    object_class->get_property = common_confirm_dialog_get_property;
    object_class->set_property = common_confirm_dialog_set_property;

    obj_properties[CANCEL_TEXT] =
            g_param_spec_string("cancel-text",
                                "Cancel Text",
                                "Text displayed in dialog's cancel button (supports pango markup)",
                                "cancel",
                                G_PARAM_READWRITE);
    obj_properties[CONFIRM_TEXT] =
            g_param_spec_string("confirm-text",
                                "Confirm Text",
                                "Text displayed in dialog's confirm button (supports pango markup)",
                                "confirm",
                                G_PARAM_READWRITE);
    obj_properties[TITLE_TEXT] =
            g_param_spec_string("title-text",
                                "Title Text",
                                "Text displayed in dialog's title (supports pango markup)",
                                "Are you sure?",
                                G_PARAM_READWRITE);
    obj_properties[DETAILS_TEXT] =
            g_param_spec_string("details-text",
                                "Details Text",
                                "Text displayed in dialog's details label (supports pango markup)",
                                "",
                                G_PARAM_READWRITE);
    obj_properties[DESTRUCTIVE_ACTION] =
            g_param_spec_boolean("destructive-action",
                                 "Is Destructive Action?",
                                 "Should the confirm button be delayed as destructive action?",
                                 FALSE,
                                 G_PARAM_READWRITE);

    g_object_class_install_properties(G_OBJECT_CLASS(class), N_PROPERTIES, obj_properties);
}

CommonConfirmDialog *
common_confirm_dialog_new(GtkWindow *win, gchar *cancel_text, gchar *confirm_text, gchar *title_text,
                          gchar *details_text) {
    return g_object_new(COMMON_CONFIRM_DIALOG_TYPE,
                        "transient-for", win,
                        "modal", TRUE,
                        "cancel-text", cancel_text,
                        "confirm-text", confirm_text,
                        "title-text", title_text,
                        "details-text", details_text,
                        NULL
    );
}