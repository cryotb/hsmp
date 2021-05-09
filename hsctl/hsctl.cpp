#include "pch.h"

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

			const auto my_base = (uintptr_t)GetModuleHandle(nullptr);
			const auto my_pid = GetCurrentProcessId();

			printf("my_base= %llx, \n",
				my_base);

			uint8_t my_start_buffer[128] = { 0 };

			if(sDriver.UReadVirtualMemory(my_pid, my_base, &my_start_buffer, 128))
			{
				printf("successfully read virtual user memory: %x.%x.%x.%x.\n",
					my_start_buffer[0], my_start_buffer[1], my_start_buffer[2],
					my_start_buffer[3]);

				// sb = start buffer base address.
				const auto my_sb_base = reinterpret_cast<uintptr_t>(&my_start_buffer);

				if(sDriver.UFillVirtualMemory(my_pid, my_sb_base, 0xFF, 128))
				{
					printf("successfully modified virtual user memory: %x.%x.%x.%x.\n",
						my_start_buffer[0], my_start_buffer[1], my_start_buffer[2],
						my_start_buffer[3]);
				}
			}

			DWORD_PTR dwSysRoutineAddress = 0;

			if(sDriver.GetSystemRoutineAddress(L"MmCopyMemory", &dwSysRoutineAddress))
			{
				printf("successfully got system routine address: MmCopyMemory= %p", 
					(void*)dwSysRoutineAddress);
			}
		}

		sDriver.Release();
	}

	return std::getchar();
}
