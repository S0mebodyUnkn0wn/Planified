#include <glib.h>
#include <stdlib.h>
#include "common.h"

typedef enum {
    TAG_NAME = 1,
    TAG_COLOR,
    N_PROPERTIES
} PlanifiedTagProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};

typedef struct {
    gchar *name;
    guint8 color;

} PlanifiedTagPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(PlanifiedTag, planified_tag, G_TYPE_OBJECT)


static void
planified_tag_set_property(GObject *object,
                           guint property_id,
                           const GValue *value,
                           GParamSpec *pspec) {
    PlanifiedTagPrivate *self = planified_tag_get_instance_private(PLANIFIED_TAG(object));

    switch ((PlanifiedTagProperty) property_id) {
        case TAG_NAME:
            g_free(self->name);
            self->name = g_value_dup_string(value);
            break;
        case TAG_COLOR:
            self->color = g_value_get_uint(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
planified_tag_get_property(GObject *object,
                           guint property_id,
                           GValue *value,
                           GParamSpec *pspec) {

    PlanifiedTagPrivate *self = planified_tag_get_instance_private(PLANIFIED_TAG(object));

    switch ((PlanifiedTagProperty) property_id) {
        case TAG_NAME:
            g_value_set_string(value, self->name);
            break;
        case TAG_COLOR:
            g_value_set_uint(value, self->color);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
planified_tag_class_init(PlanifiedTagClass *class) {
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->set_property = planified_tag_set_property;
    object_class->get_property = planified_tag_get_property;

    obj_properties[TAG_NAME] =
            g_param_spec_string("tag-name",
                                "Tag name",
                                "Name of the tag, must be unique among other tags",
                                NULL  /* default value */,
                                G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    obj_properties[TAG_COLOR] =
            g_param_spec_uint("tag-color",
                              "Tag color",
                              "6-digit hexadecimal number corresponding to the color associated with the tag (opacity not supported)",
                              0,
                              0xFFFFFF,
                              0x888888,
                              G_PARAM_READWRITE);

    g_object_class_install_properties(G_OBJECT_CLASS(class), N_PROPERTIES, obj_properties);


}

void planified_tag_set_color(PlanifiedTag *self, guint color) {
    GValue val = G_VALUE_INIT;
    g_value_init(&val, G_TYPE_UINT);
    g_value_set_uint(&val, color);
    g_object_set_property(G_OBJECT(self), "tag-color", &val);
    g_value_unset(&val);
}

guint planified_tag_get_color(PlanifiedTag *self) {
    GValue val = G_VALUE_INIT;
    g_value_init(&val, G_TYPE_UINT);
    g_object_get_property(G_OBJECT(self), "tag-color", &val);
    guint res = g_value_get_uint(&val);
    g_value_unset(&val);
    return res;
}

const gchar *planified_tag_get_name(PlanifiedTag *self) {
    GValue val = G_VALUE_INIT;
    g_value_init(&val, G_TYPE_STRING);
    g_object_get_property(G_OBJECT(self), "tag-name", &val);
    const gchar *res = g_value_dup_string(&val);
    g_value_unset(&val);
    return res;
}

static void
planified_tag_init(PlanifiedTag *self) {
    PlanifiedTagPrivate *priv = planified_tag_get_instance_private(self);

}

PlanifiedTag *
planified_tag_new_from_sqlite(sqlite3_stmt *fetch_stmt){
    return g_object_new(PLANIFIED_TAG_TYPE,
                        "tag-name", sqlite3_column_text(fetch_stmt, 0),
                        "tag-color", sqlite3_column_int(fetch_stmt, 1), NULL);

}

PlanifiedTag *
planified_tag_new_full(gchar *tag_name, guint tag_color) {
    return g_object_new(PLANIFIED_TAG_TYPE, "tag-name", tag_name, "tag-color", tag_color, NULL);
}

PlanifiedTag *
planified_tag_new(gchar *tag_name) {
    return g_object_new(PLANIFIED_TAG_TYPE, "tag-name", tag_name, NULL);
}
