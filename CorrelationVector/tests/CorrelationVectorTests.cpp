//---------------------------------------------------------------------
// <copyright file="telemetry::CorrelationVectorTests.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "correlation_vector/correlation_vector.h"
#include "correlation_vector/guid.h"
#include "correlation_vector/spin_parameters.h"
#include "utilities.h"
#include <chrono>
#include <future>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

TEST_CASE("Increment_IsUniqueAcrossMultipleThreads")
{
    const int numberOfThreads = 1000;
    telemetry::correlation_vector cv;
    telemetry::correlation_vector cv2{telemetry::correlation_vector::extend(cv.value())};

    std::vector<std::future<std::string>> futures;
    for (int i = 0; i < numberOfThreads; ++i)
    {
        futures.push_back(std::async(&telemetry::correlation_vector::increment, &cv2));
    }

    std::unordered_set<std::string> set;
    for (std::future<std::string>& f : futures)
    {
        set.insert(f.get());
    }

    REQUIRE(set.size() == numberOfThreads);
    futures.clear();
    set.clear();
}

TEST_CASE("CreateExtendAndIncrement_Default")
{
    telemetry::correlation_vector cv;
    std::vector<std::string> splitVector{telemetry::utilities::split_str(cv.value(), '.')};

    REQUIRE(splitVector.size() == 2);
    REQUIRE(splitVector[0].length() == 16);
    REQUIRE(splitVector[1] == "0");

    std::string incrementedVector{cv.increment()};
    splitVector = telemetry::utilities::split_str(incrementedVector, '.');
    REQUIRE(splitVector.size() == 2);
    REQUIRE(splitVector[1] == "1");
}

TEST_CASE("CreateExtendAndIncrement_V1")
{
    telemetry::correlation_vector cv;
    std::vector<std::string> splitVector{telemetry::utilities::split_str(cv.value(), '.')};

    REQUIRE(splitVector.size() == 2);
    REQUIRE(splitVector[0].length() == 16);
    REQUIRE(splitVector[1] == "0");

    std::string incrementedVector{cv.increment()};
    splitVector = telemetry::utilities::split_str(incrementedVector, '.');
    REQUIRE(splitVector.size() == 2);
    REQUIRE(splitVector[1] == "1");
}

TEST_CASE("CreateExtendAndIncrement_V2")
{
    telemetry::correlation_vector cv{telemetry::correlation_vector_version::v2};
    REQUIRE(cv.version() == telemetry::correlation_vector_version::v2);

    std::vector<std::string> splitVector{telemetry::utilities::split_str(cv.value(), '.')};

    REQUIRE(splitVector.size() == 2);
    REQUIRE(splitVector[0].length() == 22);
    REQUIRE(splitVector[1] == "0");

    std::string incrementedVector{cv.increment()};
    splitVector = telemetry::utilities::split_str(incrementedVector, '.');
    REQUIRE(splitVector.size() == 2);
    REQUIRE(splitVector[1] == "1");
}

TEST_CASE("CreateExtendAndIncrement_FromGuid_V2")
{
    telemetry::guid guid{telemetry::guid::create()};
    std::string expectedCvBase{guid.to_base64_string().substr(0, 22)};

    telemetry::correlation_vector cv{guid};
    REQUIRE(cv.version() == telemetry::correlation_vector_version::v2);

    std::vector<std::string> splitVector{telemetry::utilities::split_str(cv.value(), '.')};

    REQUIRE(splitVector.size() == 2);
    REQUIRE(splitVector[0].length() == 22);
    REQUIRE(expectedCvBase == splitVector[0]);
    REQUIRE(splitVector[1] == "0");

    std::string incrementedVector{cv.increment()};
    splitVector = telemetry::utilities::split_str(incrementedVector, '.');
    REQUIRE(splitVector.size() == 2);
    REQUIRE(splitVector[1] == "1");
}

TEST_CASE("Extend_FromString_V1")
{
    telemetry::correlation_vector cv{telemetry::correlation_vector::extend("tul4NUsfs9Cl7mOf.1")};
    std::vector<std::string> splitVector{telemetry::utilities::split_str(cv.value(), '.')};

    REQUIRE(splitVector.size() == 3);
    REQUIRE(splitVector[2] == "0");

    std::string incrementedVector{cv.increment()};
    splitVector = telemetry::utilities::split_str(incrementedVector, '.');
    REQUIRE(splitVector.size() == 3);
    REQUIRE(splitVector[2] == "1");

    REQUIRE(cv.to_string() == "tul4NUsfs9Cl7mOf.1.1");
}

TEST_CASE("Extend_FromString_V2")
{
    telemetry::correlation_vector cv{telemetry::correlation_vector::extend("KZY+dsX2jEaZesgCPjJ2Ng.1")};
    std::vector<std::string> splitVector{telemetry::utilities::split_str(cv.value(), '.')};

    REQUIRE(splitVector.size() == 3);
    REQUIRE(splitVector[2] == "0");

    std::string incrementedVector{cv.increment()};
    splitVector = telemetry::utilities::split_str(incrementedVector, '.');
    REQUIRE(splitVector.size() == 3);
    REQUIRE(splitVector[2] == "1");

    REQUIRE(cv.to_string() == "KZY+dsX2jEaZesgCPjJ2Ng.1.1");
}

TEST_CASE("Extend_EmptyString") { REQUIRE_THROWS_AS(telemetry::correlation_vector::extend(""), std::invalid_argument); }

TEST_CASE("Extend_WhiteSpaceString")
{
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("  "), std::invalid_argument);
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("\t  "), std::invalid_argument);
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("\t\n"), std::invalid_argument);
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("\n"), std::invalid_argument);
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("  \n"), std::invalid_argument);
}

TEST_CASE("Extend_InsufficientChars")
{
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("tul4NUsfs9Cl7mO.1"), std::invalid_argument);
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("tul4NUsfs9Cl7mO.1"), std::invalid_argument);
}

TEST_CASE("Extend_TooManyChars")
{
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("tul4NUsfs9Cl7mOfN/dupsl.1"), std::invalid_argument);
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("tul4NUsfs9Cl7mOfN/dupsl.1"), std::invalid_argument);
}

TEST_CASE("Extend_TooLong_V1")
{
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend(
                          "tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.2147483647.2147483647"),
                      std::invalid_argument);
}

TEST_CASE("Extend_TooLong_V2")
{
    REQUIRE_THROWS_AS(
        telemetry::correlation_vector::extend("KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647."
                                              "2147483647.2147483647.2147483647.2147483647.2147483647.2147483647"),
        std::invalid_argument);
}

TEST_CASE("Extend_TooLongExtension")
{
    REQUIRE_THROWS_AS(telemetry::correlation_vector::extend("tul4NUsfs9Cl7mOf.11111111111111111111111111111"),
                      std::invalid_argument);
}

TEST_CASE("Extend_OverMaxLength_V1")
{
    telemetry::correlation_vector cv{
        telemetry::correlation_vector::extend("tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.214748364.23")};
    REQUIRE(cv.value() == "tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.214748364.23!");
}

TEST_CASE("Extend_OverMaxLength_V2")
{
    telemetry::correlation_vector cv{
        telemetry::correlation_vector::extend("KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647."
                                              "2147483647.2147483647.2147483647.2147483647.2147483647.2141")};
    REQUIRE(cv.value() == "KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647."
                          "2147483647.2147483647.2147483647.2141!");
}

TEST_CASE("Increment_PastMaxWithTerminator_V1")
{
    telemetry::correlation_vector cv{
        telemetry::correlation_vector::extend("tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.2147483647")};
    cv.increment();
    REQUIRE(cv.value() == "tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.2147483647.1");
    for (int i = 0; i < 99; ++i)
    {
        cv.increment();
    }

    REQUIRE(cv.value() == "tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.2147483647.99!");
}

TEST_CASE("Increment_PastMaxWithTerminator_V2")
{
    telemetry::correlation_vector cv{
        telemetry::correlation_vector::extend("KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647."
                                              "2147483647.2147483647.2147483647.2147483647.2147483647.214")};
    cv.increment();
    REQUIRE(cv.value() == "KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647."
                          "2147483647.2147483647.2147483647.214.1");

    for (int i = 0; i < 9; ++i)
    {
        cv.increment();
    }

    REQUIRE(cv.value() == "KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647."
                          "2147483647.2147483647.2147483647.214.9!");
}

TEST_CASE("ParseExtendAndSpin_ImmutableWithTerminator_V1")
{
    std::string cvStr{"tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.2147483647.0!"};

    REQUIRE(cvStr == telemetry::correlation_vector::parse(cvStr).increment());
    REQUIRE(cvStr == telemetry::correlation_vector::extend(cvStr).value());
    REQUIRE(cvStr == telemetry::correlation_vector::spin(cvStr).value());
}

TEST_CASE("ParseExtendAndSpin_ImmutableCVWithTerminator_V2")
{
    std::string cvStr{"KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647."
                      "2147483647.2147483647.2147483647.214.0!"};

    REQUIRE(cvStr == telemetry::correlation_vector::parse(cvStr).increment());
    REQUIRE(cvStr == telemetry::correlation_vector::extend(cvStr).value());
    REQUIRE(cvStr == telemetry::correlation_vector::spin(cvStr).value());
}

TEST_CASE("Spin_OverMaxLength_V1")
{
    std::string baseVector{"tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.214748364.23"};

    telemetry::correlation_vector cv{telemetry::correlation_vector::spin(baseVector)};
    REQUIRE((baseVector + telemetry::correlation_vector::TERMINATOR) == cv.value());
}

TEST_CASE("Spin_OverMaxLength_V2")
{
    std::string baseVector{"KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647."
                           "2147483647.2147483647.2147483647.214"};

    telemetry::correlation_vector cv{telemetry::correlation_vector::spin(baseVector)};
    REQUIRE((baseVector + telemetry::correlation_vector::TERMINATOR) == cv.value());
}

TEST_CASE("Spin_SortValidation")
{
    telemetry::correlation_vector cv;

    telemetry::spin_parameters parameters = telemetry::spin_parameters();
    parameters.entropy(telemetry::spin_entropy::two);
    parameters.interval(telemetry::spin_counter_interval::fine);
    parameters.periodicity(telemetry::spin_counter_periodicity::short_length);

    long lastSpinValue = 0;
    unsigned int wrappedCounter = 0;
    for (int i = 0; i < 100; ++i)
    {
        telemetry::correlation_vector cv2{telemetry::correlation_vector::spin(cv.value(), parameters)};

        // The cv after a Spin will look like <cvBase>.0.<spinValue>.0, so the spinValue is at index = 2.
        std::vector<std::string> splitVector = telemetry::utilities::split_str(cv2.value(), '.');
        long spinValue = std::stoul(splitVector[2]);

        // Count the number of times the counter wraps.
        if (spinValue <= lastSpinValue)
        {
            wrappedCounter++;
        }

        lastSpinValue = spinValue;

        // Wait for 10ms.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // The counter should wrap at most 1 time.
    REQUIRE(wrappedCounter <= 1);
}