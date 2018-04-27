//---------------------------------------------------------------------
// <copyright file="InternalErrors.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#pragma once
#include <string>
#include <atomic>
#include <queue>
#include "export.h"

namespace Microsoft
{
	class InternalErrors
	{
	public:
		EXPORTABLE static void reportError(std::string error);
		EXPORTABLE static std::queue<std::string> getErrors();
		EXPORTABLE static bool hasErrors();
		EXPORTABLE static void clearErrors();

		EXPORTABLE static int getMaxSavedErrorCount();
		EXPORTABLE static void setMaxSavedErrorCount(int count);

	private:
		static const int MAX_SAVED_ERRORS_LIMIT = 100;
		static std::atomic<int> maxSavedErrorCount;
		static std::atomic<int> savedErrorCount;

		static std::queue<std::string> errors;

		InternalErrors() {};
	};

}