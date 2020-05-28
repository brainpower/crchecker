project('crchecker', 'cpp', version: '1.0',
  default_options: ['cpp_std=c++17'])

# {{{ files
sources = files(
  'src/util.cpp',
  'src/main.cpp'
)

priv_headers = files(
  'src/util.hpp',
  'src/constants.hpp'
)

# }}}
#
# {{{ dependencies

zlib = dependency('zlib')

if get_option('with-checkarg') == 'system'
  checkarg = dependency('checkarg++')
else
  checkarg_project = subproject('checkarg')
  checkarg = checkarg_project.get_variable('checkargpp_dep')
endif


lib_deps = zlib
main_deps = [zlib, checkarg]
# }}}


# {{{ executable

executable(
  'crchecker',
  sources, priv_headers,
  dependencies: main_deps,
  install: true
)
# }}}

# {{{ tests
if get_option('tests').enabled()
  subdir('tests')
endif
# }}}
