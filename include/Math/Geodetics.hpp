#pragma once

#include "Constants.hpp"
#include "Types.hpp"
#include <Eigen/Dense>
#include <mutex>
#include <cassert>
#include <memory>
#include <utility>

namespace ARL {
	namespace Math {

		/// WGS84 Ellipsoid
		///
		/// http://earth-info.nga.mil/GandG/publications/tr8350.2/tr8350.2-a/Chapter%203.pdf

		namespace Geodetic {

			//Somigliana model for gravity (m/s^2) as a function of latitude at the ellipsoid
			inline double go(const double &latitude_radians)
			{
				assert(fabs(latitude_radians) <= piOver2);
				register double s = std::sin(latitude_radians);
				return (9.78032553359 * ((1 + 0.001931853*s*s) / (std::sqrt(1 - e2*s*s))));
			}

			/// compute the radius of the earth as a function of geodetic latitude
			inline double Re(const double latitude_radians) {
				assert(fabs(latitude_radians) <= piOver2);
				register double ac = Re_equator * cos(latitude_radians);
				register double aac = Re_equator * ac;
				register double bc = Re_polar * sin(latitude_radians);
				register double bbc = Re_polar * bc;

				return sqrt((aac*aac + bbc*bbc) / (ac*ac + bc*bc));
			}

			/// compute the radius of curvature in the prime vertical of the earth
			///	aka: The transverse radius of curvature in meters
			inline double N(const double latitude_radians) {
				assert(fabs(latitude_radians) <= piOver2);
				register double s = sin(latitude_radians);
				return Re_equator / sqrt(1 - e2 * s * s);
			}

			/// compute the Meridian radius of curvature in meters
			inline double Rn(const double latitude_radians){
				assert(fabs(latitude_radians) <= piOver2);
				register double s = sin(latitude_radians);
				return ((Re_equator*(1 - e2)) / std::pow((1 - e2*s*s), 3 / 2));
			}

			/// convert from spherical lat,lon,alt to cartesian ECEF (Earth Centered, Earth Fixed)
			/// @param lat, lon in radians; alt in meters MSL.
			/// @returns x,y,z in meters.
			inline Coordinate3D geodeticToECEF(const double latitude_radians, const double longitude_radians, const double alt = 0) {
				assert(fabs(latitude_radians) <= piOver2);
				assert(fabs(longitude_radians) <= pi);
				assert(alt >= -Re(latitude_radians));
				register double n = N(latitude_radians);
				register double clat = cos(latitude_radians);
				return Coordinate3D((n + alt)*clat*cos(longitude_radians),
					(n + alt)*clat*sin(longitude_radians),
					(oneMinus_e2*n + alt)*sin(latitude_radians));
			}		

			inline Coordinate3D geodeticToECEF(const GeoCoordinate3D& d) {
				return geodeticToECEF(d.latitude.getValue(Radians), d.longitude.getValue(Radians), d.altitude.getValue(Meters));
			}

			/// convert from cartesian ECEF to spherical lat,lon,alt
			/// x,y,z in meters
			/// lat, lon in radians; alt in meters. longitude in x, latitude in y, and altitude in z
			///
			/// from http://download.springer.com/static/pdf/60/art%253A10.1007%252Fs00190-002-0273-6.pdf?auth66=1421171873_0837fd69cc5ca5ac8bdbbf0ff9741ec5&ext=.pdf /// source of a couple of good test vectors!
			inline GeoCoordinate3D ECEFToGeodetic(const double x, const double y, const double z) {
				double d2 = (x*x + y*y);
				double sqrtd2 = sqrt(d2);
				double p = d2 * oneOverA2;
				double q = oneMinus_e2 * z*z * oneOverA2;
				double r = (1.0 / 6) * (p + q - e4);
				double s = e4 * p * q / (4 * r*r*r);
				double t = pow( 1 + s + sqrt(s*(2+s)), 1.0 / 3);
				double u = r * (1 + t + 1 / t);
				double v = sqrt(u*u + e4*q);
				double w = e2 * (u + v - q) / (2 * v);
				double k = sqrt(u + v + w*w) - w;
				double D = k * sqrtd2 / (k + e2);
				double sqrtD2Z2 = sqrt(D*D + z*z);
				return GeoCoordinate3D(Latitude(2 * atan2(z, D + sqrtD2Z2), Radians),
					Longitude(2 * atan2( y, x + sqrtd2), Radians),
					(k + e2 - 1) * sqrtD2Z2 / k);
			}

			/// @param p Point3D in units of Meters
			inline GeoCoordinate3D ECEFToGeodetic(const Point3D& p) { return ECEFToGeodetic(p.x, p.y, p.z); }

			inline GeoCoordinate3D ECEFToGeodetic(const Coordinate3D& p) { return ECEFToGeodetic(p.getValue(Meters)); }

			// Great-Circle conversions adopted from
			// http://www.movable-type.co.uk/scripts/latlong.html
			// &&
			// http://www.sunearthtools.com/tools/distance.php

			/// Convenience class.  Contains the the origin of a local East-North-Up coordinate system
			/// as well as computed values for sin/cos of lats and lons of this point to avoid excessive recomputation throughout
			/// multiple transforms. The origin is a point in ECEF, and geoOrigin is the same point in Geodetic lat/lon/alt.
			class Origin : public ARL::Math::GeoCoordinate3D {
			public:

				Origin(const ARL::Math::GeoCoordinate3D& geoOrigin) : ARL::Math::GeoCoordinate3D(geoOrigin) {
					origin = geodeticToECEF(geoOrigin);
					computeSinusoidals();
				}

				Origin& filteredUpdate(const Origin& o, const double alpha = 1.0 / 4) {
					origin.filteredUpdate(o.origin, alpha);
					*this = ECEFToGeodetic(o.origin);
					computeSinusoidals();
					return *this;
				}

				double getSinLat() const { return sinLat; }
				double getSinLon() const { return sinLon; }
				double getCosLat() const { return cosLat; }
				double getCosLon() const { return cosLon; }

				const Coordinate3D& getECEFOrigin() const { return origin; }

				Coordinate3D ECEFToENU(const Coordinate3D& global) const {
					register double dx = global.getValue(Meters).x - origin.getValue(Meters).x;
					register double dy = global.getValue(Meters).y - origin.getValue(Meters).y;
					register double dz = global.getValue(Meters).z - origin.getValue(Meters).z;
					return Coordinate3D(cosLon*dy - sinLon*dx,
						cosLat*dz - sinLat*sinLon*dy - sinLat*cosLon*dx,
						sinLat*dz + cosLat*sinLon*dy + cosLat*cosLon*dx);
				}

				Coordinate3D ENUToECEF(const Coordinate3D& local) const {
					Coordinate3D rv(cosLat*cosLon*local.getValue(Meters).z - sinLat*cosLon*local.getValue(Meters).y - sinLon*local.getValue(Meters).x + origin.getValue(Meters).x,
						cosLat*sinLon*local.getValue(Meters).z - sinLat*sinLon*local.getValue(Meters).y + cosLon*local.getValue(Meters).x + origin.getValue(Meters).y,
						sinLat*local.getValue(Meters).z + cosLat*local.getValue(Meters).y + origin.getValue(Meters).z);
					return rv;
				}

				Coordinate3D geodeticToENU(const GeoCoordinate3D& global) const {
					return ECEFToENU(geodeticToECEF(global));
				}

				// Helper function for PerpDistance and PerpCoord
				Eigen::Vector3d CartPerpPoint(const GeoCoordinate3D &p1,
					const GeoCoordinate3D &p2) const {
					using vec_t = Eigen::Vector3d;

					// Temporary fix.  Use the same altitude on p1 & p2 as the origin.
					auto tmpP1 = p1, tmpP2 = p2;
					tmpP1.altitude = this->altitude;
					tmpP2.altitude = this->altitude;

					vec_t a = geodeticToENU(tmpP1).getValue(Meters);
					vec_t p2Cart = geodeticToENU(tmpP2).getValue(Meters);

					vec_t n = p2Cart - a;
					n.normalize();

					vec_t p(0, 0, 0);

					auto perpPoint = p + ((a - p) - ((a - p).dot(n) * n));

					return perpPoint;
				}

				// Helper function for PerpDistance and PerpCoord - uses GeoCoordinate3D and a direction to create normal vector
				// this angle needs to be with respect to ENU
				Eigen::Vector3d CartPerpPoint(const GeoCoordinate3D &p,
					const Orientation1D &angle) const {
					using vec_t = Eigen::Vector3d;

					// Temporary fix.  Use the same altitude on p1 & p2 as the origin.
					auto tmpP = p;
					tmpP.altitude = this->altitude;

					vec_t a = geodeticToENU(tmpP).getValue(Meters);

					vec_t n(cos(angle.getValue().yaw), sin(angle.getValue().yaw), 0);
					n.normalize();

					vec_t o(0, 0, 0);

					auto perpPoint = o + ((a - o) - ((a - o).dot(n) * n));

					return perpPoint;
				}

				///returns the unit (direction) vector to p in the local 2D cartesian coordinate system centered at this Origin
				Coordinate2D CartesianNormal(const GeoCoordinate& p) const {
					return static_cast<Coordinate2D>(geodeticToENU(p)).normalize();   
				}

				ARL::Math::Distance<double> PerpDistance(const GeoCoordinate3D &p1,
					const GeoCoordinate3D &p2, bool isSigned = false) const {

					double sign = 1.0;

					if (isSigned) {
						auto p1Cart(geodeticToENU(p1));
						auto p2Cart(geodeticToENU(p2));

						auto ax = p1Cart.getValue(ARL::Math::Meters).x;
						auto ay = p1Cart.getValue(ARL::Math::Meters).y;

						auto bx = p2Cart.getValue(ARL::Math::Meters).x;
						auto by = p2Cart.getValue(ARL::Math::Meters).y;

						sign = (ay * bx - ax * by) > 0 ? 1.0 : -1.0;
					}

					return ARL::Math::Distance<double>(CartPerpPoint(p1, p2).norm() * sign);
				}

				ARL::Math::Distance<double> PerpDistance(const GeoCoordinate3D &p1,
					const Orientation1D &angle) const {
					return ARL::Math::Distance<double>(CartPerpPoint(p1, angle).norm());
				}

				GeoCoordinate3D ENUToGeodetic(const Coordinate3D& local) const {
					return ECEFToGeodetic(ENUToECEF(local));
				}

				GeoCoordinate3D PerpCoord(const GeoCoordinate3D &p1,
					const GeoCoordinate3D &p2) const {
					auto perpPoint = CartPerpPoint(p1, p2);
					return ENUToGeodetic(ARL::Math::Coordinate3D(perpPoint.x(), perpPoint.y(), perpPoint.z()));
				}

				GeoCoordinate3D PerpCoord(const GeoCoordinate3D &p1,
					const Orientation1D &angle) const {
					auto perpPoint = CartPerpPoint(p1, angle);
					return ENUToGeodetic(ARL::Math::Coordinate3D(perpPoint.x(), perpPoint.y(), perpPoint.z()));
				}

			protected:

				void computeSinusoidals() {
					sinLat = sin(latitude.getValue(Radians));
					sinLon = sin(longitude.getValue(Radians));
					cosLat = cos(latitude.getValue(Radians));
					cosLon = cos(longitude.getValue(Radians));
				}

				ARL::Math::Coordinate3D origin;
				double sinLat;
				double sinLon;
				double cosLat;
				double cosLon;

				friend class cereal::access;

				template<class Archive>
				void save(Archive & archive) const
				{
					archive(cereal::base_class<GeoCoordinate3D>(this));
				}

				template<class Archive>
				void load(Archive & archive)
				{
					archive(cereal::base_class<GeoCoordinate3D>(this));
					this->origin = geodeticToECEF(*this);
					computeSinusoidals();
				}
			};

			using track_t = std::pair<GeoCoordinate3D, GeoCoordinate3D>;
			/**
			 * Returns two parallel tracks on either side of the track provided.
			 * Each track will be oriented <start, end>.
			 * @param limit The radius of the cross track limit.  This is the distance that will be projected on either side of the track.
			 */
			inline std::shared_ptr< std::pair< track_t, track_t > > CrossTrack
				(GeoCoordinate3D startPoint, GeoCoordinate3D endPoint, ARL::Math::Distance<> limit) {

				Origin origin(startPoint);

				auto endVector = origin.geodeticToENU(endPoint);

				Eigen::Vector3d direction(endVector.getValue(Meters).x, endVector.getValue(Meters).y, endVector.getValue(Meters).z);
				direction.normalize();

				Eigen::Matrix3d zRotation;

				auto closure = [&](const ARL::Math::Coordinate3D &offset, track_t *track) -> void {

					auto innerClosure = [&](const ARL::Math::Coordinate3D &offset, GeoCoordinate *geo) -> void {
						Eigen::Vector3d resultant = zRotation * direction;
						resultant *= limit.getValue(Meters);
						*geo = origin.ENUToGeodetic(Coordinate3D(resultant(0) + offset.getValue(Meters).x,
							resultant(1) + offset.getValue(Meters).y, resultant(2) + offset.getValue(Meters).z));
					};

					innerClosure(Coordinate3D{0, 0, 0}, &track->first);
					innerClosure(offset, &track->second);
				};
	
				auto ret = std::shared_ptr< std::pair<track_t, track_t> >(new std::pair<track_t, track_t>{});

				zRotation << // 90 degree rotation
					0, -1, 0,
					1, 0, 0,
					0, 0, 1;

				closure(endVector, &(ret->first));

				zRotation << // 270 degree rotation
					0, 1, 0,
					-1, 0, 0,
					0, 0, 1;

				closure(endVector, &(ret->second));

				return ret;
			}


			///returns a pair of tracks parallel and either side of track from point 1 to point 2 at given distance
			inline std::pair< track_t, track_t > CrossTrack2
				(GeoCoordinate startPoint, GeoCoordinate endPoint, ARL::Math::Distance<> distance) {

				Origin origin(startPoint);
				Point2D p = origin.geodeticToENU(endPoint).getValue(Meters);
				Point2D n = p; n.normalize();

				Point2D Nperp1(n.y, -n.x), Nperp2(-n.y, n.x);
				auto d = distance.getValue(Meters);

				return std::pair< track_t, track_t >(
					track_t(origin.ENUToGeodetic(Coordinate2D(Nperp1*d, Meters)), origin.ENUToGeodetic(Coordinate2D(Nperp1*d + p, Meters))),
					track_t(origin.ENUToGeodetic(Coordinate2D(Nperp2*d, Meters)), origin.ENUToGeodetic(Coordinate2D(Nperp2*d + p, Meters)))
					);
			}

			/// an object describe the transformation between the ECEF global coordinate system (WGS84) and a local East-North-Up ENU system
			/// instantiate the object with the specified GeoCoordinate as the origin of the local coordinate system.
			class Transformer {
			public:
				Transformer(const GeoCoordinate3D& origin) : mOrigin(origin) {
					setOrigin(origin);
				}

				Origin getOrigin() const {
					std::lock_guard<std::mutex> lk(mMutex);
					return mOrigin;
				}

				void setOrigin(const GeoCoordinate3D& origin) {
					mMutex.lock();
					mOrigin = origin;
					mMutex.unlock();
				}

				void setOrigin(const Origin &origin) {
					mMutex.lock();
					mOrigin = origin;
					mMutex.unlock();
				}

				/// Converts from (global) ECEF to (local) ENU cartesian coordinates given current origin.
				///
				/// @param global the ECEF cartesian coordinates
				/// @returns the corresponding local ENU cartesian coordinate given the current origin
				Coordinate3D ECEFToENU(const Coordinate3D& global) const {
					mMutex.lock();
					auto rv = mOrigin.ECEFToENU(global);
					mMutex.unlock();
					return rv;
				}

				/// Converts from (local) ENU to (global) ECEF cartesian coordinates given the current origin)
				///
				/// @param local the ENU cartesian coordinates
				/// @returns the (global) ECEF cartesian coordinates given the current origin
				Coordinate3D ENUToECEF(const Coordinate3D& local) const {
					mMutex.lock();
					auto rv = mOrigin.ENUToECEF(local);
					mMutex.unlock();
					return rv;
				}

				/// Converts from (global) Geodetic coordiantes to (local) ENU cartesian coordinates
				///
				/// @returns the local ENU cartesian coordinates
				Coordinate3D geodeticToENU(const double latitude_radians, const double longitude_radians, const double alt) const {
					return ECEFToENU(geodeticToECEF(latitude_radians, longitude_radians, alt));
				}

				/// Converts from (global) Geodetic coordiantes to (local) ENU cartesian coordinates
				///
				/// @returns the local ENU cartesian coordinates
				Coordinate3D geodeticToENU(const GeoCoordinate3D& global) const {
					return ECEFToENU(geodeticToECEF(global));
				}

				/// Converts from local ENU cartesian coordinates to (global) Geodetic coordinates given the current origin
				///
				/// @local the ENU cartesian coordinates
				/// @returns the global geodetic coordinates
				GeoCoordinate3D ENUToGeodetic(const Coordinate3D& local) const {
					return ECEFToGeodetic(ENUToECEF(local));
				}

				ARL::Math::Angle GeodeticToEgocentricBearing(const Orientation1D &orientation, const GeoCoordinate &point) const {
					return mOrigin.GeodeticToEgocentricBearing(orientation, point); //ENUToGeodetic(mOrigin.origin) //I think this was wrong...added geoOrigin to Origin to persist and avoid inverse computation...
				}

				ARL::Math::PolarCoordinate GeodeticToEgoCentric(const Orientation1D &orientation, const GeoCoordinate &point) const {
					return PolarCoordinate(GeodeticToEgocentricBearing(orientation, point), mOrigin.GreatCircleDistance(point));
				}

			protected:
				Origin mOrigin;

				mutable std::mutex mMutex;

			private:

			};

		}
	}
}

CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(ARL::Math::Geodetic::Origin, cereal::specialization::member_load_save)


