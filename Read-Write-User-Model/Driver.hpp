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
