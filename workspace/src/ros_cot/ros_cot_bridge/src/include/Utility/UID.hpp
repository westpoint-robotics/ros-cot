#pragma once

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <atomic>
#include "../Messaging/macro.h"

namespace ARL
{
namespace Utility
{

class UID
{
public:

	typedef uint_fast64_t id_t;

	static id_t GetUID()
	{
		assert(globalID < ~(0ULL));
		return ++globalID;
	}

	static id_t InvalidID()
	{
		return 0;
	}

private:
	static std::atomic_uint_fast64_t globalID;

	UID() {}
	DISALLOW_COPY_AND_ASSIGN(UID);
};
}
}
