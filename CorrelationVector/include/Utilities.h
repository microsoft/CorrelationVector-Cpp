//---------------------------------------------------------------------
// <copyright file="Utilities.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#pragma once
#include <string>
#include <vector>
#include <sstream>

static inline std::vector<std::string> split_str(std::string string, const char delimeter)
{
	std::vector<std::string> tokens;

	std::istringstream iss(string);
	std::string token;
	while (std::getline(iss, token, delimeter)) {
		if (!token.empty())
			tokens.push_back(token);
	}

	return tokens;
}

static const unsigned char hex_str_map[17] = "0123456789ABCDEF";

static inline std::string to_hex_str(unsigned char* data, int offset, int len)
{
	std::string s(len * 2, ' ');
	for (int i = 0; i < len; ++i) {
		s[2 * i] = hex_str_map[(data[i + offset] & 0xF0) >> 4];
		s[2 * i + 1] = hex_str_map[data[i + offset] & 0x0F];
	}
	return s;
}