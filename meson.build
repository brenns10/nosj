project(
  'nosj', 'c',
  version : '0.0.1',
)

sources = [
  'src/json.c',
  'src/string.c',
  'src/util.c',
]

inc = include_directories('inc')

libnosj = library(
  'nosj',
  sources,
  include_directories : inc,
  install : true,
)

libnosj_dep = declare_dependency(
  include_directories : inc,
  link_with : libnosj,
)

pkg = import('pkgconfig')
pkg.generate(
  libraries: libnosj,
  subdirs: 'inc',
  version: meson.project_version(),
  name: 'libnosj',
  filebase: 'nosj',
  description: 'JSON parser',
)

# For each public header in "include/":
install_headers('inc/nosj.h')
