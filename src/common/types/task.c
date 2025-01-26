#include <common/types/task.h>

// TODO: Define Task analogous to Event.
//		 Task may be a descendant of Entry, or of Event, not decided yet
//		 Task should implement Planable and Completable interfaces


typedef struct
{
	GDateTime* deadline;
	PlanifiedTimePrecision deadline_precision;
	bool is_completed;
	GDateTime* planned_time;
	PlanifiedTimePrecision planned_precision;
	guint64 duration;
} PlanifiedTaskPrivate;

typedef enum
{
	PROP_IS_COMPLETED = 1,
	PROP_DEADLINE,
	PROP_DEADLINE_PRECISION,
	PROP_PLANNED_TIME,
	PROP_DURATION,
	PROP_PLANNED_PRECISION,
} PlanifiedTaskProperty;

static void planified_task_completable_interface_init(PlanifiedCompletableInterface* iface);
static void planified_task_planable_interface_init(PlanifiedPlanableInterface* iface);

G_DEFINE_TYPE_WITH_CODE(
	PlanifiedTask, planified_task, PLANIFIED_TYPE_ENTRY,
	G_ADD_PRIVATE(PlanifiedTask)
	G_IMPLEMENT_INTERFACE(PLANIFIED_TYPE_COMPLETABLE,
		planified_task_completable_interface_init)
	G_IMPLEMENT_INTERFACE(PLANIFIED_TYPE_PLANABLE,
		planified_task_planable_interface_init)
)

static void planified_task_set_property(GObject* object,
                                        guint prop_id,
                                        const GValue* value,
                                        GParamSpec* pspec)
{
	PlanifiedTask* self = PLANIFIED_TASK(object);
	PlanifiedTaskPrivate* priv = planified_task_get_instance_private(self);
	switch ((PlanifiedTaskProperty)prop_id)
	{
	case PROP_IS_COMPLETED:
		priv->is_completed = g_value_get_boolean(value);
		break;
	case PROP_DEADLINE:
		g_free(priv->deadline);
		priv->deadline = g_value_dup_boxed(value);
		break;
	case PROP_DEADLINE_PRECISION:
		priv->deadline_precision = g_value_get_enum(value);
		break;
	case PROP_PLANNED_TIME:
		g_free(priv->planned_time);
		priv->planned_time = g_value_dup_boxed(value);
		break;
	case PROP_DURATION:
		priv->duration = g_value_get_uint64(value);
		break;
	case PROP_PLANNED_PRECISION:
		priv->planned_precision = g_value_get_enum(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void planified_task_get_property(GObject* object,
                                        guint prop_id,
                                        GValue* value,
                                        GParamSpec* pspec)
{
	PlanifiedTask* self = PLANIFIED_TASK(object);
	PlanifiedTaskPrivate* priv = planified_task_get_instance_private(self);
	switch ((PlanifiedTaskProperty)prop_id)
	{
	case PROP_DEADLINE:
		// ? will boxed work?
		g_value_set_boxed(value, priv->deadline);
		break;
	case PROP_IS_COMPLETED:
		g_value_set_boolean(value, priv->is_completed);
		break;
	case PROP_DEADLINE_PRECISION:
		g_value_set_enum(value, priv->deadline_precision);
		break;
	case PROP_PLANNED_TIME:
		// ? will boxed work?
		g_value_set_boxed(value, priv->planned_time);
		break;
	case PROP_DURATION:
		g_value_set_uint64(value, priv->duration);
		break;
	case PROP_PLANNED_PRECISION:
		g_value_set_enum(value, priv->planned_precision);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}


static bool
planified_task_editable_plan(PlanifiedPlanable* planable, GDateTime* plan_time, PlanifiedTimePrecision precision)
{
	g_object_set(G_OBJECT(planable),
	             "planned-time", plan_time,
	             "planned-time-precision", precision,
	             NULL);
	return true;
}

static bool
planified_task_planable_unplan(PlanifiedPlanable* planable)
{
	g_object_set(G_OBJECT(planable),
	             "planned-time", NULL,
	             NULL);
	return true;
}

static void
planified_task_planable_interface_init(PlanifiedPlanableInterface* iface)
{
	iface->plan = planified_task_editable_plan;
	iface->unplan = planified_task_planable_unplan;
}

static void
planified_task_completable_interface_init(PlanifiedCompletableInterface* iface)
{
}

static void
planified_task_init(PlanifiedTask* self)
{
}

static void
planified_task_class_init(PlanifiedTaskClass* klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS(klass);
	object_class->set_property = planified_task_set_property;
	object_class->get_property = planified_task_get_property;

	g_object_class_override_property(object_class, PROP_IS_COMPLETED, "is-completed");
	g_object_class_override_property(object_class, PROP_DEADLINE, "deadline");
	g_object_class_override_property(object_class, PROP_DEADLINE_PRECISION, "deadline-precision");
	g_object_class_override_property(object_class, PROP_PLANNED_TIME, "planned-time");
	g_object_class_override_property(object_class, PROP_DURATION, "duration");
	g_object_class_override_property(object_class, PROP_PLANNED_PRECISION, "planned-time-precision");
}

PlanifiedTask*
planified_task_new(gchar* name, gchar* description)
{
	return g_object_new(PLANIFIED_TYPE_TASK,
	                    "title", name,
	                    "description", description,
	                    nullptr);
}
