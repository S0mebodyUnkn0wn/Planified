common_sources = [
    'common.c',
    'task.c',
    'tag.c',
    'date-chooser-widget.c',
    'entry.c',
    'confirm-dialog.c',
]

lib_common = static_library(
    'planified-common',
    common_sources,
    dependencies : [gtk4_dep, sqlite_dep],
    include_directories : headers_dir
)