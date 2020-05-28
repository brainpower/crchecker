/*
 * Copyright (c) 2020 brainpower <brainpower at mailbox dot org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

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
