project('planified', 'c',
        version : '0.0.1',
        default_options : ['warning_level=3', 'c_std=c2x'])

gtk4_dep = dependency('gtk4')
sqlite_dep = dependency('sqlite3')
gweather_dep = dependency('gweather4')

headers_dir = include_directories('inc')

subdir('src')
