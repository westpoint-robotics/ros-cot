#pragma once

#include "RestrictedValue.hpp"

namespace ARL {
	namespace Math {

		class NormalizedValue : public RestrictedValue<double, -1, 1> {
		public:
			NormalizedValue(const double& value = 0) { setValue(value); }
		protected:
			friend class cereal::access;
		};

	}
}