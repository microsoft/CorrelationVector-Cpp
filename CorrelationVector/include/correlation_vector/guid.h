//---------------------------------------------------------------------
// <copyright file="guid.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#pragma once

#include <array>
#include <string>

namespace telemetry
{
#if defined(GUID_WINDOWS)
#include "Objbase.h"
using guid_t = GUID;
#elif defined(GUID_LIBUUID)
#include <uuid/uuid.h>
using guid_t = uuid_t;
#else
#error "No guid type avaialable."
#endif

class guid
{
private:
    guid() = default;

    guid(const std::array<unsigned char, 16>& bytes) : m_bytes{bytes} {}
    guid(std::array<unsigned char, 16>&& bytes) : m_bytes{std::move(bytes)} {}

    void _clear()
    {
        std::fill(
            m_bytes.begin(), m_bytes.end(), static_cast<unsigned char>(0));
    }

    std::array<unsigned char, 16> m_bytes{};

public:
    static guid create();
    static guid create(const guid_t&);
    static guid create(const std::array<unsigned char, 16>& bytes);
    static guid create(std::array<unsigned char, 16>&& bytes);
    static guid empty() { return {}; }

    guid(const guid& other) = default;
    guid(guid&& other) = default;
    guid& operator=(const guid& other) = default;
    guid& operator=(guid&& other) = default;

    std::string to_string() const;
    std::string to_base64_string(int len = 16) const;
};
} // telemetry
