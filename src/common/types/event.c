#include <common/types/event.h>
#include <gtk/gtk.h>

typedef struct
{
	GDateTime* start_time;
	guint64 duration;
} PlanifiedEventPrivate;

typedef enum
{
	PROP_START_TIME = 1,
	PROP_DURATION,
	N_PROPERTIES
} PlanifiedEventProperty;

// static GParamSpec* obj_properties[N_PROPERTIES] = {NULL,};

static void planified_event_plannable_interface_init(PlanifiedPlannableInterface* iface);

G_DEFINE_TYPE_WITH_CODE(
	PlanifiedEvent, planified_event, PLANIFIED_TYPE_ENTRY,
	G_ADD_PRIVATE(PlanifiedEvent)
	G_IMPLEMENT_INTERFACE(PLANIFIED_TYPE_PLANNABLE,
		planified_event_plannable_interface_init)
)

static void
planified_event_set_property(GObject* object,
                             guint prop_id,
                             const GValue* value,
                             GParamSpec* pspec)
{
	PlanifiedEvent* self = PLANIFIED_EVENT(object);
	PlanifiedEventPrivate* priv = planified_event_get_instance_private(self);
	switch ((PlanifiedEventProperty)prop_id)
	{
	case PROP_START_TIME:
		g_free(priv->start_time);
		priv->start_time = g_value_dup_object(value);
		break;
	case PROP_DURATION:
		priv->duration = g_value_get_uint64(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void
planified_event_get_property(GObject* object,
                             guint prop_id,
                             GValue* value,
                             GParamSpec* pspec)
{
	PlanifiedEvent* self = PLANIFIED_EVENT(object);
	PlanifiedEventPrivate* priv = planified_event_get_instance_private(self);
	switch ((PlanifiedEventProperty)prop_id)
	{
	case PROP_START_TIME:
		g_value_set_object(value, priv->start_time);
		break;
	case PROP_DURATION:
		g_value_set_uint64(value, priv->duration);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static bool
planified_event_editable_plan(PlanifiedPlannable* plannable, GDateTime* start_time)
{
	PlanifiedEvent* self = PLANIFIED_EVENT(plannable);
	return false;
}

static bool
planified_event_plannable_unplan(PlanifiedPlannable* plannable)
{
	PlanifiedEvent* self = PLANIFIED_EVENT(plannable);
	return false;
}


static void
planified_event_plannable_interface_init(PlanifiedPlannableInterface* iface)
{
	iface->plan = planified_event_editable_plan;
	iface->unplan = planified_event_plannable_unplan;
}

static void
planified_event_class_init(PlanifiedEventClass* klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS(klass);
	object_class->set_property = planified_event_set_property;
	object_class->get_property = planified_event_get_property;

	g_object_class_override_property(object_class, PROP_START_TIME, "start-time");
	g_object_class_override_property(object_class, PROP_DURATION, "duration");

}

PlanifiedEvent* planified_event_new(void)
{
	return g_object_new(PLANIFIED_TYPE_EVENT, NULL);
}

static void
planified_event_init(PlanifiedEvent* self)
{
	g_assert(PLANIFIED_IS_PLANNABLE(self));
	PlanifiedPlannable* plannable = PLANIFIED_PLANNABLE(self);
	planified_plannable_unplan(plannable);
}
