//
// Created by somebody on 05/05/24.
//

#ifndef PLANIFIED_WIDGETS_H
#define PLANIFIED_WIDGETS_H

#include <gtk/gtk.h>
#include "app.h"
#include "common.h"
#include "derivable-widgets.h"
// CLASS DECLARATIONS


#define PLANIFIED_TASK_WIDGET_TYPE (planified_task_widget_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedTaskWidget, planified_task_widget, PLANIFIED, TASK_WIDGET, PlanifiedTaskContainer)


#define PLANIFIED_TIMETABLE_VIEW_TYPE (planified_timetable_view_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedTimetableView, planified_timetable_view, PLANIFIED, TIMETABLE_VIEW, GtkBox)


#define PLANIFIED_ITINERARY_WIDGET_TYPE (planified_itinerary_widget_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedItineraryWidget, planified_itinerary_widget, PLANIFIED, ITINERARY_WIDGET, GtkGrid)


#define PLANIFIED_HOME_VIEW_TYPE (planified_home_view_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedHomeView, planified_home_view, PLANIFIED, HOME_VIEW, GtkBox)


#define PLANIFIED_ITINERARY_WIDGET_ENTRY_TYPE (planified_itinerary_widget_entry_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedItineraryWidgetEntry, planified_itinerary_widget_entry, PLANIFIED, ITINERARY_WIDGET_ENTRY,
                     PlanifiedTaskContainer)


#define PLANIFIED_WEATHER_WIDGET_TYPE (planified_weather_widget_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedWeatherWidget, planified_weather_widget, PLANIFIED, WEATHER_WIDGET, GtkBox)


#define PLANIFIED_AGENDA_WIDGET_TYPE (planified_agenda_widget_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedAgendaWidget, planified_agenda_widget, PLANIFIED, AGENDA_WIDGET, GtkBox)


#define PLANIFIED_UPCOMING_LIST_TYPE (planified_upcoming_list_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedUpcomingList, planified_upcoming_list, PLANIFIED, UPCOMING_LIST, GtkBox)

#define PLANIFIED_UPCOMING_LIST_ITEM_TYPE (planified_upcoming_list_item_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedUpcomingListItem, planified_upcoming_list_item, PLANIFIED, UPCOMING_LIST_ITEM, GtkBox)


#define PLANIFIED_TAG_CONTAINER_TYPE (planified_tag_container_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedTagContainer, planified_tag_container, PLANIFIED, TAG_CONTAINER, GtkBox)

#define PLANIFIED_PREFERENCES_DIALOG_TYPE (planified_preferences_dialog_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedPreferencesDialog, planified_preferences_dialog, PLANIFIED, PREFERENCES_DIALOG,
                     GtkApplicationWindow)


#define PLANIFIED_PLANNING_VIEW_TYPE (planified_planning_view_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedPlanningView, planified_planning_view, PLANIFIED, PLANNING_VIEW, GtkBox)


#define PLANIFIED_TASK_LIST_TYPE (planified_task_list_get_type())

G_DECLARE_FINAL_TYPE(PlanifiedTaskList, planified_task_list, PLANIFIED, TASK_LIST, GtkBox)

// METHOD DECLARATIONS

// PlanifiedTaskWidget methods:

PlanifiedTaskWidget *
planified_task_widget_new(PlanifiedTask *task);

/*
 * Sets linked-timetable-slot to `slot`
 *
 * To compete the link `slot` should set linked-task to `self`
 */
void planified_task_widget_link_timetable_slot(PlanifiedTaskWidget *self, PlanifiedItineraryWidgetEntry *slot);

void planified_task_widget_unlink_timetable_slot(PlanifiedTaskWidget *self);

PlanifiedItineraryWidgetEntry *planified_task_widget_get_linked_timetable_slot(PlanifiedTaskWidget *self);

void planified_task_widget_delete_task(PlanifiedTaskWidget *self);

PlanifiedTask *
planified_task_widget_get_task(PlanifiedTaskWidget *self);

// PlanifiedTimetableView Methods:

PlanifiedTimetableView *
planified_timetable_view_new();

GtkListBox *
planified_timetable_view_get_task_list(PlanifiedTimetableView *self);

void

planified_timetable_view_setup(PlanifiedTimetableView *self);

void
planified_timetable_view_update_row(GObject *_app, int operation, PlanifiedTask *updated_task, gpointer *_self);


// PlanifiedItineraryWidget Methods:

PlanifiedItineraryWidget *
planified_itinerary_widget_new();

GDateTime *
planified_itinerary_widget_get_selected_date(PlanifiedItineraryWidget *self);

void
planified_itinerary_widget_set_selected_date(PlanifiedItineraryWidget *self, GDateTime *date);

void
planified_itinerary_widget_shift_selected_date(PlanifiedItineraryWidget *self, int shift_by);

void
planified_itinerary_widget_go_to_next_day(PlanifiedItineraryWidget *self);

void
planified_itinerary_widget_go_to_prev_day(PlanifiedItineraryWidget *self);

void
planified_itinerary_widget_refresh_data(GObject *_app, int operation, PlanifiedTask *updated_task, gpointer *_self);

void
planified_itinerary_widget_setup(PlanifiedItineraryWidget *self);


// PlanifiedItineraryWidgetEntry Methods:

PlanifiedItineraryWidgetEntry *
planified_itinerary_widget_entry_new(PlanifiedTask *task, gint64 time);

PlanifiedTask *
planified_itinerary_widget_entry_get_task(PlanifiedItineraryWidgetEntry *self);


// PlainfieldAgendaWidget Methods:

void
planified_agenda_widget_shift_open_day(PlanifiedAgendaWidget *self, int shift_by);

void
planified_agenda_widget_go_to_next_day(PlanifiedAgendaWidget *self);

void
planified_agenda_widget_go_to_prev_day(PlanifiedAgendaWidget *self);

void
planified_agenda_widget_refresh_data(PlanifiedAgendaWidget *self);

PlanifiedAgendaWidget *
planified_agenda_widget_new();

// PlanifiedHomeView Methods:

PlanifiedHomeView *
planified_home_view_new();

void
planified_home_view_setup(PlanifiedHomeView *self);


// PlanifiedWeatherWidget Methods:

// PlanifiedUpcomingList Methods:
void
planified_upcoming_list_setup(PlanifiedUpcomingList *self);


// PlanifiedUpcomingListItem Methods:
GtkLabel *
planified_upcoming_list_item_get_title(PlanifiedUpcomingListItem *self);

GtkLabel *
planified_upcoming_list_item_get_description(PlanifiedUpcomingListItem *self);

GtkWidget *
planified_upcoming_list_item_new();

void
planified_upcoming_list_item_bind(PlanifiedUpcomingListItem *self, PlanifiedTask *task);

void
planified_upcoming_list_item_unbind(PlanifiedUpcomingListItem *self);


// PlanifiedTagContainer Methods:

PlanifiedTagContainer *
planified_tag_container_new(PlanifiedTag *tag, gboolean deletable);

PlanifiedTagContainer *
planified_tag_container_new_entry(gboolean deletable);

PlanifiedTag *
planified_tag_container_get_tag(PlanifiedTagContainer *self);


// PlanifiedPreferencesDialog Methods:

GtkWidget *
planified_preferences_dialog_new(GtkWindow *win, GtkApplication *app);


// PlanifiedTimetableView Methods:

GtkWidget *
planified_planning_view_new();


// PlanifiedTaskList Methods:

void
planified_task_list_setup(PlanifiedTaskList *self);

// PlanifiedPlanningView Methods:

void
planified_planning_view_setup(PlanifiedPlanningView *self);

PlanifiedTask *
planified_task_list_get_selected(PlanifiedTaskList *self);

const GtkListView *
planified_task_list_get_list_view(PlanifiedTaskList *self);

GListModel *
planified_task_list_get_model(PlanifiedTaskList *self);

#endif //PLANIFIED_WIDGETS_H
