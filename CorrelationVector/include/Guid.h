//---------------------------------------------------------------------
// <copyright file="Guid.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#pragma once
#include <string>
#include <vector>
#include "export.h"

namespace Microsoft
{
	class EXPORTABLE Guid
	{
	public:
		Guid(const unsigned char* bytes);
		Guid(const Guid &other);

		static Guid newGuid();
		static Guid empty();

		std::string toString();
		std::string toBase64String(int len = 16);

	private:
		Guid();
		unsigned char bytes[16] = { 0 };

		void clear();
	};
}
