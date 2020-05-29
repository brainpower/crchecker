// Copyright (c) 2016-2020 brainpower <brainpower at mailbox dot org>
//
// SPDX-License-Identifier: MIT

#include "../src/util.hpp"

#include <string>
#include <iostream>

using namespace std;

int main(int argc, char **argv){
  string filename = "test.bin";

  // as printed by crc32 part of perl's archive zip
  uint32_t expected_crc = 0xFE8EDB22;

  auto result = do_crc32(filename);

  if ( expected_crc == result ) {
    return 0;
  } else {
    cerr << "FAIL: calculated CRC32 does not match expected value!\n";
    cerr << "  Expected: " << hex << uppercase << expected_crc << '\n';
    cerr << "       Got: " << hex << uppercase << result << '\n';
    return 1;
  }
}
