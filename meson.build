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

# {{{ init config
config = configuration_data()
conf_inc = include_directories('.')
# }}}

# {{{ dependencies

aws_crc = dependency(
  'aws-checksums',
  modules: ['AWS::aws-checksums'],
  cmake_args: ['-DBUILD_SHARED_LIBS=ON'],
  required: false
) # it will not find shared libs on my system otherwisw

if not aws_crc.found() or get_option('force-zlib')
  message('aws-checksums not found, using good old zlib for crc32 calculations.')
  zlib = dependency('zlib')
  lib_deps = [zlib, ]
  config.set10('HAS_AWS_CRC', false)
else
  lib_deps = [aws_crc, ]
  config.set10('HAS_AWS_CRC', true)
endif


if get_option('with-checkarg') == 'system'
  checkarg = dependency('checkarg++')
else
  checkarg_project = subproject('checkarg')
  checkarg = checkarg_project.get_variable('checkargpp_dep')
endif


main_deps = lib_deps + [checkarg, ]
# }}}


# {{{ executable

configure_file(
  output: 'config.hpp',
  configuration: config
)

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
