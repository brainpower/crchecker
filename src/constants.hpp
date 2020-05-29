// Copyright (c) 2016-2020 brainpower <brainpower at mailbox dot org>
//
// SPDX-License-Identifier: MIT


#ifndef CRC_CHECKER_CONSTANTS_HPP
#define CRC_CHECKER_CONSTANTS_HPP

constexpr int BUFFLEN = 4096 * 1024;
constexpr char CRCLEN = 8; // a CRC32 has 8 chars when displayed as HEX
constexpr const char *OK = "[ ok ]";
constexpr const char *FAIL = "[fail]";
constexpr char OKLEN = 6;

#endif //CRC_CHECKER_CONSTANTS_HPP
