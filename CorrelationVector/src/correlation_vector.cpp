//---------------------------------------------------------------------
// <copyright file="correlation_vector.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#include "correlation_vector/correlation_vector.h"
#include "correlation_vector/guid.h"
#include "correlation_vector/spin_parameters.h"
#include "utilities.h"
#include <chrono>
#include <ctime>
#include <string>
#include <vector>
#include <limits> // std::numeric_limits

namespace telemetry
{
/* static */
std::string correlation_vector::_unique_value(
    correlation_vector_version version)
{
    switch (version)
    {
        case correlation_vector_version::v1:
            return guid::create().to_base64_string(12);
            break;
        case correlation_vector_version::v2:
            return guid::create().to_base64_string();
            break;
        default:
            throw std::invalid_argument(
                "Unsupported correlation vector version " +
                std::to_string(static_cast<int>(version)));
    }
}

/* static */
correlation_vector_version correlation_vector::_infer_version(
    const std::string& correlationVector)
{
    size_t index =
        correlationVector.empty() ? -1 : correlationVector.find_first_of('.');
    switch (index)
    {
        case BASE_LENGTH_V2: return correlation_vector_version::v2; break;
        // fallback to v1 if not v2 or invalid.
        case BASE_LENGTH_V1:
        default: return correlation_vector_version::v1; break;
    }
}

/* static */
void correlation_vector::_validate(const std::string& correlationVector,
                                   correlation_vector_version version)
{
    size_t maxVectorLength;
    size_t baseLength;

    switch (version)
    {
        case correlation_vector_version::v1:
            maxVectorLength = MAX_VECTOR_LENGTH_V1;
            baseLength = BASE_LENGTH_V1;
            break;
        case correlation_vector_version::v2:
            maxVectorLength = MAX_VECTOR_LENGTH_V2;
            baseLength = BASE_LENGTH_V2;
            break;
        default:
            throw std::invalid_argument(
                "Unsupported correlation vector version.");
    }

    if (correlationVector.empty())
    {
        throw std::invalid_argument("Correlation vector cannot be empty.");
    }

    if (utilities::contains_whitespace(correlationVector))
    {
        throw std::invalid_argument("Correlation vector cannot contain "
                                    "whitespace. Correlation vector: " +
                                    correlationVector);
    }

    // Check if last char is terminator and remove for further validation.
    std::string notTerminatedCV{
        correlationVector.find_first_of(TERMINATOR) ==
                correlationVector.size() - 1
            ? correlationVector.substr(0, correlationVector.size() - 1)
            : correlationVector};

    if (notTerminatedCV.length() > maxVectorLength)
    {
        throw std::invalid_argument("Correlation vector: " + correlationVector +
                                    ", was bigger than the allowed range of " +
                                    std::to_string(maxVectorLength) + ".");
    }

    std::vector<std::string> parts{utilities::split_str(notTerminatedCV, '.')};
    if (parts.size() < 2 || parts[0].length() != baseLength)
    {
        throw std::invalid_argument(
            "Invalid correlation vector: " + correlationVector +
            ". Invalid base value " + parts[0]);
    }

    // Validate each extension
    for (size_t i = 1; i < parts.size(); ++i)
    {
        std::size_t lastChar;
        int result = -1;
        try
        {
            result = std::stoi(parts[i], &lastChar, 10);
        }
        catch (const std::out_of_range&)
        {
        }
        catch (const std::invalid_argument&)
        {
        }

        if (lastChar != parts[i].length() || result < 0)
        {
            throw std::invalid_argument(
                "Invalid correlation vector " + correlationVector +
                ". Invalid extension value " + parts[i]);
        }
    }
}

bool correlation_vector::_is_oversized(const std::string& baseVector,
                                       int extension,
                                       correlation_vector_version version)
{
    if (baseVector.empty())
    {
        return false;
    }

    size_t cvLen = baseVector.length() + 1 + _int_length(extension);
    return (version == correlation_vector_version::v1 &&
            cvLen > MAX_VECTOR_LENGTH_V1) ||
           (version == correlation_vector_version::v2 &&
            cvLen > MAX_VECTOR_LENGTH_V2);
}

correlation_vector correlation_vector::extend(
    const std::string& correlationVector)
{
    if (_is_immutable(correlationVector))
    {
        return parse(correlationVector);
    }

    correlation_vector_version version{_infer_version(correlationVector)};
    _validate(correlationVector, version);

    if (_is_oversized(correlationVector, 0, version))
    {
        return parse(correlationVector + TERMINATOR);
    }

    return {correlationVector, version};
}

/* static */
correlation_vector correlation_vector::spin(
    const std::string& correlationVector, const spin_parameters& parameters)
{
    if (_is_immutable(correlationVector))
    {
        return parse(correlationVector);
    }

    const correlation_vector_version version{_infer_version(correlationVector)};
    _validate(correlationVector, version);

    const int entropyBytes{static_cast<int>(parameters.entropy())};
    std::vector<unsigned char> entropy(entropyBytes);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    for (int i = 0; i < entropyBytes; ++i)
    {
        entropy[i] = static_cast<unsigned char>(rand());
    }

    long long ticks{
        std::chrono::system_clock::now().time_since_epoch().count()};
    long long value{ticks >> static_cast<int>(parameters.interval())};
    for (int i = 0; i < entropyBytes; ++i)
    {
        value = (value << 8) | static_cast<uint64_t>(entropy[i]);
    }

    int totalBits{parameters.total_bits()};
    value &= (totalBits == 64 ? 0 : (1LL << totalBits)) - 1;

    std::string s{std::to_string(static_cast<unsigned int>(value))};
    if (totalBits > 32)
    {
        s = std::to_string(static_cast<unsigned int>(value >> 32)) + '.' + s;
    }

    std::string baseVector{correlationVector + '.' + s};
    if (_is_oversized(baseVector, version))
    {
        return parse(correlationVector + TERMINATOR);
    }

    return correlation_vector(baseVector, version);
}

correlation_vector correlation_vector::parse(
    const std::string& correlationVector)
{
    _validate(correlationVector, _infer_version(correlationVector));
    size_t p = correlationVector.find_last_of('.');
    bool isImmutable = _is_immutable(correlationVector);
    if (p > 0)
    {
        std::string lastStage = correlationVector.substr(p + 1);
        int extension =
            isImmutable ? std::stoi(lastStage.substr(0, lastStage.length() - 1))
                        : std::stoi(lastStage);

        int extLen = _int_length(extension);
        bool isValidExt = isImmutable ? (extLen == lastStage.length() - 1)
                                      : (extLen == lastStage.length());

        if (isValidExt && extension >= 0)
        {
            return {correlationVector.substr(0, p),
                    extension,
                    _infer_version(correlationVector),
                    isImmutable};
        }
    }

    return {};
}

std::string correlation_vector::increment()
{
    if (m_is_immutable)
    {
        return value();
    }

    int snapshot = 0;
    int next = 0;

    do
    {
        snapshot = m_extension.load();
#pragma push_macro("max")
#undef max
        if (snapshot == std::numeric_limits<int>::max())
        {
            return value();
        }
#pragma pop_macro("max")

        next = snapshot + 1;
        if (_is_oversized(m_base_vector, next, m_version))
        {
            m_is_immutable = true;
            return value();
        }
    } while (!m_extension.compare_exchange_weak(snapshot, next));

    return m_base_vector + '.' + std::to_string(next);
}
} // namespace telemetry
