//---------------------------------------------------------------------
// <copyright file="correlation_vector.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#pragma once
#include "correlation_vector/guid.h"
#include "correlation_vector/spin_parameters.h"
#include <cmath>
#include <atomic>
#include <string>

namespace telemetry
{
enum class correlation_vector_version
{
    v1,
    v2
};

class correlation_vector
{
private:
    static constexpr const size_t MAX_VECTOR_LENGTH_V1 = 63;
    static constexpr const size_t MAX_VECTOR_LENGTH_V2 = 127;
    static constexpr const size_t BASE_LENGTH_V1 = 16;
    static constexpr const size_t BASE_LENGTH_V2 = 22;

    static std::string _base_from_guid(const guid& guid)
    {
        return guid.to_base64_string().substr(
            0, correlation_vector::BASE_LENGTH_V2);
    }

    static std::string _unique_value(correlation_vector_version version);

    static correlation_vector_version _infer_version(
        const std::string& correlationVector);

    static void _validate(const std::string& correlationVector,
                          correlation_vector_version version);

    static int _int_length(int i)
    {
        return (i == 0) ? 1 : static_cast<int>(std::log10(i)) + 1;
    }

    static bool _is_immutable(const std::string& correlationVector)
    {
        return !correlationVector.empty() &&
               correlationVector.at(correlationVector.length() - 1) ==
                   TERMINATOR;
    }

    static bool _is_oversized(const std::string& baseVector,
                              int extension,
                              correlation_vector_version version);

    static bool _is_oversized(const std::string& baseVector,
                              correlation_vector_version version)
    {
        return _is_oversized(baseVector, 0, version);
    }

    correlation_vector(const std::string& baseVector,
                       int extension,
                       correlation_vector_version version,
                       bool isImmutable)
        : m_base_vector{baseVector}
        , m_extension{extension}
        , m_version{version}
        , m_is_immutable{isImmutable}
    {
    }

    correlation_vector(const std::string& baseVector,
                       correlation_vector_version version)
        : m_base_vector{baseVector}, m_version{version}
    {
    }

    correlation_vector_version m_version{correlation_vector_version::v1};
    std::string m_base_vector;
    std::atomic<int> m_extension{0};
    bool m_is_immutable{false};

public:
    /**
    This is the delimiter to indicate that a CV is terminated.
    */
    static constexpr const char TERMINATOR = '!';

    /**
    Initializes a new instance of the Correlation Vector. This should only
    be called when no existing Correlation Vector was found.
    */
    correlation_vector() : m_base_vector{_unique_value(m_version)} {}

    /**
    Initializes a new instance of the Correlation Vector of the V2
    implementation using the given Guid as the vector base. This should only be
    called when no Correlation Vector was found.
    */
    correlation_vector(const guid& guid)
        : m_base_vector{_base_from_guid(guid)}
        , m_version{correlation_vector_version::v2}
    {
    }

    /**
    Initializes a new instance of the Correlation Vector of the given
    implementation version. This should only be called when no Correlation
    Vector was found in the message header.
    */
    correlation_vector(correlation_vector_version version)
        : m_base_vector{_unique_value(version)}, m_version{version}
    {
    }

    // NOTE: we need to implement the special member functions due to
    // std::atomic member.

    /**
    Initializes a new instance of the Correlation Vector using the given
    Correlation Vector.
    */
    correlation_vector(const correlation_vector& other)
        : m_base_vector{other.m_base_vector}
        , m_extension{other.m_extension.load()}
        , m_version{other.m_version}
        , m_is_immutable{other.m_is_immutable}
    {
    }

    correlation_vector(correlation_vector&& other)
        : m_base_vector{std::move(other.m_base_vector)}
        , m_extension{other.m_extension.load()}
        , m_version{other.m_version}
        , m_is_immutable{other.m_is_immutable}
    {
    }

    correlation_vector& operator=(const correlation_vector& other)
    {
        m_base_vector = other.m_base_vector;
        m_extension.store(other.m_extension.load());
        m_version = other.m_version;
        m_is_immutable = other.m_is_immutable;
    }

    correlation_vector& operator=(correlation_vector&& other)
    {
        m_base_vector = std::move(other.m_base_vector);
        m_extension.store(other.m_extension.load());
        m_version = other.m_version;
        m_is_immutable = other.m_is_immutable;
    }

    /**
    Creates a new Correlation Vector by extending an existing value. This should
    be done at the entry point of an operation.
    @param The Correlation Vector taken from the message header
    @return A new Correlation Vector extended from the current vector
    */
    static correlation_vector extend(const std::string& correlationVector);

    /**
    Creates a new Correlation Vector by applying the Spin operator to an
    existing value. This should be done at the entry point of an operation.
    @param correlationVector The Correlation Vector taken from the message
    header
    @return A new Correlation Vector extended from the current vector
    */
    static correlation_vector spin(const std::string& correlationVector)
    {
        return spin(correlationVector, {});
    };

    /**
    Creates a new Correlation Vector by applying the spin operator to an
    existing value. This should be done at the entry point of an operation.
    @param correlationVector The existing Correlation Vector.
    @param parameters The parameters to use when applying the spin operator.
    @return A new Correlation Vector extended from the provided vector.
    */
    static correlation_vector spin(const std::string& correlationVector,
                                   const spin_parameters& parameters);

    /**
    Creates a new Correlation Vector by parsing its string representation
    @param correlationVector The Correlation Vector in its string representation
    @return A new Correlation Vector parsed from its string representation
    */
    static correlation_vector parse(const std::string& correlationVector);


    /**
    Gets the value of the Correlation Vector as a string
    @return The string representation of the Correlation Vector
    */
    std::string value() const
    {
        std::string value{m_base_vector + '.' + std::to_string(m_extension)};
        if (m_is_immutable)
        {
            value += TERMINATOR;
        }

        return value;
    }

    /**
    Increments the current extension by one. Do this before passing the value to
    an outbound message header.
    @return The new value as a string that you can add to the outbound message
    header
    */
    std::string increment();

    /**
    Gets the version of the Correlation Vector implementation.
    @return The version of the Correlation Vector implementation
    */
    correlation_vector_version version() const { return m_version; }

    /**
    Gets the value of the Correlation Vector as a string
    @return The string representation of the Correlation Vector
    */
    std::string to_string() const { return value(); }

    /**
    Determines whether two instances of the Correlation Vector are equal
    @param other The Correlation Vector you want to compare with the current
    Correlation Vector
    @result true if the specified Correlation Vector is equal to the current
    Correlation Vector, otherwise false.
    */
    bool operator==(const correlation_vector& other)
    {
        return m_base_vector == other.m_base_vector &&
               m_extension == other.m_extension;
    }

    /**
    Determines whether two instances of the Correlation Vector are not equal
    @param other The Correlation Vector you want to compare with the current
    Correlation Vector
    @result true if the specified Correlation Vector is not equal to the current
    Correlation Vector, otherwise false.
    */
    bool operator!=(const correlation_vector& other)
    {
        return m_base_vector != other.m_base_vector ||
               m_extension != other.m_extension;
    }
};
} // namespace telemetry