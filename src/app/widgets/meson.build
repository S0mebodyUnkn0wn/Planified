subdir('containers')
subdir('dialogs')
subdir('lists')

app_widgets_source = files(
    [
        'weather-widget.c',
    ]
) + [
    app_lists_source,
    app_containers_source,
    app_dialogs_source
]


planified_app_widgets = library(
    'planified_app_widgets',
    app_widgets_source,
    dependencies : [gtk4_dep, gweather_dep],
    include_directories : headers_dir,
    link_args : '-Wl,--export-dynamic',

)