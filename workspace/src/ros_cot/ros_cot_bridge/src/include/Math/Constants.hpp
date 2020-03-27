#pragma once

#include <limits>

namespace ARL {
	namespace Math {
		const double pi = 3.14159265358979323846;
		const double piOver2 = 1.57079632679489661923;
		const double NaN = std::numeric_limits<double>::quiet_NaN();
		const int intNaN = INT_MIN; //std::numeric_limits<int>::quiet_NaN();
		const unsigned int uintNaN = UINT_MAX;//std::numeric_limits<unsigned int>::quiet_NaN();

		namespace Geodetic {

			const double Re_equator = 6378137;	 //radius of the Earth at the equator
			const double Re_polar = 6356752.3142; //radius of the Earth at the poles, in meters
			const double a = Re_equator;
			const double a2 = a*a;
			const double oneOverA2 = 1 / a2;
			const double b = Re_polar;
			const double b2 = b*b;
			const double bOverA = 0.996647189335; // axis ratio (Re_polar/Re_equator <=> b/a)
			const double Cbar_2_0 = -0.484166774958e-3; // second degree Zonal harmonic
			const double E = 5.2185400842339e5; // linear eccentricity of the earth
			const double e = 0.081819190842622; // first eccentricity of the earth
			const double e2 = 0.00669437999014; // first eccentricity of the earth squared (meters^2)
			const double e4 = e2 * e2; //first eccentricity of the earth to the fourth power (meters^4)
			const double oneMinus_e2 = 1 - e2;
			const double eprime = 8.2094437949696e-2;	//second eccentriciy of the earth
			const double eprime2 = 6.73949674228e-3;	//second eccentricity of the earth squared
			const double Se = 5.10065621724e14; // surface area, in square meters
			const double Ve = 1.08320731980e21; // volume, in cubic meters
			const double Me = 5.9733328e24; // mass of the earth (including atmosphere), in kg
			const double R1 = 6371008.7714; // Mean radius of Semi-axes in meters
			const double R2 = 6371007.1809; // Radius of sphere of equal area in meters
			const double R3 = 6371000.7900; // Radius of sphere of equal volume in meters
			const double wie = 7.292115e-5; // Earth's angular rate (in radians/second)
			const double Rp = 6356752.31425; //polar radius
			const double u = 3.986004418e14; //Earth's Gravitational Constant
			const double f = 1 / 298.257223563; //ellipsoid flattening constant

		}

		
		inline int round(const double x) {
//			return static_cast<int>(x + (x >= 0 ? 0.5 : -0.5));
			return static_cast<int>(::round(x));
		}

		template <typename T = double>
		T radiansToDegrees(const T& r) {
			return r * 180.0 / pi;
		}

		template <typename T = double>
		T degreesToRadians(const T& d) {
			return d * pi / 180.0;
		}

		template <typename T = double>
		T metersToYards(const T& x) {
			return x * 1.0936; // 1.0936 yds/m
		}

		template <typename T = double>
		T yardsToMeters(const T& x) {
			return x / 1.0936; // 1.0936 yds/m
		}

		template <typename T = double>
		T metersToFeet(const T& x) {
			return x * 3.2808; // 3.2808 ft/m
		}

		template <typename T = double>
		T feetToMeters(const T& x) {
			return x / 3.2808; // 3.2808 ft/m
		}

		template <typename T = double>
		T nauticalMilesToMeters(const T& x) {
			return 1852.0 * x; // 1852.0 m/nm
		}

		template <typename T = double>
		T metersToNauticalMiles(const T& x) {
			return 5.399568e-4 * x; // 1/1852.0 nm/m
		}

		template <typename T = double>
		T milesToMeters(const T& x) {
			return 1609.344 * x; //1690.344 m/mi
		}

		template <typename T = double>
		T metersToMiles(const T& x) {
			return 6.21371192237e-4 * x; // 1/1690.344 mi/m
		}

		template <typename T = double>
		T halfWaveRectify(const T& x) {
			return x > 0 ? x : 0;
		}

		template <typename T = double>
		T celsiusToKelvin(const T& t) {
			return t + 273.15;
		}

		template <typename T = double>
		T kelvinToCelsius(const T& t) {
			return t - 273.15;
		}
		template <typename T = double>
		T celsiusToFahrenheit(const T& t) {
			return t * 9.0 / 5 + 32;
		}

		template <typename T = double>
		T fahrenheitToCelsius(const T& t) {
			return (t - 32) * 5.0 / 9;
		}

		template <typename T = double>
		T joulesToKiloWattHours(const T& t) {
			return t / 3.6e6;
		}

		template <typename T = double>
		T kiloWattHoursToJoules(const T& t) {
			return t * 3.6e6;
		}

		template <typename T = double>
		T joulesToWattHours(const T& t) {
			return t / 3.6e3;
		}

		template <typename T = double>
		T wattHoursToJoules(const T& t) {
			return t * 3.6e3;
		}

		template <typename T = double>
		T pascalsToPSI(const T& v) {
			return v / 0.000145037738;
		}

		template <typename T = double>
		T psiToPascals(const T& v) {
			return v * 0.000145037738;
		}

	}
}