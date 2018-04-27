//---------------------------------------------------------------------
// <copyright file="InternalErrors.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#include "InternalErrors.h"

namespace Microsoft
{
	std::atomic<int> InternalErrors::maxSavedErrorCount = 1;
	std::atomic<int> InternalErrors::savedErrorCount = 0;
	std::queue<std::string> InternalErrors::errors;

	void InternalErrors::reportError(std::string error)
	{
		if (InternalErrors::maxSavedErrorCount.load() == 0)
		{
			return;
		}
		InternalErrors::errors.push(error);
		InternalErrors::savedErrorCount.fetch_add(1);

		int current = InternalErrors::savedErrorCount.load();
		while (current > InternalErrors::maxSavedErrorCount.load())
		{
			InternalErrors::errors.pop();
			current = InternalErrors::savedErrorCount.fetch_sub(1) - 1;
		}
	}

	std::queue<std::string> InternalErrors::getErrors()
	{
		return InternalErrors::errors;
	}

	bool InternalErrors::hasErrors()
	{
		return !InternalErrors::errors.empty();
	}

	void InternalErrors::clearErrors()
	{
		std::queue<std::string> empty;
		std::swap(InternalErrors::errors, empty);
		InternalErrors::savedErrorCount.store(0);
	}

	int InternalErrors::getMaxSavedErrorCount()
	{
		return InternalErrors::maxSavedErrorCount;
	}

	void InternalErrors::setMaxSavedErrorCount(int count)
	{
		InternalErrors::maxSavedErrorCount.store(count < 0 ? 0 : (count > InternalErrors::MAX_SAVED_ERRORS_LIMIT ? InternalErrors::MAX_SAVED_ERRORS_LIMIT : count));
	}

}