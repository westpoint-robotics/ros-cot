#pragma once
#include <condition_variable>
#include <mutex>

namespace Utility {
	/**
		* Simple semaphore.  Wait() enqueues the thread until
		* another thread calls Post().
		*/
	class Semaphore {
	public:
		Semaphore(unsigned init = 0)
			: mWaitSize(init) {}

		void Post() {
			std::unique_lock<decltype(mMtx)> lk(mMtx);
			++mWaitSize;
			mCV.notify_one();
		}

		void Wait() {
			std::unique_lock<decltype(mMtx)> lk(mMtx);
			mCV.wait(lk, [this] {return mWaitSize > 0; });
			--mWaitSize;
		}

	private:
		unsigned mWaitSize;
		std::condition_variable mCV;
		std::mutex mMtx;
	};
}
