#include <gtk/gtk.h>
#include "widgets.h"
#include "common.h"
#include "app.h"
#include "storage.h"
#include "common-widgets.h"


typedef enum {
    TAG = 1,
    DELETABLE,
    N_PROPERTIES
} PlanifiedTagContainerProperty;

typedef enum {
    DELETE_TAG = 1,
    CONFIRMED,
    N_SIGNALS
} PlanifiedTagContainerSignal;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};

static guint obj_signals[N_SIGNALS];

struct _PlanifiedTagContainer {
    GtkBox parent;

    PlanifiedTag *tag;
    gboolean deletable;

    GtkWidget *tag_name;
    GtkWidget *delete_button;
};


G_DEFINE_FINAL_TYPE(PlanifiedTagContainer, planified_tag_container, GTK_TYPE_BOX)


static void
planified_tag_container_init(PlanifiedTagContainer *self) {
    self->tag_name = gtk_label_new("");
    self->delete_button = NULL;
    gtk_widget_set_halign((GtkWidget *) self, GTK_ALIGN_START);
    gtk_widget_add_css_class((GtkWidget *) self, "inline_button");
    gtk_box_set_spacing((GtkBox *) self, 5);
    GtkWidget *tag_icon = gtk_image_new_from_icon_name("tag");
    gtk_box_append((GtkBox *) self, tag_icon);
    gtk_widget_add_css_class(tag_icon,"tiny_font");
    gtk_widget_add_css_class(tag_icon,"inline_icon");
    gtk_box_append((GtkBox *) self, self->tag_name);
}

static void
refresh_data(GObject *_tag, GParamSpec *pspec, gpointer _self) {
    PlanifiedTagContainer *self = PLANIFIED_TAG_CONTAINER(_self);
    if (PLANIFIED_IS_TAG(self->tag)) {
        gtk_label_set_label((GtkLabel *) self->tag_name, planified_tag_get_name(self->tag));
    }
}

static void
confirm_entry(PlanifiedTagContainer *self) {
    if (gtk_editable_label_get_editing((GtkEditableLabel *) self->tag_name) == FALSE) {
//        g_signal_handlers_disconnect_by_data(self->tag_name, self); Should be disconnected automatically
        gchar *name = (gchar *) gtk_editable_get_text((GtkEditable *) self->tag_name);
        if (strlen(name) == 0)
            g_signal_emit(self, obj_signals[DELETE_TAG], 0);
        else {
            GtkWidget *t = gtk_label_new("");
            gtk_box_insert_child_after((GtkBox *) self, t, self->tag_name);
            gtk_box_remove((GtkBox *) self, self->tag_name);
            self->tag_name = t;
            GValue tag_val = G_VALUE_INIT;
            g_value_init(&tag_val, PLANIFIED_TAG_TYPE);
            g_value_set_object(&tag_val, planified_tag_new(name));
            g_object_set_property((GObject *) self, "tag", &tag_val);
            g_value_unset(&tag_val);
            g_signal_emit(self, obj_signals[CONFIRMED], 0);
        }
    }
}

static void
setup_entry(PlanifiedTagContainer *self) {
    GtkWidget *t = gtk_editable_label_new("");
    gtk_box_insert_child_after((GtkBox *) self, t, self->tag_name);
    gtk_box_remove((GtkBox *) self, self->tag_name);
    self->tag_name = t;
    gtk_editable_label_start_editing((GtkEditableLabel *) self->tag_name);
    gtk_widget_set_size_request(self->tag_name, 25, -1);
    gtk_widget_set_halign((GtkWidget *) self->tag_name, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand((GtkWidget *) self->tag_name, TRUE);
    gtk_widget_set_vexpand((GtkWidget *) self->tag_name, TRUE);

    g_signal_connect_swapped(self->tag_name, "notify::editing", G_CALLBACK(confirm_entry), self);
}

static void
planified_tag_container_set_property(GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec) {
    PlanifiedTagContainer *self = PLANIFIED_TAG_CONTAINER(object);

    switch ((PlanifiedTagContainerProperty) property_id) {
        case TAG:
            if (self->tag != NULL) {
                g_signal_handlers_disconnect_by_data(self->tag, self);
                g_object_unref(self->tag);
            }
            self->tag = g_value_get_object(value);
            if (PLANIFIED_IS_TAG(self->tag)) {
                g_signal_connect(self->tag, "notify", G_CALLBACK(refresh_data), self);
                refresh_data(NULL, NULL, self);
            }
            break;
        case DELETABLE:
            self->deletable = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            return;
    }

}

static void
planified_tag_container_get_property(GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec) {
    PlanifiedTagContainer *self = PLANIFIED_TAG_CONTAINER(object);

    switch ((PlanifiedTagContainerProperty) property_id) {
        case TAG:
            g_value_take_object(value, self->tag);
            break;
        case DELETABLE:
            g_value_set_boolean(value, self->deletable);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            return;
    }

}

static void
planified_tag_container_dispose(GObject *gobject) {
    PlanifiedTagContainer *self = PLANIFIED_TAG_CONTAINER(gobject);
    if (self->tag != NULL)
        g_object_unref(self->tag);
    G_OBJECT_CLASS (planified_tag_container_parent_class)->dispose(gobject);
}

static void
show(GtkWidget *widget) {
    gtk_widget_set_visible(widget, TRUE);
}

static void
hide(GtkWidget *widget) {
    gtk_widget_set_visible(widget, FALSE);
}

static void
delete_cb(GtkButton *btn,
          gpointer _self) {
    g_signal_emit(_self, obj_signals[DELETE_TAG], 0);
    g_print("DEL!\n");
}

static void
planified_tag_container_constructed(GObject *gobject) {
    PlanifiedTagContainer *self = PLANIFIED_TAG_CONTAINER(gobject);
    if (self->deletable) {
        self->delete_button = gtk_button_new_from_icon_name("delete");
        gtk_widget_add_css_class(self->delete_button, "flat");
        gtk_widget_add_css_class(self->delete_button, "inline_button");
//        gtk_widget_add_css_class(self->delete_button, "tall");
        gtk_widget_set_halign(self->delete_button, GTK_ALIGN_END);
        gtk_widget_set_vexpand(self->delete_button, FALSE);
        gtk_widget_set_visible(self->delete_button, FALSE);
        gtk_button_set_can_shrink((GtkButton *) self->delete_button, TRUE);
        gtk_box_append((GtkBox *) self, self->delete_button);
        GtkEventController *mouseover = gtk_event_controller_motion_new();
        g_signal_connect(self->delete_button, "clicked", G_CALLBACK(delete_cb), self);
        g_signal_connect_swapped(mouseover, "enter", G_CALLBACK(show), self->delete_button);
        g_signal_connect_swapped(mouseover, "leave", G_CALLBACK(hide), self->delete_button);
        gtk_widget_add_controller((GtkWidget *) self, mouseover);
    }
}

PlanifiedTag *
planified_tag_container_get_tag(PlanifiedTagContainer *self) {
    return self->tag;
}


static void
planified_tag_container_class_init(PlanifiedTagContainerClass *class) {
    obj_properties[TAG] =
            g_param_spec_object("tag",
                                "Tag",
                                "Tag object that is represented by this widget",
                                PLANIFIED_TAG_TYPE,
                                G_PARAM_READWRITE);
    obj_properties[DELETABLE] =
            g_param_spec_boolean("deletable",
                                 "Deletable",
                                 "Is a delete action appropriate in the current context",
                                 FALSE,
                                 G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->set_property = planified_tag_container_set_property;
    object_class->get_property = planified_tag_container_get_property;
    object_class->dispose = planified_tag_container_dispose;
    object_class->constructed = planified_tag_container_constructed;
    gtk_widget_class_set_css_name((GtkWidgetClass *) class, "button");

    g_object_class_install_properties(G_OBJECT_CLASS(class), N_PROPERTIES, obj_properties);

    obj_signals[DELETE_TAG] =
            g_signal_newv("delete-tag",
                          G_TYPE_FROM_CLASS (object_class),
                          G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                          NULL /* closure */,
                          NULL /* accumulator */,
                          NULL /* accumulator data */,
                          NULL /* C marshaller */,
                          G_TYPE_NONE /* return_type */,
                          0     /* n_params */,
                          NULL /* param_types */);
    obj_signals[CONFIRMED] =
            g_signal_newv("confirmed",
                          G_TYPE_FROM_CLASS (object_class),
                          G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                          NULL /* closure */,
                          NULL /* accumulator */,
                          NULL /* accumulator data */,
                          NULL /* C marshaller */,
                          G_TYPE_NONE /* return_type */,
                          0     /* n_params */,
                          NULL/* param_types */);
}

PlanifiedTagContainer *
planified_tag_container_new(PlanifiedTag *tag, gboolean deletable) {
    return g_object_new(PLANIFIED_TAG_CONTAINER_TYPE, "tag", tag, "deletable", deletable, NULL);
}

PlanifiedTagContainer *
planified_tag_container_new_entry(gboolean deletable) {
    PlanifiedTagContainer *self = g_object_new(PLANIFIED_TAG_CONTAINER_TYPE, "deletable", deletable, NULL);
    setup_entry(self);
    return self;
}

