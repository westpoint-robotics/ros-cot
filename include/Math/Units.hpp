#pragma once

#include <string>
#include <boost/algorithm/string.hpp>
#include "../Utility/StringConversions.hpp"
#include "../Utility/Exception.hpp"

namespace ARL {
	namespace Math {

		class UnitHelper
		{
		public:
			using string = ::std::string;
			using wstring = ::std::wstring;

		protected:
				int iUnit;

				static bool ValidUnitRange(int iUnit, int nUnits) {
					return((iUnit >= 0)&&(iUnit < nUnits));
				}

				int ToUnits(const string &str, const char *(*ToString)(int iUnit), int nUnits)
				{
					string str1LC = str;
					boost::algorithm::to_lower(str1LC);

					for (int iUnit = 0; iUnit < nUnits; iUnit++)
					{
						string str2LC = ToString(iUnit);
						boost::algorithm::to_lower(str2LC);
						if (str1LC == str2LC)
							return(iUnit);
					}
					return(nUnits);
				}

				UnitHelper()
					: iUnit{0} {}
		};

		enum SpeedUnits {
			MetersPerSecond, Knots, FeetPerSecond, MilesPerHour, KilometersPerHour
		};
#define nSpeedUnits 5

		class SpeedUnitHelper : public UnitHelper
		{
			public:
				static const char *ToString(int iUnit)
				{
					static const char *units[nSpeedUnits] = { "metersPerSecond", "knots", "feetPerSecond", "milesPerHour", "kilometersPerHour" };

					if (!ValidUnitRange(iUnit, nSpeedUnits))
						throw Exception("Invalid units range");
					return(units[iUnit]);
				}

				static wstring ToStringW(int iUnit)
				{
					return(Utility::quickS2W(string(ToString(iUnit))));
				}

				operator const char *() { return(ToString(iUnit)); }
				operator int()          { return(iUnit); }

				SpeedUnitHelper(int iUnitIn)            {	iUnit = iUnitIn; }
				SpeedUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nSpeedUnits); }
				SpeedUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nSpeedUnits); }
		};
#undef nSpeedUnits

		enum AccelerationUnits {
			MetersPerSecondSquared, FeetPerSecondSquared
		};
#define nAccelerationUnits 2

		class AccelerationUnitHelper : public UnitHelper
		{
		public:
			static const char *ToString(int iUnit)
			{
				static const char *units[nAccelerationUnits] = { "metersPerSecondSquared", "feetPerSecondSquared" };

				if (!ValidUnitRange(iUnit, nAccelerationUnits))
					throw Exception("Invalid units range");
				return(units[iUnit]);
			}

			static wstring ToStringW(int iUnit)
			{
				return(Utility::quickS2W(string(ToString(iUnit))));
			}

			operator const char *() { return(ToString(iUnit)); }
			operator int()          { return(iUnit); }

			AccelerationUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
			AccelerationUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nAccelerationUnits); }
			AccelerationUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nAccelerationUnits); }
		};
#undef nAngleUnits

		enum LengthUnits {
			Meters, NauticalMiles, Miles, Kilometers, Feet, Yards
		};
#define nLengthUnits 6

		class LengthUnitHelper : public UnitHelper
		{
			public:
				static const char *ToString(int iUnit)
				{
					static const char *units[nLengthUnits] = { "meters", "nauticalMiles", "miles", "kilometers", "feet", "yards" };

					if (!ValidUnitRange(iUnit, nLengthUnits))
						throw Exception("Invalid units range");
					return(units[iUnit]);
				}

				static wstring ToStringW(int iUnit)
				{
					return(Utility::quickS2W(string(ToString(iUnit))));
				}

				operator const char *() { return ToString(iUnit); }
				operator int() { return iUnit; }
				operator LengthUnits() { return LengthUnits(iUnit); }

				LengthUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
				LengthUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nLengthUnits); }
				LengthUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nLengthUnits); }
		};
#undef nLengthUnits

		enum AngleUnits {
			Radians, Degrees
		};
#define nAngleUnits 2

		class AngleUnitHelper : public UnitHelper
		{
			public:
				static const char *ToString(int iUnit)
				{
					static const char *units[nAngleUnits] = { "radians", "degrees" };

					if (!ValidUnitRange(iUnit, nAngleUnits))
						throw Exception("Invalid units range");
					return(units[iUnit]);
				}

				static wstring ToStringW(int iUnit)
				{
					return(Utility::quickS2W(ToString(iUnit)));
				}

				operator const char *() { return(ToString(iUnit)); }
				operator int()          { return(iUnit); }

				AngleUnitHelper(int iUnitIn)             { iUnit = iUnitIn; }
				AngleUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nAngleUnits); }
				AngleUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nAngleUnits); }

		};
#undef nSpeedUnits

		enum AngularSpeedUnits {
			RadiansPerSecond, DegreesPerSecond
		};
#define nAngularSpeedUnits 2

		class AngularSpeedUnitHelper : public UnitHelper
		{
			public:
				static const char *ToString(int iUnit)
				{
					static const char *units[nAngularSpeedUnits] = { "radiansPerSecond", "degreesPerSecond" };

					if (!ValidUnitRange(iUnit, nAngularSpeedUnits))
						throw Exception("Invalid units range");
					return(units[iUnit]);
				}

				static wstring ToStringW(int iUnit)
				{
					return(Utility::quickS2W(string(ToString(iUnit))));
				}

				operator const char *() { return(ToString(iUnit)); }
				operator int()          { return(iUnit); }

				AngularSpeedUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
				AngularSpeedUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nAngularSpeedUnits); }
				AngularSpeedUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nAngularSpeedUnits); }
		};
#undef nAccelerationUnits

		enum FrequencyUnits {
			Hertz, KiloHertz, MegaHertz, GigaHertz, TeraHertz
		};
#define nFrequencyUnits 5

		class FrequencyUnitHelper : public UnitHelper
		{
		public:
			static const char *ToString(int iUnit)
			{
				static const char *units[nFrequencyUnits] = { "hertz", "kilohertz", "megahertz", "gigahertz", "terahertz" };

				if (!ValidUnitRange(iUnit, nFrequencyUnits))
					throw Exception("Invalid units range");
				return(units[iUnit]);
			}

			static wstring ToStringW(int iUnit)
			{
				return(Utility::quickS2W(string(ToString(iUnit))));
			}

			operator const char *() { return(ToString(iUnit)); }
			operator int()          { return(iUnit); }

			FrequencyUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
			FrequencyUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nFrequencyUnits); }
			FrequencyUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nFrequencyUnits); }
		};
#undef nFrequencyUnits

		enum TemperatureUnits {
			Celsius, Kelvin, Fahrenheit
		};
#define nTemperatureUnits 3

		class TemperatureUnitHelper : public UnitHelper
		{
		public:
			static const char *ToString(int iUnit)
			{
				static const char *units[nTemperatureUnits] = { "celsius", "kelvin", "fahrenheit" };

				if (!ValidUnitRange(iUnit, nTemperatureUnits))
					throw Exception("Invalid units range");
				return(units[iUnit]);
			}

			static wstring ToStringW(int iUnit)
			{
				return(Utility::quickS2W(string(ToString(iUnit))));
			}

			operator const char *() { return(ToString(iUnit)); }
			operator int()          { return(iUnit); }

			TemperatureUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
			TemperatureUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nTemperatureUnits); }
			TemperatureUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nTemperatureUnits); }
		};
#undef nTemperatureUnits

		enum VoltageUnits {
			Volts
		};
#define nVoltageUnits 1

		class VoltageUnitHelper : public UnitHelper
		{
		public:
			static const char *ToString(int iUnit)
			{
				static const char *units[nVoltageUnits] = { "volts" };

				if (!ValidUnitRange(iUnit, nVoltageUnits))
					throw Exception("Invalid units range");
				return(units[iUnit]);
			}

			static wstring ToStringW(int iUnit)
			{
				return(Utility::quickS2W(string(ToString(iUnit))));
			}

			operator const char *() { return(ToString(iUnit)); }
			operator int()          { return(iUnit); }

			VoltageUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
			VoltageUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nVoltageUnits); }
			VoltageUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nVoltageUnits); }
		};
#undef nVoltageUnits

		enum CurrentUnits {
			Amps, milliAmps
		};
#define nCurrentUnits 2

		class CurrentUnitHelper : public UnitHelper
		{
		public:
			static const char *ToString(int iUnit)
			{
				static const char *units[nCurrentUnits] = { "amps", "milliamps" };

				if (!ValidUnitRange(iUnit, nCurrentUnits))
					throw Exception("Invalid units range");
				return(units[iUnit]);
			}

			static wstring ToStringW(int iUnit)
			{
				return(Utility::quickS2W(string(ToString(iUnit))));
			}

			operator const char *() { return(ToString(iUnit)); }
			operator int()          { return(iUnit); }

			CurrentUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
			CurrentUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nCurrentUnits); }
			CurrentUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nCurrentUnits); }
		};
#undef nCurrentUnits

		enum WorkUnits {
			Joules, KiloWattHours, WattHours
		};
#define nWorkUnits 3

		class WorkUnitHelper : public UnitHelper
		{
		public:
			static const char *ToString(int iUnit)
			{
				static const char *units[nWorkUnits] = { "joules", "kilowattHours", "wattHours" };

				if (!ValidUnitRange(iUnit, nWorkUnits))
					throw Exception("Invalid units range");
				return(units[iUnit]);
			}

			static wstring ToStringW(int iUnit)
			{
				return(Utility::quickS2W(string(ToString(iUnit))));
			}

			operator const char *() { return(ToString(iUnit)); }
			operator int()          { return(iUnit); }

			WorkUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
			WorkUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nWorkUnits); }
			WorkUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nWorkUnits); }
		};
#undef nWorkUnits

		enum PowerUnits {
			Watts, KiloWatts
		};
#define nPowerUnits 2

		class PowerUnitHelper : public UnitHelper
		{
		public:
			static const char *ToString(int iUnit)
			{
				static const char *units[nPowerUnits] = { "watts", "kilowatts" };

				if (!ValidUnitRange(iUnit, nPowerUnits))
					throw Exception("Invalid units range");
				return(units[iUnit]);
			}

			static wstring ToStringW(int iUnit)
			{
				return(Utility::quickS2W(string(ToString(iUnit))));
			}

			operator const char *() { return(ToString(iUnit)); }
			operator int()          { return(iUnit); }

			PowerUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
			PowerUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nPowerUnits); }
			PowerUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nPowerUnits); }
		};
#undef nPowerUnits

		enum PressureUnits {
			Pascals, psi
		};
#define nPressureUnits 2

		class PressureUnitHelper : public UnitHelper
		{
		public:
			static const char *ToString(int iUnit)
			{
				static const char *units[nPressureUnits] = { "pascals", "psi" };

				if (!ValidUnitRange(iUnit, nPressureUnits))
					throw Exception("Invalid units range");
				return(units[iUnit]);
			}

			static wstring ToStringW(int iUnit)
			{
				return(Utility::quickS2W(string(ToString(iUnit))));
			}

			operator const char *() { return(ToString(iUnit)); }
			operator int()          { return(iUnit); }

			PressureUnitHelper(int iUnitIn)            { iUnit = iUnitIn; }
			PressureUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nPressureUnits); }
			PressureUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nPressureUnits); }
		};
#undef nPressureUnits

		enum AltitudeReferenceUnits {
			WGS84, EarthsLandSeaSurface, EarthsTopographicalBathymetricSurface
		};
#define nAltitudeReferenceUnits 3

		class AltitudeReferenceUnitHelper : public UnitHelper
		{
		public:
			static const char *ToString(int iUnit)
			{
				static const char *units[nAltitudeReferenceUnits] = { "WGS84 Geoid", "Earth's Land/Sea Surface", "Earth's Topographical/Bathymetric Surface" };

				if (!ValidUnitRange(iUnit, nAltitudeReferenceUnits))
					throw Exception("Invalid units range");
				return(units[iUnit]);
			}

			static wstring ToStringW(int iUnit)
			{
				return(Utility::quickS2W(string(ToString(iUnit))));
			}

			operator const char *() { return(ToString(iUnit)); }
			operator int() { return(iUnit); }

			AltitudeReferenceUnitHelper(int iUnitIn) { iUnit = iUnitIn; }
			AltitudeReferenceUnitHelper(const string &str) { iUnit = ToUnits(str, ToString, nAltitudeReferenceUnits); }
			AltitudeReferenceUnitHelper(const wstring &str){ iUnit = ToUnits(Utility::quickW28(str), ToString, nAltitudeReferenceUnits); }
		};
#undef nAltitudeReferenceUnits

	}
}