#pragma once

// If you include this file you must link against the library created by the geotrans project.

#include <Math/Geodetics.hpp>
#include <Messaging/macro.h>
#include "geotrans/UTM.h"
#include "geotrans/GeodeticCoordinates.h"
#include "geotrans/UTMCoordinates.h"

namespace ARL {

	namespace Utility {

		class UTMConverter {
		public:			
			
			UTMConverter() {
				mHemisphere = 'N';
				mZone = 18;
				mEasting = 258204.8;
				mNorthing = 4519694.8;
			}

			/**
			 * Converts a relative UTM Easting/Northing pair to Lat/Long
			 */
			ARL::Math::GeoCoordinate UTMToGeodetic(double easting, double northing) const {
				MSP::CCS::UTMCoordinates coord(MSP::CCS::CoordinateType::geodetic, mZone, mHemisphere,
					mEasting + easting, mNorthing + northing);

				MSP::CCS::UTM converter;
				auto conversion = converter.convertToGeodetic(&coord);

				ARL::Math::GeoCoordinate ret(conversion->latitude() * 180 / M_PI,
					conversion->longitude() * 180 / M_PI);

				delete conversion;

				return ret;
			}

			void setReferencePoint(char hemisphere, unsigned zone, double easting, double northing) {
				mHemisphere = hemisphere;
				mZone = zone;
				mEasting = easting;
				mNorthing = northing;
			}

		private:
			char mHemisphere;
			unsigned mZone;
			double mEasting, mNorthing;
		};

	}

}