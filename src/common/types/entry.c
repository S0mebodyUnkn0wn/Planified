#include <common/types/entry.h>

typedef enum
{
	PROP_TITLE = 1,
	PROP_DESCRIPTION,
	PROP_META,
	N_PROPERTIES
} PlanifiedEntryProperty;

static GParamSpec* obj_properties[N_PROPERTIES] = {NULL,};

typedef struct
{
	gchar* title;
	gchar* description;
	void* metadata;
} PlanifiedEntryPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(PlanifiedEntry, planified_entry, G_TYPE_OBJECT)

static void
planified_entry_set_property(GObject* object,
                             guint property_id,
                             const GValue* value,
                             GParamSpec* pspec)
{
	PlanifiedEntryPrivate* priv = planified_entry_get_instance_private(PLANIFIED_ENTRY(object));
	switch ((PlanifiedEntryProperty)property_id)
	{
	case PROP_TITLE:
		g_free(priv->title);
		priv->title = g_value_dup_string(value);
		break;
	case PROP_DESCRIPTION:
		g_free(priv->description);
		priv->description = g_value_dup_string(value);
		break;
	case PROP_META:
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void
planified_entry_get_property(GObject* object,
                             guint property_id,
                             GValue* value,
                             GParamSpec* pspec)
{
	PlanifiedEntryPrivate* priv = planified_entry_get_instance_private(PLANIFIED_ENTRY(object));
	switch ((PlanifiedEntryProperty)property_id)
	{
	case PROP_TITLE:
		g_value_set_string(value, priv->title);
		break;
	case PROP_DESCRIPTION:
		g_value_set_string(value, priv->description);
		break;
	case PROP_META:
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void
planified_entry_class_init(PlanifiedEntryClass* klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS(klass);
	object_class->set_property = planified_entry_set_property;
	object_class->get_property = planified_entry_get_property;
	obj_properties[PROP_TITLE] =
		g_param_spec_string("title",
		                    "Title",
		                    "The title of the entry",
		                    NULL,
		                    G_PARAM_READWRITE);
	obj_properties[PROP_DESCRIPTION] =
		g_param_spec_string("description",
		                    "Description",
		                    "The description of the entry",
		                    NULL,
		                    G_PARAM_READWRITE);


	obj_properties[PROP_META] =
		g_param_spec_string("meta",
							"Meta",
							"Placeholder for metadata property",
							NULL,
							G_PARAM_READWRITE);

	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

static void
planified_entry_init(PlanifiedEntry* self)
{
}

PlanifiedEntry*
planified_entry_new(gchar* name, gchar* description)
{
	return g_object_new(PLANIFIED_TYPE_ENTRY,
	                    "name", name,
	                    "description", description,
	                    NULL);
};
