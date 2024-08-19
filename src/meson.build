headers_dir = include_directories('include')
ui_layout_dir = include_directories('resources')
gnome = import('gnome')

planified_resources = gnome.compile_resources(
    'planified-res', 'resources/planified.gresource.xml',
    source_dir: 'resources',
)


subdir('resources')
subdir('planified-common') # Build common lib, will be put into lib_common var
subdir('sqlite-manager')

subdir('planified-app')

