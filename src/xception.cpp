#include "../include/xcept"

#define XCEPTION(_Name) \
	xcept::_Name::_Name(std::string const & _msg) noexcept : \
		msg(_msg) \
	{ \
	} \
	xcept::_Name::~_Name() noexcept \
	{ \
	} \
	const char* xcept::_Name::what() const noexcept \
	{ \
		return msg.c_str(); \
	}

XCEPTION(invalid_argument)
XCEPTION(invalid_operation)
XCEPTION(io_error)
XCEPTION(end_of_stream)
