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

#include "constants.hpp"
#include "../config.hpp"
#include "util.hpp"

#include <memory>
#include <sys/ioctl.h>

#if HAS_AWS_CRC
#include <aws/checksums/crc.h>
#else
#include <zlib.h>
#endif

#include <cstring>

#define unlikely(x)     __builtin_expect((x),0)

using FilePtr = std::unique_ptr<FILE, int (*)(FILE*)>;

int cols() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
}


bool
file_readable(const std::string &file) {
  return access(file.c_str(), R_OK | F_OK) != -1;
}


#if HAS_AWS_CRC
uint32_t do_crc32_aws(FilePtr fin) {
  uint8_t buff[BUFFLEN];
  size_t  nread;
  uint32_t crc = aws_checksums_crc32(nullptr, 0, 0L);

  while( 1 ) {
    nread = fread(buff, 1, BUFFLEN, fin.get());
    if ( nread < BUFFLEN ) { // mark unlikely?
      // something is unusual
      if ( feof(fin.get()) ) {
        // we reached the end! one last round of CRCing:
        return aws_checksums_crc32(buff, nread, crc);
      }
      else if ( ferror(fin.get()) ) {
        perror("[ERROR] An error occurred while reading the file: ");
        return 0; // a likely invalid CRC??
      } else {
        perror("[ERROR] An unknown error occurred!");
        return 0; // a likely invalid CRC??
      }
    } else {
      // business as usual
      crc = aws_checksums_crc32(buff, nread, crc);
    }
  }
}
#else

uint32_t
do_crc32_zlib(FilePtr fin){
  Bytef buff[BUFFLEN];
  size_t nread = 0;
  uint32_t crc = crc32(0L, Z_NULL, 0);

  while( 1 ){
    nread = fread(buff, 1, BUFFLEN, fin.get());
    //printf("[DEBUG] read %lu bytes.\n", nread);

    if( nread < BUFFLEN){ // something is up!

      if( ferror(fin.get()) ) { // uh-uh, an error!
        perror("[ERROR] An error occurred while reading the file: ");
        return 0;
      }
      else if( feof(fin.get()) ) {
        // puh! only a EOF
        // do a last crc call and off we go!
        return crc32( crc, buff, nread );
      }
      else {
        perror("[ERROR] An unknown error occurred!");
        return 0;
      }

    }

    // normal case, buffer was fully filled and no EOF or error occurred
    crc = crc32( crc, buff, nread );

  }
}
#endif

uint32_t
do_crc32(const std::string &filename){
  FilePtr fin(
    fopen(filename.c_str(), "rb"),
    &fclose
    //[](FILE *fp)->int{ if(fp) return ::fclose(fp); return EOF;}
  );

  if( fin != nullptr ) {
#if HAS_AWS_CRC
    return do_crc32_aws(std::move(fin));
#else
    return do_crc32_zlib(std::move(fin));
#endif
  }
  else {
    perror("[ERROR] Error while reading input file: ");
    return 0;
  }
}


std::string get_crc_from_name(const std::string &filepath) {
  bool was_paren = false;
  std::string crc;

  std::string filename( basename(filepath.c_str()) );

  auto it = filename.begin();
  auto end = filename.end();

  start_search:
  crc = ""; was_paren = false;
  while( it != end && ( *it != '[' && *it != '(' ) ) ++it;
  //printf( "searching for [, current char: '%c'\n", *it );
  if( it == end ) return "";
  if( *it == '(' ) was_paren = true;

  // we found a [
  ++it;
  while( isxdigit( *it ) && crc.size() < CRCLEN ) {
    crc += *it;
    ++it;
  }

  //printf("searching for CRC, current char: '%c', current crc: '%s'\n", *it, crc.c_str());
  if( crc.size() != CRCLEN ){
    // not a CRC
    goto start_search;
  }
  if( !was_paren && *it != ']' ){
    // next char is no ] -> probably longer "tag", so no CRC
    goto start_search;
  }
  if( was_paren && *it != ')' ){
    goto start_search;
  }

  // at this point we've got a pair of [] with 8 hex digits in between
  // so it's most likely a crc!
  return crc;

}
