app_source = [
    'planified-app.c'
]



planified = executable(
    'planified',
    app_source,
    install : true,
    dependencies : [gtk4_dep, sqlite_dep, gweather_dep],
#    link_with : [lib_common, sqlite_manager],
    include_directories : headers_dir,
    link_args : '-Wl,--export-dynamic',
)
