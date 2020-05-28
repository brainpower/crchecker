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

#include "util.hpp"
#include "constants.hpp"

#include <checkarg++.hpp>

#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

using std::cout;
using std::cerr;

char SEP = '\n';

std::string
append_crc_to_filename(const std::string &file) {
  auto dotpos = file.begin() + file.rfind('.');
  std::string extension(dotpos, file.end());
  std::string basename(file.begin(), dotpos);

  if( access(file.c_str(), R_OK | F_OK) != -1){
    std::ostringstream ss(basename, std::ios_base::ate);
    uLong crc = do_crc32(file);
    ss << "[" << std::hex << std::uppercase << crc << "]" << extension;
    return ss.str();
  } else {
    cerr << "ERROR: File not readable: " << file << "\n";
    return std::string{};
  }
}

int do_append(const std::vector<std::string> &files){
  cerr << "WARNING: This option is not completely implemented, thus a noop.\n";
  int ret = 0;
  for( auto &file : files ) {
    auto newname = append_crc_to_filename(file);
    if ( !newname.empty() ) {
      cout << file << " -> " << newname << "\n";
      // TODO: figure out renaming
    }
  }
  return ret;
}

int do_generate(const std::vector<std::string> &files) {
  int ret = 0;
  for( auto &file : files ) {
    auto newname = append_crc_to_filename(file);
    if (!newname.empty()) {
      cout << newname << SEP;
    } else {
      ++ret;
    }
  }
  return ret;
}

int do_normal(const std::vector<std::string> &files){
  int pad = (int)std::max(0, cols() - 6 - CRCLEN - CRCLEN - OKLEN - 3);

  cout << "<file>" << std::string(pad, ' ') << "<src CRC> <real CRC>\n";
  cout << std::string(cols(), '-') << '\n';

  for(auto &file : files){
    if( access(file.c_str(), R_OK | F_OK) != -1){
      pad = (int)(cols() - file.size() - CRCLEN*2 - OKLEN - 2);

      cout << file;
      cout << std::setw(pad) << std::setfill(' ') << ' ';

      uLong scrc = 0, rcrc;
      auto str_crc = get_crc_from_name(file);

      if(str_crc.empty()){
        cout << std::setw(CRCLEN) << std::setfill(' ') << ' ';
      } else {
        scrc = std::stoul( str_crc, 0, 16 );
        cout << std::hex << std::uppercase
             << std::setw(CRCLEN) << std::setfill('0')
             << scrc << ' ';
      }
      cout << std::flush;

      rcrc = do_crc32(file);
      cout << std::hex << std::uppercase
           << std::setw(CRCLEN) << std::setfill('0')
           << rcrc << ' ';

      cout << ((scrc == rcrc) ? OK : FAIL) << '\n';

    } else {
      cerr << "ERROR: File not readable: " << file << '\n';
      //printf("[DEBUG] File not readable!\n");
    }
  }
  return 0;
}


int main(int argc, char **argv ) {
  CheckArg ca(argc, argv, "crc_checker", "Check CRC values embedded in filenames.");

  ca.add('a', "append",   "append CRC hash to every given file (not implemented)");
  ca.add('g', "generate", "show filenames with generated CRC appended");
  ca.add('z', "zero",     "print stuff delimited with \\0 instead of \\n.");
  ca.add_autohelp();

  ca.parse();

  int ret = 0;
  if( ca.isset("zero") ) {
    SEP = '\0';
  }
  if( ca.isset("generate") ){
    ret = do_generate(ca.pos_args());
    // print: X errors occurred
    if (ret > 1) ret = 1;
  }
  else if ( ca.isset("append") ) {
    ret = do_append(ca.pos_args());
    // print: X errors occurred
    if (ret > 1) ret = 1;
  }
  else {
    ret = do_normal(ca.pos_args());
  }

  return ret;
}
