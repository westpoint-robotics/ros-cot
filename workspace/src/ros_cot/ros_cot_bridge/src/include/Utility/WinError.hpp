#pragma once

#ifdef _WIN64
#include <Windows.h>
#endif // _WIN64

namespace Utility {
	
	void SupressCrash() {
			
#ifdef _WIN64

		DWORD oldMode;

		SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
		SetThreadErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX, &oldMode);
		_set_abort_behavior(0, _WRITE_ABORT_MSG);

#endif // _WIN64

	}

}
