//---------------------------------------------------------------------
// <copyright file="Utilities.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#pragma once
#include <sstream>
#include <string>
#include <vector>

namespace telemetry
{
namespace utilities
{
inline std::vector<std::string> split_str(std::string string, char delimeter)
{
    std::vector<std::string> tokens;

    std::istringstream iss(string);
    std::string token;
    while (std::getline(iss, token, delimeter))
    {
        if (!token.empty()) tokens.push_back(token);
    }

    return tokens;
}

constexpr const unsigned char hex_str_map[17] = "0123456789ABCDEF";

inline std::string to_hex_str(const unsigned char* data, int offset, int len)
{
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i)
    {
        s[2 * i] = hex_str_map[(data[i + offset] & 0xF0) >> 4];
        s[2 * i + 1] = hex_str_map[data[i + offset] & 0x0F];
    }

    return s;
}

inline bool contains_whitespace(const std::string& s)
{
    return s.find_first_of("\t\n ") != std::string::npos;
}
} // namespace utilities
} // namespace telemetry