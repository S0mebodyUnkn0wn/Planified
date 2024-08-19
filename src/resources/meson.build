settings_schemas = ['com.github.somebodyunknown.planified.gschema.xml']
schemas_dir = get_option('prefix') / get_option('datadir') / 'glib-2.0' / 'schemas'
install_data(settings_schemas, install_dir: schemas_dir)

gnome.compile_schemas(
    depend_files : files(settings_schemas)
)

meson.add_install_script('glib-compile-schemas', schemas_dir)