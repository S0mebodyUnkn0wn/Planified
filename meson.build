project('planified', 'c',
        version : '1.0.0',
        default_options : ['warning_level=3', 'c_std=c2x'])

gtk4_dep = dependency('gtk4')
libadw_dep = dependency('libadwaita-1')
sqlite_dep = dependency('sqlite3')
gweather_dep = dependency('gweather4')

subdir('src')

subdir('tests')