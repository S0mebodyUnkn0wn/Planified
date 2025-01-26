subdir('widgets')
subdir('views')

app_source = files(
    [
        'planified-app.c',
        'app-window.c',
    ]
) + planified_resources + app_widgets_source + app_views_source

planified_app = executable(
    'planified',
    app_source,
    install : false,
    dependencies : [gtk4_dep, sqlite_dep, gweather_dep],
    link_with : [planified_common, planified_core], #, planified_app_views, planified_app_widgets],
    include_directories : headers_dir,
    link_args : '-Wl,--export-dynamic',
)
