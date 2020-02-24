#pragma once

#include "NormalizedValue.hpp"

namespace ARL {
	namespace Math {

		class PercentValue : public RestrictedValue<double, 0, 100> {
		public:
			PercentValue(const double& value = 0) { setValue(value); }
			operator NormalizedValue() { return NormalizedValue(value / 100); }
		protected:
			friend class cereal::access;
		};

	}
}