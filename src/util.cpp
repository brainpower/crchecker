// Copyright (c) 2016-2020 brainpower <brainpower at mailbox dot org>
//
// SPDX-License-Identifier: MIT

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
  uint32_t crc = aws_checksums_crc32c(nullptr, 0, 0L);

  while( 1 ) {
    nread = fread(buff, 1, BUFFLEN, fin.get());

    if ( nread < BUFFLEN ) {
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

    if( nread < BUFFLEN ){ // something is up!

      if( feof(fin.get()) ) {
        // puh! only a EOF
        // do a last crc call and off we go!
        return crc32( crc, buff, nread );
      }
      else if( ferror(fin.get()) ) { // uh-uh, an error!
        perror("[ERROR] An error occurred while reading the file: ");
        return 0;
      } else {
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
