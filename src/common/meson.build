subdir('types')
subdir('widgets')

common_source = files(
    [

    ]
) + [
    common_types_source,
    common_widgets_source,
]

planified_common = library(
    'planified_common',
    common_source,
    dependencies : [gtk4_dep, gweather_dep],
    include_directories : headers_dir,
    link_args : '-Wl,--export-dynamic',

)