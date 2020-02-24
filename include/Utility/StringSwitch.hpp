#pragma once

#include <cstring>
#include <string>

namespace Utility {

	template <typename T>
	inline bool StringSwitchSet(const char *str, T *set) {
		boost::ignore_unused_variable_warning(str);
		boost::ignore_unused_variable_warning(set);
		return false;
	}

	template <typename T>
	inline bool WStringSwitchSet(const wchar_t *str, T *set) {
		boost::ignore_unused_variable_warning(str);
		boost::ignore_unused_variable_warning(set);
		return false;
	}

#pragma inline_recursion(on)

	template <typename T, typename... args>
	inline bool StringSwitchSet(const char *str, T *set, const char *comp,
		const T& value, const args&... arg) {
		if (std::strcmp(str, comp) == 0) {
			*set = value;
			return true;
		}
		else return StringSwitchSet<T>(str, set, arg...);
	}

	template <typename T, typename U, typename... args>
	bool StringSwitchSet(const std::basic_string<U> &str, T *set, const char *comp,
		const T &value, const args&... arg) {
		return StringSwitchSet(str.c_str(), set, comp, value, arg...);
	}

	template <typename T, typename... args>
	inline bool WStringSwitchSet(const wchar_t *str, T *set, const wchar_t *comp,
		const T& value, const args&... arg) {
		if (std::wcscmp(str, comp) == 0) {
			*set = value;
			return true;
		}
		else return WStringSwitchSet<T>(str, set, arg...);
	}

	template <typename T, typename U, typename... args>
	bool WStringSwitchSet(const std::basic_string<U> &str, T *set, const wchar_t *comp,
		const T &value, const args&... arg) {
		return WStringSwitchSet(str.c_str(), set, comp, value, arg...);
	}
}
