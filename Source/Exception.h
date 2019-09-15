#pragma once
#include <string>

class Exception
{
public:
	Exception(std::wstring message);
	virtual std::wstring GetDetails() noexcept;

private:
	std::wstring message;
};

