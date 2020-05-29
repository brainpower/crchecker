// Copyright (c) 2016-2020 brainpower <brainpower at mailbox dot org>
//
// SPDX-License-Identifier: MIT

#ifndef CRC_CHECKER_UTIL_HPP
#define CRC_CHECKER_UTIL_HPP

#include <zconf.h>

#include <string>

int         cols();

bool        file_readable(const std::string &file);

uint32_t    do_crc32(const std::string &filename);

std::string get_crc_from_name(const std::string &filename);

#endif //CRC_CHECKER_UTIL_HPP
