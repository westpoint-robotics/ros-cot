#pragma once

#include "Types.hpp"

namespace ARL {
	namespace Math {

		namespace Geodetic {
			/**	A class to represent 6DOF pose space (WGS 84 GeoCoordinate3D + Orientation3D)
			*
			*/
			class GeoPose : public GeoCoordinate3D, public Orientation3D {
			public:
				Angle GeodeticToEgocentricBearing(const GeoCoordinate3D& point) const {
					lock_guard<mutex> lk(poseMutex);
					return GeoCoordinate3D::GeodeticToEgocentricBearing(*this, point);
				}

				/** Project GeoCoodinate point into egocentric Cylindrical coordinates
				*
				*	@return Cylindrical projection of point w.r.t. this pose space.
				*/
				CylindricalCoordinate GeodeticToEgocentricCylindrical(const GeoCoordinate3D& point) const {
					lock_guard<mutex> lk(poseMutex);
					return CylindricalCoordinate(GeodeticToEgocentricBearing(point), GreatCircleDistance(point), point.altitude - altitude);
				}

				/** Project GeoCoodinate point into egocentric cartesian coordinates
				*
				*	@return Cartesian projection of point w.r.t. this pose space.
				*/
				Coordinate3D GeodeticToEgocentricCartesian(const GeoCoordinate3D& point) const {
					return GeodeticToEgocentricCylindrical(point).toCartesian();
				}

				/** Project Egocentric cartesian point into WGS84 coordinates.
				*
				*	@return GeoCoordinate projection of point w.r.t. this pose space.
				*/
				GeoCoordinate3D EgocentricToGeodetic(const Coordinate3D&) const {
					throw NotImplementedException();
				}

				void filteredUpdate(const GeoCoordinate3D& position, const double alpha = 0.25) { lock_guard<mutex> lk(poseMutex); GeoCoordinate3D::filteredUpdate(position, alpha); }
				void filteredUpdate(const Orientation3D& orientation, const double alpha = 0.25) { lock_guard<mutex> lk(poseMutex); Orientation3D::filteredUpdate(orientation, alpha); }
				void filteredUpdate(const GeoPose& pose, const double alpha = 0.25) {
					lock_guard<mutex> lk(poseMutex);
					GeoCoordinate3D::filteredUpdate(pose, alpha);
					Orientation3D::filteredUpdate(pose, alpha);
				}
				void filteredUpdate(const GeoCoordinate3D& position, const Orientation3D& orientation, const double alpha = 0.25) {
					lock_guard<mutex> lk(poseMutex);
					GeoCoordinate3D::filteredUpdate(position, alpha);
					Orientation3D::filteredUpdate(orientation, alpha);
				}
				GeoPose(const GeoPose &) = default; // added for vs2015
				GeoPose()=default;                  // added for vs2015
			protected:
				mutable mutex poseMutex;
			};
		}
	}
}