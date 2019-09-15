#include "Exception.h"

Exception::Exception(std::wstring message)
{
	this->message = message;
}

std::wstring Exception::GetDetails() noexcept
{
	return this->message;
}
