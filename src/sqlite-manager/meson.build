backend_sources = [
    'dbio.c',
    'migrations.c',

]

sqlite_manager = static_library(
    'sqlite-manager',
    backend_sources,
    dependencies : [gtk4_dep, sqlite_dep],
    include_directories : headers_dir
)