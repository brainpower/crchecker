
tests = [
  'filename_parser',
  'crc_calculation',
]

foreach t : tests
  test_filename = 'test_' + t
  test_exe = executable(
    test_filename,
    files(test_filename + '.cpp', '../src/util.cpp'),
    dependencies: lib_deps,
    install: false
  )

  # set workdir, otherwise tests wont find 'test.bin' file
  test(t, test_exe, workdir: meson.current_source_dir())
endforeach
