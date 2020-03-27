#pragma once

#include <chrono>
#include <memory>

namespace Utility
{
	class RuntimeClock
	{
	public:
		/**
			* @return Program runtime in microseconds. 
			*/
		static long long Now()
		{
			return mClock->NowInternal();
		}

	private:
		RuntimeClock()
		{
			mInitTime = std::chrono::system_clock::now();
		}

		long long NowInternal() const
		{
			return std::chrono::duration_cast<std::chrono::microseconds>
				(
				std::chrono::system_clock::now() - mInitTime
				).count();
		}

		std::chrono::system_clock::time_point mInitTime;
		static std::unique_ptr<RuntimeClock> mClock;
	};
}
