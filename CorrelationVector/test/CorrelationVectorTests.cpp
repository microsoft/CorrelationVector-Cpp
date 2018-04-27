//---------------------------------------------------------------------
// <copyright file="CorrelationVectorTests.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CorrelationVector.h"
#include "Utilities.h"
#include "InternalErrors.h"
#include "Guid.h"
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_set>
#include <future>

struct TestCaseEventListener : Catch::TestEventListenerBase {

	using TestEventListenerBase::TestEventListenerBase; // inherit constructor

	virtual void testCaseStarting(Catch::TestCaseInfo const& testInfo) override {
		// Resetting all static variables since all Test Cases run in the same process
		Microsoft::CorrelationVector::setValidateCorrelationVectorDuringCreation(false);
		Microsoft::InternalErrors::clearErrors();
	}
};
CATCH_REGISTER_LISTENER(TestCaseEventListener)

TEST_CASE("CorrelationVector_Increment_Is_Unique_Across_Multiple_Threads") 
{
	const int numberOfThreads = 1000;
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector();
	Microsoft::CorrelationVector cV2 = Microsoft::CorrelationVector::extend(cV.getValue());

	std::vector<std::future<std::string>> futures;
	for (int i = 0; i < numberOfThreads; ++i)
	{
		futures.push_back(std::async(&Microsoft::CorrelationVector::increment, &cV2));
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

TEST_CASE("CreateExtendAndIncrementCorrelationVectorDefault")
{
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector();

	std::vector<std::string> splitVector = split_str(cV.getValue(), '.');

	REQUIRE(splitVector.size() == 2);
	REQUIRE(splitVector[0].length() == 16);
	REQUIRE(splitVector[1] == "0");

	std::string incrementedVector = cV.increment();
	splitVector = split_str(incrementedVector, '.');
	REQUIRE(splitVector.size() == 2);
	REQUIRE(splitVector[1] == "1");
}

TEST_CASE("CreateExtendAndIncrementCorrelationVectorV1")
{
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector();

	std::vector<std::string> splitVector = split_str(cV.getValue(), '.');

	REQUIRE(splitVector.size() == 2);
	REQUIRE(splitVector[0].length() == 16);
	REQUIRE(splitVector[1] == "0");

	std::string incrementedVector = cV.increment();
	splitVector = split_str(incrementedVector, '.');
	REQUIRE(splitVector.size() == 2);
	REQUIRE(splitVector[1] == "1");
}

TEST_CASE("CreateExtendAndIncrementCorrelationVectorV2")
{
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector(Microsoft::CorrelationVectorVersion::V2);
	REQUIRE(cV.getVersion() == Microsoft::CorrelationVectorVersion::V2);

	std::vector<std::string> splitVector = split_str(cV.getValue(), '.');

	REQUIRE(splitVector.size() == 2);
	REQUIRE(splitVector[0].length() == 22);
	REQUIRE(splitVector[1] == "0");

	std::string incrementedVector = cV.increment();
	splitVector = split_str(incrementedVector, '.');
	REQUIRE(splitVector.size() == 2);
	REQUIRE(splitVector[1] == "1");
}

TEST_CASE("CreateExtendAndIncrementCorrelationVectorV2FromGuid")
{
	Microsoft::Guid guid = Microsoft::Guid::newGuid();
	std::string expectedCvBase = guid.toBase64String().substr(0, 22);

	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector(guid);
	REQUIRE(cV.getVersion() == Microsoft::CorrelationVectorVersion::V2);

	std::vector<std::string> splitVector = split_str(cV.getValue(), '.');

	REQUIRE(splitVector.size() == 2);
	REQUIRE(splitVector[0].length() == 22);
	REQUIRE(expectedCvBase == splitVector[0]);
	REQUIRE(splitVector[1] == "0");

	std::string incrementedVector = cV.increment();
	splitVector = split_str(incrementedVector, '.');
	REQUIRE(splitVector.size() == 2);
	REQUIRE(splitVector[1] == "1");
}

TEST_CASE("CreateCorrelationVectorFromString")
{
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend("tul4NUsfs9Cl7mOf.1");
	std::vector<std::string> splitVector = split_str(cV.getValue(), '.');

	REQUIRE(splitVector.size() == 3);
	REQUIRE(splitVector[2] == "0");

	std::string incrementedVector = cV.increment();
	splitVector = split_str(incrementedVector, '.');
	REQUIRE(splitVector.size() == 3);
	REQUIRE(splitVector[2] == "1");

	REQUIRE(cV.toString() == "tul4NUsfs9Cl7mOf.1.1");
}

TEST_CASE("CreateCorrelationVectorFromStringV2")
{
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend("KZY+dsX2jEaZesgCPjJ2Ng.1");
	std::vector<std::string> splitVector = split_str(cV.getValue(), '.');

	REQUIRE(splitVector.size() == 3);
	REQUIRE(splitVector[2] == "0");

	std::string incrementedVector = cV.increment();
	splitVector = split_str(incrementedVector, '.');
	REQUIRE(splitVector.size() == 3);
	REQUIRE(splitVector[2] == "1");

	REQUIRE(cV.toString() == "KZY+dsX2jEaZesgCPjJ2Ng.1.1");
}

TEST_CASE("ExtendNullCorrelationVector")
{
	std::string nullString;
	// This shouldn't throw since we skip validation
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend(nullString);
	REQUIRE(cV.toString() == ".0");
	bool hasErrors = Microsoft::InternalErrors::hasErrors();
	REQUIRE_FALSE(hasErrors);

	Microsoft::CorrelationVector::setValidateCorrelationVectorDuringCreation(true);
	Microsoft::CorrelationVector cV2 = Microsoft::CorrelationVector::extend(nullString);
	REQUIRE(cV2.toString() == ".0");
	hasErrors = Microsoft::InternalErrors::hasErrors();
	REQUIRE(hasErrors);
}

TEST_CASE("ThrowWithInsufficientCharsCorrelationVectorValue")
{
	// This shouldn't throw since we skip validation
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend("tul4NUsfs9Cl7mO.1");
	bool hasErrors = Microsoft::InternalErrors::hasErrors();
	REQUIRE_FALSE(hasErrors);

	Microsoft::CorrelationVector::setValidateCorrelationVectorDuringCreation(true);
	Microsoft::CorrelationVector cV2 = Microsoft::CorrelationVector::extend("tul4NUsfs9Cl7mO.1");
	hasErrors = Microsoft::InternalErrors::hasErrors();
	REQUIRE(hasErrors);
}

TEST_CASE("ThrowWithTooManyCharsCorrelationVectorValue")
{
	// This shouldn't throw since we skip validation
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend("tul4NUsfs9Cl7mOfN/dupsl.1");
	bool hasErrors = Microsoft::InternalErrors::hasErrors();
	REQUIRE_FALSE(hasErrors);

	Microsoft::CorrelationVector::setValidateCorrelationVectorDuringCreation(true);
	Microsoft::CorrelationVector cV2 = Microsoft::CorrelationVector::extend("tul4NUsfs9Cl7mOfN/dupsl.1");
	hasErrors = Microsoft::InternalErrors::hasErrors();
	REQUIRE(hasErrors);
}

TEST_CASE("ThrowWithTooBigCorrelationVectorValue")
{
	Microsoft::CorrelationVector::setValidateCorrelationVectorDuringCreation(true);
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend("tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.2147483647.2147483647");
	bool hasErrors = Microsoft::InternalErrors::hasErrors();
	REQUIRE(hasErrors);
}

TEST_CASE("ThrowWithTooBigCorrelationVectorValueV2")
{
	Microsoft::CorrelationVector::setValidateCorrelationVectorDuringCreation(true);
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend("KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647");
	bool hasErrors = Microsoft::InternalErrors::hasErrors();
	REQUIRE(hasErrors);
}

TEST_CASE("ThrowWithTooBigExtensionCorrelationVectorValue")
{
	Microsoft::CorrelationVector::setValidateCorrelationVectorDuringCreation(true);
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend("tul4NUsfs9Cl7mOf.11111111111111111111111111111");
	bool hasErrors = Microsoft::InternalErrors::hasErrors();
	REQUIRE(hasErrors);
}

TEST_CASE("IncrementPastMaxWithNoErrors")
{
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend("tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.21474836479");
	cV.increment();
	REQUIRE(cV.getValue() == "tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.21474836479.1");

	for (int i = 0; i < 20; ++i)
	{
		cV.increment();
	}

	REQUIRE(cV.getValue() == "tul4NUsfs9Cl7mOf.2147483647.2147483647.2147483647.21474836479.9");
}

TEST_CASE("IncrementPastMaxWithNoErrorsV2")
{
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector::extend("KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.214");
	cV.increment();
	REQUIRE(cV.getValue() == "KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.214.1");

	for (int i = 0; i < 20; ++i)
	{
		cV.increment();
	}

	REQUIRE(cV.getValue() == "KZY+dsX2jEaZesgCPjJ2Ng.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.2147483647.214.9");
}

TEST_CASE("SpinSortValidation")
{
	Microsoft::CorrelationVector cV = Microsoft::CorrelationVector();

	Microsoft::SpinParameters parameters = Microsoft::SpinParameters();
	parameters.setEntropy(Microsoft::SpinEntropy::Two);
	parameters.setInterval(Microsoft::SpinCounterInterval::Fine);
	parameters.setPeriodicity(Microsoft::SpinCounterPeriodicity::Short);

	long lastSpinValue = 0;
	unsigned int wrappedCounter = 0;
	for (int i = 0; i < 100; ++i)
	{
		Microsoft::CorrelationVector cV2 = Microsoft::CorrelationVector::spin(cV.getValue(), parameters);

		// The cV after a Spin will look like <cvBase>.0.<spinValue>.0, so the spinValue is at index = 2.
		std::vector<std::string> splitVector = split_str(cV2.getValue(), '.');
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