#pragma once

#include <cereal/access.hpp>
#include "Utility/Exception.hpp"

namespace ARL {
	namespace Math {

		template <typename T, int min, int max > /// value restricted to range [min, max]
		class RestrictedValue {
		public:
			RestrictedValue(const T& value = min) { setValue(value); }
			void setValue(const T& value) {
				if (value <= T(max) && value >= T(min))
					this->value = value;
				else
					throw Exception("restricted value out of range");
			}
			const T& getValue() const { return value; }
			operator const T&() const { return value; }

		protected:
			T value;

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(value);
			}
		};

	}
}