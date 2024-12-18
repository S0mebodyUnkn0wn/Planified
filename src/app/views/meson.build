app_views_source = files(
    [
        'home-view.c',
        'planning-view.c',
        'timetable-view.c',
    ]
)


planified_app_views = library(
    'planified_app_views',
    app_views_source,
    dependencies : [gtk4_dep, gweather_dep],
    include_directories : headers_dir,
    link_args : '-Wl,--export-dynamic',

)