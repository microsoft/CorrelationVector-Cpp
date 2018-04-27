//---------------------------------------------------------------------
// <copyright file="Guid.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//---------------------------------------------------------------------
#include "Guid.h"
#include "Utilities.h"

#ifdef GUID_WINDOWS
#include "Objbase.h"
#endif

#ifdef GUID_LIBUUID
#include <uuid/uuid.h>
#endif

namespace Microsoft
{
	Guid::Guid()
	{
	}

	Guid::Guid(const unsigned char* bytes)
	{
		std::copy(bytes, bytes + 16, this->bytes);
	}

	Guid::Guid(const Guid &other)
	{
		std::copy(other.bytes, other.bytes + 16, this->bytes);
	}

	Guid Guid::newGuid()
	{
#ifdef GUID_WINDOWS
		GUID guid;
		HRESULT hCreateGuid = CoCreateGuid(&guid);
		return Guid((unsigned char*)&guid);
#endif

#ifdef GUID_LIBUUID
		uuid_t id;
		uuid_generate(id);
		return Guid(id);
#endif
	}

	Guid Guid::empty()
	{
		return Guid();
	}

	void Guid::clear()
	{
		std::fill(this->bytes, this->bytes + 16, static_cast<unsigned char>(0));
	}

	std::string Guid::toString()
	{
		return to_hex_str(this->bytes, 0, 4)
			+ '-' + to_hex_str(this->bytes, 4, 2)
			+ '-' + to_hex_str(this->bytes, 6, 2)
			+ '-' + to_hex_str(this->bytes, 8, 2)
			+ '-' + to_hex_str(this->bytes, 10, 6);
	}

	static const unsigned char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string Guid::toBase64String(int len)
	{
		int outputLength = (int)ceil(len * 8 / 6.0);
		std::string s(outputLength, ' ');

		unsigned char buffer[3];

		for (int i = 0, j = 0; i < len;)
		{
			buffer[i++ % 3] = this->bytes[i];
			if (i % 3 == 0)
			{
				s[j] = base64_table[(buffer[0] & 0xFC) >> 2];
				s[j + 1] = base64_table[((buffer[0] & 0x03) << 4) + ((buffer[1] & 0xF0) >> 4)];
				s[j + 2] = base64_table[((buffer[1] & 0x0F) << 2) + ((buffer[2] & 0xC0) >> 6)];
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
				s[outputLength - 2] = base64_table[((buffer[0] & 0x03) << 4) + ((buffer[1] & 0xF0) >> 4)];
				s[outputLength - 1] = base64_table[((buffer[1] & 0x0F) << 2)];
			}
		}

		return s;
	}
}