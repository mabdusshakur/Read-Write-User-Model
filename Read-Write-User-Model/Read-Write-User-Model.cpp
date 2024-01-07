#include <Windows.h>
#include "iostream"
#include "Driver.hpp"

int main()
{
	DWORD PID = GetProcessId("process_name.exe");
	if (!Driver::initialize(PID))
	{
		std::cout << "Failed to initialize Driver" << std::endl;
	}

	//Driver::read();
	//Driver::write();
}