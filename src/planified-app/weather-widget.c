//
// Created by somebody on 17/06/24.
//
#include <gtk/gtk.h>
#include <libgweather/gweather.h>
#include "widgets.h"


struct _PlanifiedWeatherWidget {
    GtkBox parent;
    GSettings *settings;


    GtkWidget *weather_icon;
    GtkWidget *weather_summary;
    GtkWidget *weather_temperature;

    GWeatherLocation *location;
    GWeatherInfo *weather_info;

    guint weather_updater_id;
};

G_DEFINE_TYPE(PlanifiedWeatherWidget, planified_weather_widget, GTK_TYPE_BOX);


static void
update_weather_labels(
        GWeatherInfo *weather_info,
        gpointer user_data) {
    g_print("Updating weather labels...\n");
    PlanifiedWeatherWidget *self = user_data;

    // We can pull an icon name straight from the info, cool!
    // We use symbolic icons (which is pulled from breeze in most cases)
    // because some icon themes screw up their weather icons horribly
    gtk_image_set_from_icon_name(GTK_IMAGE(self->weather_icon),
                                 g_settings_get_boolean(self->settings, "weather-symbolic-icons")
                                 ?
                                 gweather_info_get_symbolic_icon_name(weather_info)
                                 :
                                 gweather_info_get_icon_name(weather_info));


    gtk_label_set_label(GTK_LABEL(self->weather_temperature), gweather_info_get_temp(weather_info));

    // We'll create our own summary, so we can allow user to customise it later
    GString *summary = g_string_new("");

    char *cond = gweather_info_get_conditions(weather_info);
    char *sky = gweather_info_get_sky(weather_info);
    if (g_str_equal(cond, "-")) {
        g_string_append(summary, sky);
    } else {
        g_string_append(summary, cond);
    }
    if (g_settings_get_boolean(self->settings, "weather-show-station")) {
        g_string_append(summary, "\n<span size=\"50%\" font_style=\"italic\">Closest weather station: ");
        g_string_append(summary, gweather_info_get_location_name(weather_info));
        g_string_append(summary, "</span>");
    }

    gtk_label_set_label(GTK_LABEL(self->weather_summary), summary->str);


}

static void
planified_weather_widget_init(PlanifiedWeatherWidget *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
    self->settings = g_settings_new("com.github.somebodyunknown.planified");
    gtk_label_set_label(GTK_LABEL(self->weather_summary),
                        "<span size=\"75%\" style=\"italic\">Could not fetch weather information</span>");
    gtk_label_set_label(GTK_LABEL(self->weather_temperature), "");

    GWeatherLocation *world = gweather_location_get_world();
    double lat, lon;
    if (!g_settings_get_boolean(self->settings, "weather-autolocation")) {
        g_settings_get(self->settings, "weather-location", "(dd)", &lat, &lon);
    } else {
        lat = 0;
        lon = 0;
    }
    self->location = gweather_location_find_nearest_city(world, lat, lon);
    g_object_unref(world);
    self->weather_info = gweather_info_new(self->location);
    gweather_info_set_enabled_providers(self->weather_info, GWEATHER_PROVIDER_METAR);
    gweather_info_set_contact_info(self->weather_info, "daniel.guryev@gmail.com");

    g_signal_connect(self->weather_info, "updated", G_CALLBACK(update_weather_labels), self);
    self->weather_updater_id = g_timeout_add_seconds(g_settings_get_uint(self->settings, "weather-update-rate"),
                                                     G_SOURCE_FUNC(gweather_info_update),
                                                     self->weather_info);
    g_signal_connect_swapped(self->settings, "changed", G_CALLBACK(gweather_info_update), self->weather_info);
    gweather_info_update(self->weather_info);
}

static void
planified_weather_widget_class_init(PlanifiedWeatherWidgetClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/planified/weather-widget.ui");
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedWeatherWidget, weather_icon);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedWeatherWidget, weather_summary);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedWeatherWidget, weather_temperature);
}


PlanifiedWeatherWidget *
planified_weather_widget_new() {
    return g_object_new(PLANIFIED_WEATHER_WIDGET_TYPE, NULL);
}
