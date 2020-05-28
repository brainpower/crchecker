# crchecker

A small tool to check files with crc32 hashes in their filenames for correctness.

## Build Instructions

The tool depends on either the aws-checksums library: [awslabs/aws-checksums](https://github.com/awslabs/aws-checksums)
or alternatively good old zlib.

Be aware that aws-checksums will be a lot faster if you have a modern CPU with SSE4.1 or SSE4.2,
for older CPUs zlib may be faster, I have not tested it yet.

On my Ryzen 9 3900X aws-checksums is about twice as fast, see [doc/benchmark.txt](doc/benchmark.txt) for details.

### Build from git

    git clone https://github.com/brainpower/crchecker.git --recurse-submodules
    meson -Dwith-checkarg=subproject build
    ninja -C build

### Build from git with tests

    git clone https://github.com/brainpower/crchecker.git --recurse-submodules
    meson -Dwith-checkarg=subproject -Dtests=enabled build
    ninja -C build test

### Build from release tarball

    wget https://github.com/brainpower/crchecker/releases/X.Y.tar.gz

Now either download a release of [checkarg](https://github.com/brainpower/checkarg) into 3rdparty/checkarg,
or build and install it system wide.

For the former, see the meson and ninja commands of the "Build from git"-instructions,
for the latter, just do:

    meson build
    ninja -C build

Optionally enable tests, if you want.

