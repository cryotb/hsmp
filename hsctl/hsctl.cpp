#include "pch.h"

class C_HSCTL
{
public:
	C_HSCTL() = default;
	~C_HSCTL() = default;

	auto IsActive() CONST { return m_bActive; }

	C_HSCTL(const std::wstring& deviceName)
	{
		m_hDevice = CreateFileW(fmt::format(L"\\\\.\\{}", deviceName).c_str(), GENERIC_ALL, 0, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_SYSTEM, nullptr);

		if (m_hDevice == nullptr || m_hDevice == INVALID_HANDLE_VALUE)
			return;

		m_bActive = TRUE;
	}

	[[nodiscard]] BOOLEAN GetStatus(comms::K64StatusInfo_t& sData) CONST
	{
		assert(m_bActive);

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::GET_STATUS), SCYTHE_UNUSED_BUFFER, &sData))
			return FALSE;

		return TRUE;
	}

	VOID Release()
	{
		if (!m_bActive)
			return;

		m_bActive = FALSE;

		CloseHandle(m_hDevice);
	}
private:
	BOOLEAN m_bActive{};
	HANDLE m_hDevice{};
};

int main()
{
	auto sDriver = C_HSCTL{ L"hsmp" };

	if(sDriver.IsActive())
	{
		comms::K64StatusInfo_t sStatus{};

		if(sDriver.GetStatus(sStatus))
		{
			fmt::print("status: {}, mm'd: {}. \n", 
				sStatus.CurrentState, sStatus.ManuallyMapped);
		}

		sDriver.Release();
	}

	return std::getchar();
}
