// pch.h: Dies ist eine vorkompilierte Headerdatei.
// Die unten aufgeführten Dateien werden nur einmal kompiliert, um die Buildleistung für zukünftige Builds zu verbessern.
// Dies wirkt sich auch auf die IntelliSense-Leistung aus, Codevervollständigung und viele Features zum Durchsuchen von Code eingeschlossen.
// Die hier aufgeführten Dateien werden jedoch ALLE neu kompiliert, wenn mindestens eine davon zwischen den Builds aktualisiert wird.
// Fügen Sie hier keine Dateien hinzu, die häufig aktualisiert werden sollen, da sich so der Leistungsvorteil ins Gegenteil verkehrt.

#ifndef PCH_H
#define PCH_H

#define FMT_HEADER_ONLY

#include <iostream>

#include <fmt/format.h>

#include <Windows.h>

// Fügen Sie hier Header hinzu, die vorkompiliert werden sollen.

#endif //PCH_H

#define SCYTHE_UNUSED_BUFFER 0xCFD11D1023

namespace Util
{
	template < typename _BUFFER_TYPE_IN, typename _BUFFER_TYPE_OUT >
	inline BOOLEAN DeviceControl(HANDLE hDevice, DWORD dwCode, _BUFFER_TYPE_IN pInBuffer, _BUFFER_TYPE_OUT pOutBuffer)
	{
		auto dwBytesReturned = DWORD{};

		return DeviceIoControl(hDevice, dwCode, reinterpret_cast<void*>(pInBuffer), sizeof(_BUFFER_TYPE_IN),
			reinterpret_cast<void*>(pOutBuffer), sizeof(_BUFFER_TYPE_OUT), &dwBytesReturned, nullptr);
	}
}

namespace comms
{
	namespace IOC
	{
		constexpr auto TranslateCode(UINT32 dwIn) -> ULONG
		{
			return CTL_CODE(FILE_DEVICE_UNKNOWN, dwIn, METHOD_NEITHER, FILE_ANY_ACCESS);
		}

		enum class Code : ULONG
		{
			GET_STATUS = TranslateCode(0x800),
			GET_SUMMARY = TranslateCode(0x801),
			READ_MEMORY = TranslateCode(0x802),
			WRITE_MEMORY = TranslateCode(0x803),
		};
	}

	struct K64StatusInfo_t {
		INT CurrentState;
		BOOLEAN ManuallyMapped;
	};
}
