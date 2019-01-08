#include "../include/xstd/guid"
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cctype>

std::string xstd::to_string(guid const & guid)
{
	char buffer[64];
	using uint = unsigned int;

	snprintf(buffer, sizeof buffer, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02XX%02X%02X%02X%02X%02X",
			uint(guid[0x00]), uint(guid[0x01]), uint(guid[0x02]), uint(guid[0x03]),
	    uint(guid[0x04]), uint(guid[0x05]), uint(guid[0x06]), uint(guid[0x07]),
	    uint(guid[0x08]), uint(guid[0x09]), uint(guid[0x0A]), uint(guid[0x0B]),
	    uint(guid[0x0C]), uint(guid[0x0D]), uint(guid[0x0E]), uint(guid[0x0F])
	);

	return std::string(buffer);
}



xstd::guid xstd::parse_guid(std::string const & text)
{
	char buffer[64];
	strncpy(buffer, text.c_str(), sizeof buffer);

	guid guid;
	size_t offset = 0;
	for(size_t i = 0; i < 16; i++)
	{
		while(offset < sizeof(buffer) and not isxdigit(buffer[offset]))
			offset += 1;
		assert(offset < sizeof(buffer) - 2);
		char str[3] = { buffer[offset + 0], buffer[offset + 1], 0 };
		guid[i] = std::byte(strtoul(str, nullptr, 16));
		offset += 2;
	}
	return guid;
}
