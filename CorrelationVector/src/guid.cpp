//---------------------------------------------------------------------
// <copyright file="guid.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#include "correlation_vector/guid.h"

#include "utilities.h"
#include <cmath>

#ifdef GUID_BOOST
#include <algorithm> // for std::transform
#include <boost/uuid/uuid_generators.hpp>
#endif

namespace microsoft
{
namespace impl
{
constexpr std::array<unsigned char, 16> convert_to_array(const unsigned char* g)
{
    return std::array<unsigned char, 16>{g[0],
                                         g[1],
                                         g[2],
                                         g[3],
                                         g[4],
                                         g[5],
                                         g[6],
                                         g[7],
                                         g[8],
                                         g[9],
                                         g[10],
                                         g[11],
                                         g[12],
                                         g[13],
                                         g[14],
                                         g[15]};
}

#ifdef GUID_WINDOWS
std::array<unsigned char, 16> convert_to_array(const GUID& g)
{
    return impl::convert_to_array(reinterpret_cast<const unsigned char*>(&g));
}
#endif
#ifdef GUID_LIBUUID
std::array<unsigned char, 16> convert_to_array(const uuid_t& g)
{
    return impl::convert_to_array(reinterpret_cast<const unsigned char*>(g));
}
#endif

#ifdef GUID_BOOST
std::array<unsigned char, 16> convert_to_array(const boost::uuids::uuid& g)
{
    std::array<unsigned char, 16> guid_array;
    std::transform(g.begin(), g.end(), guid_array.begin(), [](uint8_t value) {
        return static_cast<unsigned char>(value);
    });
    return guid_array;
}
#endif

} // namespace impl
guid guid::create()
{
#ifdef GUID_WINDOWS
    GUID g;
    HRESULT hrCreateGuid{CoCreateGuid(&g)};
    if (FAILED(hrCreateGuid))
    {
    }
#elif defined(GUID_LIBUUID)
    uuid_t g;
    uuid_generate(g);
#elif defined(GUID_BOOST)
    boost::uuids::uuid g{boost::uuids::random_generator()()};
#endif

    return guid{impl::convert_to_array(g)};
}

std::string guid::to_string() const
{
    return utilities::to_hex_str(m_bytes.data(), 0, 4) + '-' +
           utilities::to_hex_str(m_bytes.data(), 4, 2) + '-' +
           utilities::to_hex_str(m_bytes.data(), 6, 2) + '-' +
           utilities::to_hex_str(m_bytes.data(), 8, 2) + '-' +
           utilities::to_hex_str(m_bytes.data(), 10, 6);
}

static constexpr const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string guid::to_base64_string(int len) const
{
    int outputLength = static_cast<int>(ceil(len * 8 / 6.0));
    std::string s(outputLength, ' ');

    std::array<unsigned char, 3> buffer{0};

    for (int i = 0, j = 0; i < len; i++)
    {
        buffer[i % 3] = m_bytes[i];
        if ((i + 1) % 3 == 0)
        {
            s[j] = base64_table[(buffer[0] & 0xFC) >> 2];
            s[j + 1] = base64_table[((buffer[0] & 0x03) << 4) +
                                    ((buffer[1] & 0xF0) >> 4)];
            s[j + 2] = base64_table[((buffer[1] & 0x0F) << 2) +
                                    ((buffer[2] & 0xC0) >> 6)];
            s[j + 3] = base64_table[buffer[2] & 0x3F];
            j += 4;
        }
    }

    int remainingBytes = len % 3;
    if (remainingBytes > 0)
    {
        for (int i = remainingBytes; i < 3; ++i)
        {
            buffer[i] = '\0';
        }

        // Remaining Bytes can only be 1 or 2
        if (remainingBytes == 1)
        {
            s[outputLength - 2] = base64_table[(buffer[0] & 0xFC) >> 2];
            s[outputLength - 1] = base64_table[((buffer[0] & 0x03) << 4)];
        }
        else
        {
            s[outputLength - 3] = base64_table[(buffer[0] & 0xFC) >> 2];
            s[outputLength - 2] = base64_table[((buffer[0] & 0x03) << 4) +
                                               ((buffer[1] & 0xF0) >> 4)];
            s[outputLength - 1] = base64_table[((buffer[1] & 0x0F) << 2)];
        }
    }

    return s;
}
} // namespace microsoft