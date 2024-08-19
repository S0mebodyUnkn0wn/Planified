app_source = [
    'app.c',
    'timetable-view.c',
    'app-window.c',
    'newtask-dialog.c',
    'task-widget.c',
    'itinerary-widget.c',
    'home-view.c',
    'itinerary-widget-entry.c',
    'weather-widget.c',
    'agenda-widget.c',
    'upcoming-list.c',
    'upcoming-list-item.c',
    'task-container.c',
    'tag-container.c',
    'preferences-dialog.c',
    'planning-view.c',
    'task-list.c',
    planified_resources,
]



planified = executable(
    'planified',
    app_source,
    install : true,
    dependencies : [gtk4_dep, sqlite_dep, gweather_dep, libadw_dep],
    link_with : [lib_common, sqlite_manager],
    include_directories : headers_dir,
    link_args : '-Wl,--export-dynamic',
)