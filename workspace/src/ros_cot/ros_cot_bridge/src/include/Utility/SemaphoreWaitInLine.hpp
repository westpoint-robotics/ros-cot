#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace Utility {
	/**
	* Simple semaphore.  Wait() enqueues the thread until
	* another thread calls Post().
	*/
	class SemaphoreWaitInLine {
	public:
		SemaphoreWaitInLine(){}

		void Post() {
			std::unique_lock<decltype(mMtx)> lk(mMtx);
			q.pop();
			if (q.size() > 0)
			{
				currentThreadID = q.front();
				mCV.notify_all();
			}
		}

		void Wait() {
			std::unique_lock<decltype(mMtx)> lk(mMtx);
			q.push(std::this_thread::get_id());
			mCV.wait(lk, [this] {return (std::this_thread::get_id() == currentThreadID || q.size() == 1); });
		}

	private:
		std::condition_variable mCV;
		std::mutex mMtx;
		std::thread::id currentThreadID;
		std::queue<std::thread::id> q;
	};
}

