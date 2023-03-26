project(
  'nosj', 'c',
  version : '2.1.0',
)

fs = import('fs')

sources = [
  'src/json.c',
  'src/string.c',
  'src/util.c',
  'src/format.c',
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

tests = [
  'test/compare_strings.c',
  'test/load_strings.c',
  'test/parse_arrays.c',
  'test/parse_atomics.c',
  'test/parse_numbers.c',
  'test/parse_objects.c',
  'test/parse_strings.c',
  'test/lookup.c',
  'test/easy.c',
  'test/format.c',
]
unity_dep = dependency(
    'Unity',
    fallback: ['Unity', 'unity_dep'],
)
foreach t: tests
  testname = fs.name(t)
  exe = executable('test_' + testname, t, dependencies : [libnosj_dep, unity_dep])
  test('TEST_' + testname, exe)
endforeach
