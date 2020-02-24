#pragma once
#include "Geodetics.hpp"
#include "Types.hpp"

namespace ARL {
	namespace Math {
		
		class CoordinateTransform {
		public:
			/// Convert a GeoCoordinate egocentric PolarCoordinate w.r.t Origin and compute egocentric first derivative
			///
			///@param deltaT in seconds
			static PolarVelocity GeoCoordinateToPolar(const ARL::Math::GeoCoordinate &geoLocation,
				const ARL::Math::Orientation1D &orientation,
				const ARL::Math::GeoCoordinate3D &origin,
				const double deltaT,
				PolarCoordinate& polar) {

				PolarCoordinate prev = polar;
				polar = origin.GeodeticToEgocentric(orientation, geoLocation);
				return PolarVelocity(polar, prev, deltaT);
			}

		protected:
		};
	}
}