/*
 * Copyright (c) 2016 brainpower <brainpower at mailbox dot org>
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

#include <vector>
#include <string>



int main(int argc, char **argv){
  int status = 0;
  int testno = 0;
  std::string crc = "48782E36";

  std::vector<std::string> names = {
      "Detective_Conan_-_OVA12_[DCTP][H264][48782E36].mp4", // ok
      "Detective_Conan_-_OVA12_[DCTP][1264h987][48782E36].mp4", // ok, last segment must be crc!
      "/path/should/get[12345678]/skipped/Detective_Conan_-_OVA12_[DCTP][H264][48782E36].mp4", // ok
      "/path/should/get[12345678]/skipped/Detective_Conan_-_OVA12_[DCTP][H264](48782E36).mp4", // ok
      "/path/should/get[12345678]/skipped/Detective_Conan_-_OVA12_(DCTP)(H264)(48782E36).mp4" // ok
  };

  std::vector<std::string> bad_names = {
      "Detective_Conan_-_OVA12_[DCTP][H264].mp4", // no crc!
  };

  for( auto &name : names ){
    printf("Test %d: ", testno);
    fflush(stdout);

    auto ret = get_crc_from_name(name);
    if( ret != crc ) {
      fprintf( stderr, "FAIL: expected '%s', got '%s'; filename was '%s'\n", crc.c_str(), ret.c_str(), name.c_str());
      ++status;
    } else {
      printf("OK\n");
    }
    ++testno;
  }

  for( auto &name :  bad_names ){
    printf("Test %d: ", testno);

    auto ret = get_crc_from_name(name);
    if( !ret.empty() ){
      fprintf( stderr, "FAIL: expected '', got '%s'; filename was '%s'\n", ret.c_str(), name.c_str());
      ++status;
    } else {
      printf("OK\n");
    }
    ++testno;
  }

  return status;
}
