#include "app/widgets/containers/entry-container.h"

typedef struct
{
	PlanifiedEntry* entry;
	GtkLabel* title;
	GtkLabel* description;
	GtkCheckButton* complete_button;
} PlanifiedEntryContainerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(PlanifiedEntryContainer, planified_entry_container, GTK_TYPE_BOX)

static void
planified_entry_container_init(PlanifiedEntryContainer* self)
{
	gtk_widget_init_template(GTK_WIDGET(self));
}

void planified_entry_container_set_entry(PlanifiedEntryContainer* self, PlanifiedEntry* entry)
{
	PlanifiedEntryContainerPrivate* priv = planified_entry_container_get_instance_private(self);
	if (priv->entry != nullptr)
		g_object_unref(priv->entry);
	priv->entry = entry;
	if (priv->entry != nullptr)
	{
		g_object_ref(priv->entry);
		g_object_bind_property(entry, "title", priv->title, "label", G_BINDING_SYNC_CREATE);
		g_object_bind_property(entry, "description", priv->description, "label", G_BINDING_SYNC_CREATE);
	}
	if (PLANIFIED_IS_TASK(priv->entry))
	{
		gtk_widget_set_visible(GTK_WIDGET(priv->complete_button),true);
	}
	else
	{
		gtk_widget_set_visible(GTK_WIDGET(priv->complete_button),false);
	}
}

static void
planified_entry_container_class_init(PlanifiedEntryContainerClass* klass)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(klass), "/planified/app/entry-container.ui");

	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), PlanifiedEntryContainer, title);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), PlanifiedEntryContainer, description);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), PlanifiedEntryContainer, complete_button);
}

PlanifiedEntryContainer* planified_entry_container_new()
{
	return g_object_new(PLANIFIED_TYPE_ENTRY_CONTAINER, nullptr);
}
