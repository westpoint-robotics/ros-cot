#pragma once

#include <algorithm>

// Generic solution derived from:
// http://stackoverflow.com/questions/709146/how-do-i-clear-the-stdqueue-efficiently


namespace Utility
{

	template < typename T >
	inline void SwapClear(T *container)
	{
		std::swap(T(), *container);
	}

}
