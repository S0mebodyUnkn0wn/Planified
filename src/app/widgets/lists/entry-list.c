#include "app/widgets/lists/entry-list.h"


struct _PlanifiedEntryList
{
	GtkWidget parent;
	GListModel* list_model;
	GtkListView* entry_list_view;
};

typedef enum
{
	ENTRY_LIST_MODEL = 1,
	N_PROPERTIES,
} PlanifiedEntryListProperty;

static GParamSpec* obj_properties[N_PROPERTIES] = {nullptr,};

G_DEFINE_FINAL_TYPE(PlanifiedEntryList, planified_entry_list, GTK_TYPE_BOX)

static void
planified_entry_list_set_property(GObject* object,
                                  guint property_id,
                                  const GValue* value,
                                  GParamSpec* pspec)
{
	PlanifiedEntryList* self = PLANIFIED_ENTRY_LIST(object);
	switch ((PlanifiedEntryListProperty)property_id)
	{
	case ENTRY_LIST_MODEL:
		if (self->list_model != nullptr)
			g_object_unref(self->list_model);
		self->list_model = g_value_get_object(value);
		gtk_list_view_set_model(self->entry_list_view, GTK_SELECTION_MODEL(gtk_single_selection_new(self->list_model)));
	// g_print("test\n");
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void
planified_entry_list_get_property(GObject* object,
                                  guint property_id,
                                  GValue* value,
                                  GParamSpec* pspec)
{
	PlanifiedEntryList* self = PLANIFIED_ENTRY_LIST(object);
	switch ((PlanifiedEntryListProperty)property_id)
	{
	case ENTRY_LIST_MODEL:
		g_value_set_object(value, self->list_model);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void
setup_list_item(GtkSignalListItemFactory* self,
                GtkListItem* item,
                gpointer user_data)
{
	PlanifiedEntryContainer* container = planified_entry_container_new();
	gtk_list_item_set_child(item,GTK_WIDGET(container));
}

static void
bind_list_item(GtkSignalListItemFactory* self,
               GtkListItem* item,
               gpointer user_data)
{
	PlanifiedEntry* entry = gtk_list_item_get_item(item);
	PlanifiedEntryContainer* container = PLANIFIED_ENTRY_CONTAINER(gtk_list_item_get_child(item));
	planified_entry_container_set_entry(container, entry);
}

static void
unbind_list_item(GtkSignalListItemFactory* self,
                 GtkListItem* item,
                 gpointer user_data)
{
	PlanifiedEntryContainer* container = PLANIFIED_ENTRY_CONTAINER(gtk_list_item_get_child(item));
	planified_entry_container_set_entry(container, nullptr);
}

static void
teardown_list_item(GtkSignalListItemFactory* self,
                   GtkListItem* item,
                   gpointer user_data)
{
	gtk_list_item_set_child(item, nullptr);
}

static void
planified_entry_list_init(PlanifiedEntryList* self)
{
	gtk_widget_init_template(GTK_WIDGET(self));
	GtkListItemFactory* entry_list_item_factory = gtk_signal_list_item_factory_new();
	g_signal_connect(entry_list_item_factory, "setup", G_CALLBACK (setup_list_item), NULL);
	g_signal_connect(entry_list_item_factory, "bind", G_CALLBACK (bind_list_item), NULL);

	gtk_list_view_set_factory(self->entry_list_view, entry_list_item_factory);
}

static void
planified_entry_list_class_init(PlanifiedEntryListClass* klass)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(klass),
	                                            "/planified/app/entry-list.ui");
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), PlanifiedEntryList, entry_list_view);

	GObjectClass* object_class = G_OBJECT_CLASS(klass);
	object_class->set_property = planified_entry_list_set_property;
	object_class->get_property = planified_entry_list_get_property;
	obj_properties[ENTRY_LIST_MODEL] =
		g_param_spec_object("list-model",
		                    "List Model",
		                    "The list model of entries to be used by the list",
		                    G_TYPE_LIST_MODEL,
		                    G_PARAM_READWRITE);

	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

PlanifiedEntryList*
planified_entry_list_new()
{
	return g_object_new(PLANIFIED_TYPE_ENTRY_LIST, nullptr);
}
