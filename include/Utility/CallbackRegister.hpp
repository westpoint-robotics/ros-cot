#pragma once

#include <functional>
#include <vector>
#include <mutex>

namespace ARL {
	namespace Utility {

		/// A template class to handle the task of registering callbacks and providing a method to invoke callbacks.
		/// derived classes need only INvokeCallback to pass message to all registered callbacks
		/// template parameter T is the data type of the message to be passed as the formal parameter to the callback functions, carrying the message.
		/// i.e. message is of type T.
		template <typename T>
		class CallbackRegister {
		public:
			using message_t = T;
			using callbackArg_t = const message_t&;
			using callback_t = std::function<void(callbackArg_t)>;

			void RegisterCallback(callback_t cb) {
				std::lock_guard<std::mutex> lock(cbMutex);
				mCallbackFunctions.push_back(cb);
			}

		protected:
			void InvokeCallback(const message_t &arg) {
				typename std::vector<callback_t>::const_iterator itr;
				std::lock_guard<std::mutex> lock(cbMutex);
				for (itr = mCallbackFunctions.cbegin();
					itr != mCallbackFunctions.cend(); ++itr) {
					(*itr)(arg);
				}
			}
		private:
			using callbackFunctions_t = std::vector < callback_t >;
			callbackFunctions_t mCallbackFunctions;
			std::mutex cbMutex;
		};

				///Extends CallbackRegister with a public 'NotifyAll' method to allow external objects to trigger callbacks. CallbackRegister only allows derived classes to invoke callbacks.
		template <typename T>
		class CallbackManager : public CallbackRegister < T > {
		public:
			void NotifyAll(const typename CallbackRegister<T>::message_t& arg) {
				CallbackRegister<T>::InvokeCallback(arg);
			}
		};

	}
}
