//---------------------------------------------------------------------
// <copyright file="CorrelationVector.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#include "CorrelationVector.h"
#include "Utilities.h"
#include "InternalErrors.h"
#include <ctime>
#include <chrono>
#include <vector>
#include <string>

#define isEmptyOrWhiteSpace(str)	str.empty() || str.find_first_not_of(' ') == str.npos

namespace Microsoft
{
	bool CorrelationVector::ValidateCorrelationVectorDuringCreation;
	const std::string CorrelationVector::HEADER_NAME = "MS-CV";
	const char CorrelationVector::TERMINATOR = '!';

	CorrelationVector::CorrelationVector()
		: CorrelationVector(CorrelationVectorVersion::V1)
	{
	}

	CorrelationVector::CorrelationVector(Microsoft::Guid guid)
		: CorrelationVector(getBaseFromGuid(&guid), 0, CorrelationVectorVersion::V2, false)
	{
	}

	CorrelationVector::CorrelationVector(CorrelationVectorVersion version)
		: CorrelationVector(CorrelationVector::getUniqueValue(version), 0, version, false)
	{
	}

	CorrelationVector::CorrelationVector(const CorrelationVector &cV)
	{
		this->baseVector = cV.baseVector;
		this->extension = cV.extension.load();
		this->correlationVectorVersion = cV.correlationVectorVersion;
		this->isCvImmutable = cV.isCvImmutable;
	}

	CorrelationVector::CorrelationVector(std::string baseVector, int extension, CorrelationVectorVersion version, bool isImmutable)
	{
		this->baseVector = baseVector;
		this->extension = extension;
		this->correlationVectorVersion = version;
		this->isCvImmutable = isImmutable || CorrelationVector::isOversized(baseVector, extension, version);
	}

	bool CorrelationVector::getValidateCorrelationVectorDuringCreation()
	{
		return CorrelationVector::ValidateCorrelationVectorDuringCreation;
	}

	void CorrelationVector::setValidateCorrelationVectorDuringCreation(bool value)
	{
		CorrelationVector::ValidateCorrelationVectorDuringCreation = value;
	}

	std::string CorrelationVector::getBaseFromGuid(Microsoft::Guid* guid)
	{
		return guid->toBase64String().substr(0, CorrelationVector::BASE_LENGTH_V2);
	}

	std::string CorrelationVector::getUniqueValue(CorrelationVectorVersion version)
	{
		if (CorrelationVectorVersion::V1 == version)
		{
			Microsoft::Guid guid = Microsoft::Guid::newGuid();
			return guid.toBase64String(12);
		}
		else if (CorrelationVectorVersion::V2 == version)
		{
			Microsoft::Guid guid = Microsoft::Guid::newGuid();
			return guid.toBase64String();
		}
		else
		{
			throw std::invalid_argument("Unsupported correlation vector version: " + (int)version);
		}
	}

	CorrelationVectorVersion CorrelationVector::inferVersion(std::string correlationVector, bool reportErrors)
	{
		size_t index = correlationVector.empty() ? -1 : correlationVector.find_first_of('.');

		if (BASE_LENGTH == index)
		{
			return CorrelationVectorVersion::V1;
		}
		else if (BASE_LENGTH_V2 == index)
		{
			return CorrelationVectorVersion::V2;
		}
		else
		{
			if (reportErrors)
			{
				Microsoft::InternalErrors::reportError("Invalid correlation vector " + correlationVector);
			}

			// Fallback to V1 implementation for invalid cVs
			return CorrelationVectorVersion::V1;
		}
	}

	void CorrelationVector::validate(std::string correlationVector, CorrelationVectorVersion version)
	{
		try
		{
			size_t maxVectorLength;
			size_t baseLength;

			if (CorrelationVectorVersion::V1 == version)
			{
				maxVectorLength = CorrelationVector::MAX_VECTOR_LENGTH;
				baseLength = CorrelationVector::BASE_LENGTH;
			}
			else if (CorrelationVectorVersion::V2 == version)
			{
				maxVectorLength = CorrelationVector::MAX_VECTOR_LENGTH_V2;
				baseLength = CorrelationVector::BASE_LENGTH_V2;
			}
			else
			{
				throw std::invalid_argument("Unsupported correlation vector version: " + (int)version);
			}

			if (isEmptyOrWhiteSpace(correlationVector) || correlationVector.length() > maxVectorLength)
			{
				throw std::invalid_argument("The " + correlationVector + " correlation vector can not be null or bigger than " + std::to_string(maxVectorLength) + " characters");
			}

			std::vector<std::string> parts = split_str(correlationVector, '.');

			size_t length = parts.size();
			if (length < (size_t)2 || parts[0].length() != baseLength)
			{
				throw std::invalid_argument("Invalid correlation vector " + correlationVector + ". Invalid base value " + parts[0]);
			}

			for (size_t i = 1; i < length; ++i)
			{
				try
				{
					std::size_t lastChar;
					int result = std::stoi(parts[i], &lastChar, 10);
					if (lastChar != parts[i].length() || result < 0)
					{
						throw std::invalid_argument("Invalid correlation vector " + correlationVector + ". Invalid extension value " + parts[i]);
					}
				}
				catch (std::invalid_argument&)
				{
					throw std::invalid_argument("Invalid correlation vector " + correlationVector + ". Invalid extension value " + parts[i]);
				}
				catch (std::out_of_range&)
				{
					throw std::invalid_argument("Invalid correlation vector " + correlationVector + ". Invalid extension value " + parts[i]);
				}
			}
		}
		catch (std::invalid_argument& exception)
		{
			Microsoft::InternalErrors::reportError(exception.what());
		}
	}

	int CorrelationVector::intLength(int i)
	{
		return (i == 0) ? 1 : (int) std::log10(i) + 1;
	}

	bool CorrelationVector::isImmutable(std::string correlationVector)
	{
		return !correlationVector.empty() && correlationVector.at(correlationVector.length() - 1) == TERMINATOR;
	}

	bool CorrelationVector::isOversized(std::string baseVector, int extension, CorrelationVectorVersion version)
	{
		if (baseVector.empty())
		{
			return false;
		}

		size_t cvLen = baseVector.length() + 1 + intLength(extension);
		return (version == CorrelationVectorVersion::V1 && cvLen > CorrelationVector::MAX_VECTOR_LENGTH)
			|| (version == CorrelationVectorVersion::V2 && cvLen > CorrelationVector::MAX_VECTOR_LENGTH_V2);
	}

	CorrelationVector CorrelationVector::extend(std::string correlationVector)
	{
		if (CorrelationVector::isImmutable(correlationVector))
		{
			return CorrelationVector::parse(correlationVector);
		}

		CorrelationVectorVersion version = CorrelationVector::inferVersion(
			correlationVector, CorrelationVector::ValidateCorrelationVectorDuringCreation);

		if (CorrelationVector::ValidateCorrelationVectorDuringCreation)
		{
			CorrelationVector::validate(correlationVector, version);
		}

		if (CorrelationVector::isOversized(correlationVector, 0, version))
		{
			return CorrelationVector::parse(correlationVector + CorrelationVector::TERMINATOR);
		}

		return CorrelationVector(correlationVector, 0, version, false);
	}

	CorrelationVector CorrelationVector::spin(std::string correlationVector)
	{
		return CorrelationVector::spin(correlationVector, SpinParameters::getDefaultSpinParameters());
	}

	CorrelationVector CorrelationVector::spin(std::string correlationVector, SpinParameters parameters)
	{
		if (CorrelationVector::isImmutable(correlationVector))
		{
			return CorrelationVector::parse(correlationVector);
		}

		CorrelationVectorVersion version = CorrelationVector::inferVersion(
			correlationVector, CorrelationVector::ValidateCorrelationVectorDuringCreation);

		if (CorrelationVector::ValidateCorrelationVectorDuringCreation)
		{
			CorrelationVector::validate(correlationVector, version);
		}

		int entropyBytes = parameters.getEntropyBytes();
		unsigned char* entropy = new unsigned char[entropyBytes]();

		std::srand((unsigned int)std::time(nullptr));

		for (int i = 0; i < entropyBytes; ++i)
		{
			entropy[i] = (unsigned char)rand();
		}

		long long ticks = std::chrono::system_clock::now().time_since_epoch().count();
		long long value = ticks >> parameters.getTicksBitsToDrop();
		for (int i = 0; i < entropyBytes; ++i)
		{
			value = (value << 8) | ((uint64_t)entropy[i]);
		}
		int totalBits = parameters.getTotalBits();
		value &= (totalBits == 64 ? 0 : (long long)1 << totalBits) - 1;

		std::string s = std::to_string((unsigned int)value);
		if (totalBits > 32)
		{
			s = std::to_string((unsigned int)(value >> 32)) + '.' + s;
		}

		std::string baseVector = correlationVector + '.' + s;
		if (CorrelationVector::isOversized(baseVector, 0, version))
		{
			return parse(correlationVector + CorrelationVector::TERMINATOR);
		}
		return CorrelationVector(baseVector, 0, version, false);
	}

	CorrelationVector CorrelationVector::parse(std::string correlationVector)
	{
		if (!correlationVector.empty())
		{
			size_t p = correlationVector.find_last_of('.');
			bool isImmutable = CorrelationVector::isImmutable(correlationVector);

			if (p > 0)
			{
				std::string lastStage = correlationVector.substr(p + 1);
				try
				{

					int extension = isImmutable ? std::stoi(lastStage.substr(0, lastStage.length() - 1)) : std::stoi(lastStage);
					int extLen = CorrelationVector::intLength(extension);
					bool isValidExt = isImmutable ? (extLen == lastStage.length() - 1) : (extLen == lastStage.length());
					if (isValidExt && extension >= 0)
					{
						CorrelationVectorVersion version = CorrelationVector::inferVersion(correlationVector, false);
						return CorrelationVector(correlationVector.substr(0, p), extension, version, isImmutable);
					}
				}
				catch (std::invalid_argument)
				{
				}
				catch (std::out_of_range&)
				{
				}
			}
		}
		return CorrelationVector();
	}

	std::string CorrelationVector::getValue()
	{
		std::string cv = this->baseVector + '.' + std::to_string(this->extension);
		if (this->isCvImmutable)
		{
			cv += CorrelationVector::TERMINATOR;
		}
		return cv;
	}

	std::string CorrelationVector::increment()
	{

		if (this->isCvImmutable)
		{
		    return this->getValue();
		}

		int snapshot = 0;
		int next = 0;

		do
		{
			snapshot = this->extension.load();
			if (snapshot == INT_MAX)
			{
				return this->getValue();
			}
			next = snapshot + 1;
			size_t size = baseVector.length() + 1 + (int)std::log10(next) + 1;
			if (CorrelationVector::isOversized(this->baseVector, next, this->correlationVectorVersion))
			{
				this->isCvImmutable = true;
				return this->getValue();
			}
		} while (!this->extension.compare_exchange_weak(snapshot, next));

		return this->baseVector + '.' + std::to_string(next);
	}

	CorrelationVectorVersion CorrelationVector::getVersion()
	{
		return this->correlationVectorVersion;
	}

	std::string CorrelationVector::toString()
	{
		return this->getValue();
	}

	bool CorrelationVector::equals(CorrelationVector vector)
	{
		return this->baseVector == vector.baseVector && this->extension == vector.extension;
	}
}
