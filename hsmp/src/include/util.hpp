#pragma once

namespace util
{
	INLINE NTSTATUS CreateDevice(DRIVER_OBJECT* driverObject, DEVICE_OBJECT** outCreatedDeviceObject, UNICODE_STRING* deviceName, const ULONG deviceType, const ULONG deviceCrs) {
		return IoCreateDevice(driverObject, NULL, deviceName, deviceType, deviceCrs, FALSE, outCreatedDeviceObject);
	}

	INLINE NTSTATUS CreateSymLink(UNICODE_STRING* deviceName, UNICODE_STRING* symbolName) {
		return IoCreateSymbolicLink(symbolName, deviceName);
	}
}
