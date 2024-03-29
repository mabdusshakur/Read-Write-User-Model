#include "skCrypter.h"
#include <windows.h>

#define DRIVER_NAME skCrypt(L"\\\\.\\rwkmdrv")

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#define FILE_DEVICE_UNKNOWN             0x00000022
#define METHOD_BUFFERED                 0
#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)

#define IO_INIT_REQUEST             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x991, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // Initialize
#define IO_READ_REQUEST             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x992, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // RPM
#define IO_WRITE_REQUEST            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x993, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // WPM

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;

typedef struct init_t {
	DWORD32 processId;
	NTSTATUS result;
} init, * p_init;

typedef struct read_t {
	DWORD_PTR address;
	void* response;
	SIZE_T size;
	NTSTATUS result;
} read, * p_read;

typedef struct write_t {
	DWORD_PTR address;
	void* value;
	SIZE_T size;
	NTSTATUS result;
} write, * p_write;

namespace Driver {
	inline HANDLE driverHandle;
	__forceinline auto isvalidptr(uint64_t ptr) {
		if (ptr < 0x500 || ptr > 0x7FFFFFFFFFFF) return false;
		return true;
	}

	__forceinline auto initialize(DWORD pId) -> bool {
		driverHandle = CreateFileW(DRIVER_NAME, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (driverHandle == INVALID_HANDLE_VALUE) [[unlikely]] {
			return false;
			}

		init_t ioData;
		ioData.processId = pId;
		ioData.result = -1;

		DeviceIoControl(driverHandle, IO_INIT_REQUEST, &ioData, sizeof(ioData), &ioData, sizeof(ioData), nullptr, nullptr);
		return ioData.result == 0;
	}

	template <typename Type>
	__forceinline auto read(uintptr_t address) -> Type
	{
		if (address < 0x0 || address > 0x7FFFFFFFFFFF) return Type();
		read_t ioData;
		ioData.address = address;

		Type returnValue;
		ioData.response = &returnValue;
		ioData.size = sizeof(Type);
		ioData.result = -1;

		DeviceIoControl(driverHandle, IO_READ_REQUEST, &ioData, sizeof(ioData), &ioData, sizeof(ioData), nullptr, nullptr);
		if (ioData.result != 0) return Type();
		return *(Type*)&returnValue;
	}

	template <typename Type>
	__forceinline auto write(uint64_t address, Type value) -> NTSTATUS
	{
		if (address < 0x0 || address > 0x7FFFFFFFFFFF) return -1;
		write_t ioData;
		ioData.size = sizeof(Type);
		ioData.address = address;
		ioData.value = &value;
		ioData.result = -1;

		DeviceIoControl(driverHandle, IO_WRITE_REQUEST, &ioData, sizeof(ioData), &ioData, sizeof(ioData), nullptr, nullptr);
		return ioData.result;
	}
}