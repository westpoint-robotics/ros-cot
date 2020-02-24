#pragma once
#include <xercesc/dom/DOM.hpp>
#include "xstr.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include "Exception.hpp"
#include "../Math/Constants.hpp"
#include "../Math/Units.hpp"
#include "StringSwitch.hpp"
#include <chrono>
#include <string>
#include <sstream>


	namespace Utility {

		using boost::posix_time::ptime;
		using boost::posix_time::time_duration;
		using boost::posix_time::not_a_date_time;
		using namespace std;
		using namespace ARL::Math;
		using Utility::StringSwitchSet;
		using Utility::WStringSwitchSet;
		using Utility::quickW28;

	namespace DOMHelper {
//		public:
		using DOMDocument = XERCES_CPP_NAMESPACE::DOMDocument;
		using DOMElement = XERCES_CPP_NAMESPACE::DOMElement;
		using DOMAttr = XERCES_CPP_NAMESPACE::DOMAttr;
		using xStr = Utility::xStr;

		//************************************************************************************************************
		//	helper functions

		template <class T>
		static wstring toISOwString(T v) {
			if (std::isnan(v)) {
				return L"NaN";
			}
			else if (std::isinf(v)) {
				return L"INF";
			}
			else {
				wstringstream ss;
				ss << setprecision(14) << v;
				return ss.str();
			}
		}

		template <class T>
		static T fromISOwString(const wstring& isoString) {
			T value;
			if (!WStringSwitchSet(isoString, &value,
				L"NaN", numeric_limits<T>::quiet_NaN(),
				L"INF", numeric_limits<T>::infinity())) //if a special case if found, value will be set appropriately
			{
				wstringstream ss(isoString); //else, normal value, let stringstream deserialize it
				ss >> value;
				if (ss.fail()) throw Exception("error parsing isoString in DOMHelper::fromISOwString");
			}
			return value;
		}

		//************************************************************************************************************
		//	create DOMElement* for value types. TextNodeContent carries the data.

		template <class T>
		static DOMElement* createDOMElement(const T& value, DOMDocument* pDoc, const XMLCh* name = xStr(L"Element")) {
			wstringstream ss;
			ss << value;
			DOMElement* pE = pDoc->createElement(name);
			pE->setTextContent(xStr(ss.str().c_str()));
			return pE;
		}

		template <> //force specific handling of conversion from double to ensure NaN, INF and precision sufficient
		static DOMElement* createDOMElement<double>(const double& value, DOMDocument* pDoc, const XMLCh* name) {
			DOMElement* pE = pDoc->createElement(name);
			pE->setTextContent(xStr(toISOwString(value)));
			return pE;
		}

		static DOMElement* createDOMElement(const ptime& value, DOMDocument* pDoc, const XMLCh* name = xStr(L"time")) {
			wstringstream ss;
			string s = boost::posix_time::to_iso_extended_string(value);
			ss << wstring(s.begin(), s.end());
			XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
			pElement->setTextContent(xStr(ss.str().c_str()));
			return pElement;
		}

		static DOMElement* createDOMElement(const time_duration& duration, DOMDocument* pDoc, const XMLCh* name = xStr(L"Duration")) {
			auto pElement = pDoc->createElement(name);
			stringstream ss;
			if (!duration.is_special()) { ///@todo handle special values? not sure if they are ISO 8601 compliant...
				auto seconds = duration.seconds() + duration.total_microseconds() * 1.0e-6 - duration.total_seconds(); //compute seconds
				ss << (duration.is_negative() ? "-" : "") << "PT";
				if (duration.hours() > 0) ss << duration.hours() << 'H';
				if (duration.minutes() > 0) ss << duration.minutes() << 'M';
				if (seconds > 0 || (duration.hours() == 0 && duration.minutes() == 0)) ss << seconds << 'S';
				pElement->setTextContent(xStr(ss.str().c_str()));
			}
			return pElement;
		}

		//************************************************************************************************************
		//	set attributes onto DOMElement* from value types

		template <class T>
		static DOMElement* setDOMAttribute(const T& value, DOMElement* pElement, const XMLCh* name = xStr(L"Attribute")) {
			wstringstream ss;
			ss << value;
			pElement->setAttribute(name, xStr(ss.str().c_str()));
			return pElement;
		}

		static DOMElement* setDOMAttribute(const std::string& value, DOMElement* pElement, const XMLCh* name = xStr(L"StringAttribute")) {
			pElement->setAttribute(name, xStr(value.c_str()));
			return pElement;
		}

		static DOMElement* setDOMAttribute(const ptime& value, DOMElement *pElement, const XMLCh* name = xStr(L"Time")) {
			if (!value.is_not_a_date_time()) {
				wstringstream ss;
				string s = boost::posix_time::to_iso_extended_string(value);
				ss << wstring(s.begin(), s.end());
				pElement->setAttribute(name, xStr(ss.str().c_str()));
			}
			return pElement;
		}

		static DOMElement* setDOMAttribute(const time_duration& duration, DOMElement* pElement, const XMLCh* name = xStr(L"Duration")) {
			stringstream ss;
			if (!duration.is_special()) { ///@todo handle special values? not sure if they are ISO 8601 compliant...
				auto seconds = duration.seconds() + duration.total_microseconds() * 1.0e-6 - duration.total_seconds(); //compute seconds
				ss << (duration.is_negative() ? "-" : "") << "PT";
				if (duration.hours() > 0) ss << duration.hours() << 'H';
				if (duration.minutes() > 0) ss << duration.minutes() << 'M';
				if (seconds > 0) ss << seconds << 'S';
				pElement->setAttribute(name, xStr(ss.str().c_str()));
			}
			return pElement;
		}

//		static DOMElement* setDOMAttribute(const std::chrono::high_resolution_clock::time_point& time, DOMElement *pElement, const XMLCh* name = xStr(L"Time")) {
//			return setDOMAttribute(boost::posix_time::from_time_t(std::chrono::high_resolution_clock::to_time_t(time)), pElement, name);
//		}

		static DOMElement* setDOMAttribute(const SpeedUnits& units, DOMElement* pElement, const XMLCh* name) {
			switch (units) {
			case MetersPerSecond: pElement->setAttribute(name, xStr(L"MetersPerSecond")); break;
			case Knots: pElement->setAttribute(name, xStr(L"Knots")); break;
			case FeetPerSecond: pElement->setAttribute(name, xStr(L"FeetPerSecond")); break;
			case MilesPerHour: pElement->setAttribute(name, xStr(L"MilesPerHour")); break;
			case KilometersPerHour: pElement->setAttribute(name, xStr(L"KilometersPerHour")); break;
			default: throw Exception("Unknown SpeedUnits in DOMHelper::setDOMAttribute()");
			}
			return pElement;
		}

		static DOMElement* setDOMAttribute(const AccelerationUnits& units, DOMElement* pElement, const XMLCh* name) {
			switch (units) {
			case MetersPerSecondSquared: pElement->setAttribute(name, xStr(L"MetersPerSecondSquared")); break;
			case FeetPerSecondSquared: pElement->setAttribute(name, xStr(L"FeetPerSecondSquared")); break;
			default: throw Exception("Unknown AccelerationUnits in DOMHelper::setDOMAttribute()");
			}
			return pElement;
		}

		static DOMElement* setDOMAttribute(const LengthUnits& units, DOMElement* pElement, const XMLCh* name) {
			switch (units) {
			case Meters: pElement->setAttribute(name, xStr(L"Meters")); break;
			case NauticalMiles: pElement->setAttribute(name, xStr(L"NauticalMiles")); break;
			case Miles: pElement->setAttribute(name, xStr(L"Miles")); break;
			case Kilometers: pElement->setAttribute(name, xStr(L"Kilometers")); break;
			case Feet: pElement->setAttribute(name, xStr(L"Feet")); break;
			case Yards: pElement->setAttribute(name, xStr(L"Yards")); break;
			default: throw Exception("Unknown Length Units in DOMHelper::setDOMAttribute()");
			}
			return pElement;
		}

		static DOMElement* setDOMAttribute(const AngularSpeedUnits& units, DOMElement* pElement, const XMLCh* name) {
			switch (units) {
			case RadiansPerSecond: pElement->setAttribute(name, xStr(L"RadiansPerSecond")); break;
			case DegreesPerSecond: pElement->setAttribute(name, xStr(L"DegreesPerSecond")); break;
			default: throw Exception("Unknown AngularVelocity units in DOMHelper::setDOMAttribute()");
			}
			return pElement;
		}

		static DOMElement* setDOMAttribute(const AngleUnits& units, DOMElement* pElement, const XMLCh* name) {
			switch (units) {
			case Radians: pElement->setAttribute(name, xStr(L"Radians")); break;
			case Degrees: pElement->setAttribute(name, xStr(L"Degrees")); break;
			default: throw Exception("Unknown AngularVelocity units in DOMHelper::setDOMAttribute()");
			}
			return pElement;
		}

		static DOMElement* setDOMAttribute(const TemperatureUnits& units, DOMElement* pElement, const XMLCh* name) {
			switch (units) {
			case Celsius: pElement->setAttribute(name, xStr(L"Celsius")); break;
			case Kelvin: pElement->setAttribute(name, xStr(L"Kelvin")); break;
			case Fahrenheit: pElement->setAttribute(name, xStr(L"Fahrenheit")); break;
			default: throw Exception("Unknown TemperatureUnits units in DOMHelper::setDOMAttribute()");
			}
			return pElement;
		}

		static DOMElement* setDOMAttribute(const AltitudeReferenceUnits& units, DOMElement* pElement, const XMLCh* name) {
			switch (units) {
			case WGS84: pElement->setAttribute(name, xStr(L"WGS84 Geoid")); break;
			case EarthsLandSeaSurface: pElement->setAttribute(name, xStr(L"Earth's Land/Sea Surface")); break;
			case EarthsTopographicalBathymetricSurface: pElement->setAttribute(name, xStr(L"Earth's Topographical/Bathymetric Surface")); break;
			default: throw Exception("Unknown TemperatureUnits units in DOMHelper::setDOMAttribute()");
			}
			return pElement;
		}

		static DOMElement* setDOMAttribute(bool value, DOMElement* pElement, const XMLCh* name) {
			auto boolToString = [](bool value) -> const char* { return value ? "true" : "false"; };
			pElement->setAttribute(name, xStr(boolToString(value)));
			return pElement;
		}

		//************************************************************************************************************
		//	parse DOMElement* into value types

		template <class T>
		static T parseDOMElement(const DOMElement* pElement) {
			wistringstream ss(pElement->getTextContent());
			T value;
			ss >> value;
			if (ss.fail()) throw Exception("error parsing DOMElement");
			return value;
		}

		template <>
		static double parseDOMElement<double>(const DOMElement* pElement) {
			return fromISOwString<double>(wstring(aStr(pElement->getTextContent())));
		}

		template <>
		static ptime parseDOMElement<ptime>(const DOMElement* pElement) {
			if (pElement == nullptr)
				return ptime(not_a_date_time);
			else 
				return boost::date_time::parse_delimited_time<ptime>(quickW28(wstring(aStr(pElement->getTextContent()))), 'T');
		}

		template <>
		static time_duration parseDOMElement<time_duration>(const DOMElement* pElement) {
			auto s = quickW28(wstring(aStr(pElement->getTextContent())));
			if (s.length() == 0) return boost::posix_time::not_a_date_time; ///@todo or throw Exception here?
			stringstream ss(s);
			char c;
			ss >> c;
			bool isNegative = c == '-';
			if (isNegative) ss >> c; //look for the 'P'
			if (c != 'P') throw Exception("Invalid ISO 8601 format for duration in DOMHelper::parseDOMAttribute<time_duration>");
			uint64_t total_microseconds = 0;
			ss >> c; //consume the 'T'
			if (c != 'T') throw Exception("Invalid ISO 8601 format for duration in DOMHelper::parseDOMAttribute<time_duration>");
			do {
				double v;
				ss >> v >> c; // extract value followed by ISO designator for value's weight in the duration
				if (ss.fail()) throw Exception("Invalid ISO 8601 format for duration in DOMHelper::parseDOMAttribute<time_duration>");
				switch (c) {
				case 'H': total_microseconds += static_cast<uint64_t>(v * 3.6e9); break;
				case 'M': total_microseconds += static_cast<uint64_t>(v * 6.0e7); break;
				case 'S': total_microseconds += static_cast<uint64_t>(v * 1.0e6); break;
				default: throw Exception("Invalid or unsupported ISO 8601 format for duration in DOMHelper::parseDOMAttribute<time_duration>");
				}
			} while (ss.tellg() < (int)s.length());
			time_duration duration = boost::posix_time::microseconds(total_microseconds);
			if (isNegative) duration = duration.invert_sign();
			return duration;
		}

		//************************************************************************************************************
		//	parse named attributes of DOMElement* into value types

		template <class T>
		static T parseDOMAttribute(const DOMElement* pElement, const XMLCh* name) {
			wistringstream ss(wstring(aStr(pElement->getAttribute(name))));
			T value;
			ss >> value;
			if (ss.fail()) throw Exception("error parsing DOMAttribute");
			return value;
		}

		template <>
		static ptime parseDOMAttribute<ptime>(const DOMElement* pElement, const XMLCh* name) {
			if (!pElement->hasAttribute(name)) return boost::posix_time::ptime(boost::date_time::special_values::not_a_date_time);
			boost::posix_time::ptime time;
			auto s = quickW28(wstring(aStr(pElement->getAttribute(name))));
			try {
				time = boost::date_time::parse_delimited_time<boost::posix_time::ptime>(s, 'T');
			}
			catch (...) {
				if (s == "not-a-date-time" || s == "") time = boost::posix_time::ptime(boost::date_time::special_values::not_a_date_time);
				else if (s == "-infinity") time = boost::posix_time::ptime(boost::date_time::special_values::neg_infin);
				else if (s == "+infinity") time = boost::posix_time::ptime(boost::date_time::special_values::pos_infin);
				else throw ARL::Exception("Unable to deserialize ptime in void load<Archive>(...)");
			}
			return time;
		}

		template <>
		static time_duration parseDOMAttribute<time_duration>(const DOMElement* pElement, const XMLCh* name) {
			auto s = quickW28(wstring(aStr(pElement->getAttribute(name))));
			if (s.length() == 0) return boost::posix_time::not_a_date_time; ///@todo or throw Exception here?
			stringstream ss(s);
			char c;
			ss >> c;
			bool isNegative = c == '-';
			if (isNegative) ss >> c; //look for the 'P'
			if (c != 'P') throw Exception("Invalid ISO 8601 format for duration in DOMHelper::parseDOMAttribute<time_duration>");
			uint64_t total_microseconds = 0;
			ss >> c; //consume the 'T'
			if (c != 'T') throw Exception("Invalid ISO 8601 format for duration in DOMHelper::parseDOMAttribute<time_duration>");
			do {
				double v;
				ss >> v >> c; // extract value followed by ISO designator for value's weight in the duration
				if (ss.fail()) throw Exception("Invalid ISO 8601 format for duration in DOMHelper::parseDOMAttribute<time_duration>");
				switch (c) {
				case 'H': total_microseconds += static_cast<uint64_t>(v * 3.6e9); break;
				case 'M': total_microseconds += static_cast<uint64_t>(v * 6.0e7); break;
				case 'S': total_microseconds += static_cast<uint64_t>(v * 1.0e6); break;
				default: throw Exception("Invalid or unsupported ISO 8601 format for duration in DOMHelper::parseDOMAttribute<time_duration>");
				}
			} while (ss.tellg() < (int)s.length());
			time_duration duration = boost::posix_time::microseconds(total_microseconds);
			if (isNegative) duration = duration.invert_sign();
			return duration;
		}

//		template <>
//		static std::chrono::high_resolution_clock::time_point parseDOMAttribute<std::chrono::high_resolution_clock::time_point>(const DOMElement* pElement, const XMLCh* name) {
//			return std::chrono::high_resolution_clock::from_time_t(boost::posix_time::to_time_t(parseDOMAttribute<ptime>(pElement, name)));
//		}

		template <>
		static SpeedUnits parseDOMAttribute<SpeedUnits>(const DOMElement* pElement, const XMLCh* name) {
			SpeedUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"MetersPerSecond", MetersPerSecond,
				L"Knots", Knots,
				L"FeetPerSecond", FeetPerSecond,
				L"MilesPerHour", MilesPerHour,
				L"KilometersPerHour", KilometersPerHour))
				throw Exception("Unknown SpeedUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static AccelerationUnits parseDOMAttribute<AccelerationUnits>(const DOMElement* pElement, const XMLCh* name) {
			AccelerationUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"MetersPerSecondSquared", MetersPerSecondSquared,
				L"FeetPerSecondSquared", FeetPerSecondSquared))
				throw Exception("Unknown AccelerationUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static LengthUnits parseDOMAttribute<LengthUnits>(const DOMElement* pElement, const XMLCh* name) {
			LengthUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"Meters", Meters,
				L"NauticalMiles", NauticalMiles,
				L"Miles", Miles,
				L"Kilometers", Kilometers,
				L"Feet", Feet,
				L"Yards", Yards))
				throw Exception("Unknown LengthUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static AngularSpeedUnits parseDOMAttribute<AngularSpeedUnits>(const DOMElement* pElement, const XMLCh* name) {
			AngularSpeedUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"RadiansPerSecond", RadiansPerSecond,
				L"DegreesPerSecond", DegreesPerSecond))
				throw Exception("Unknown AngularSpeedUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static AngleUnits parseDOMAttribute<AngleUnits>(const DOMElement* pElement, const XMLCh* name) {
			AngleUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"Radians", Radians,
				L"Degrees", Degrees))
				throw Exception("Unknown AngleUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static FrequencyUnits parseDOMAttribute<FrequencyUnits>(const DOMElement* pElement, const XMLCh* name) {
			FrequencyUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"Hertz", Hertz,
				L"KiloHertz", KiloHertz,
				L"MegaHertz", MegaHertz,
				L"GigaHertz", GigaHertz,
				L"TeraHertz", TeraHertz))
				throw Exception("Unknown FrequencyUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static TemperatureUnits parseDOMAttribute<TemperatureUnits>(const DOMElement* pElement, const XMLCh* name) {
			TemperatureUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"Celsius", Celsius,
				L"Kelvin", Kelvin,
				L"Fahrenheit", Fahrenheit))
				throw Exception("Unknown TemperatureUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static VoltageUnits parseDOMAttribute<VoltageUnits>(const DOMElement* pElement, const XMLCh* name) {
			VoltageUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"Volts", Volts))
				throw Exception("Unknown VoltageUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static CurrentUnits parseDOMAttribute<CurrentUnits>(const DOMElement* pElement, const XMLCh* name) {
			CurrentUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"Amps", Amps,
				L"milliAmps", milliAmps))
				throw Exception("Unknown CurrentUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static WorkUnits parseDOMAttribute<WorkUnits>(const DOMElement* pElement, const XMLCh* name) {
			WorkUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"Joules", Joules, L"KiloWattHours", KiloWattHours))
				throw Exception("Unknown WorkUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static PowerUnits parseDOMAttribute<PowerUnits>(const DOMElement* pElement, const XMLCh* name) {
			PowerUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"Watts", Watts,
				L"KiloWatts", KiloWatts))
				throw Exception("Unknown PowerUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static PressureUnits parseDOMAttribute<PressureUnits>(const DOMElement* pElement, const XMLCh* name) {
			PressureUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"Pascals", Pascals,
				L"psi", psi))
				throw Exception("Unknown PressureUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static AltitudeReferenceUnits parseDOMAttribute<AltitudeReferenceUnits>(const DOMElement* pElement, const XMLCh* name) {
			AltitudeReferenceUnits units;
			if (!WStringSwitchSet(wstring(aStr(pElement->getAttribute(name))), &units,
				L"WGS84 Geoid", WGS84,
				L"Earth's Land/Sea Surface", EarthsLandSeaSurface,
				L"Earth's Topographical/Bathymetric Surface", EarthsTopographicalBathymetricSurface))
				throw Exception("Unknown PowerUnits in DOMHelper::parseDOMAttribute");
			return units;
		}

		template <>
		static bool parseDOMAttribute<bool>(const DOMElement* pElement, const XMLCh* name) {
			return wstring(aStr(pElement->getAttribute(name))) == L"true";
		}

		template <>
		static wstring parseDOMAttribute<wstring>(const DOMElement* pElement, const XMLCh* name) {
			return wstring(aStr(pElement->getAttribute(name)));
		}

		template <>
		static string parseDOMAttribute<string>(const DOMElement* pElement, const XMLCh* name) {
			return quickW28(wstring(aStr(pElement->getAttribute(name))));
		}

		template <>
		static double parseDOMAttribute<double>(const DOMElement* pElement, const XMLCh* name) {
			double value;
			wstringstream ss(wstring(aStr(pElement->getAttribute(name))));
			ss >> value;
			if (ss.fail()) throw Exception("Unable to parseDOMAttribute<double>");
			return value;
		}

		//************************************************************************************************************
		//	parse DOMAttr* into value types

		template <class T>
		static T parseDOMAttribute(const DOMAttr* pAttr);

		template <>
		static ptime parseDOMAttribute<ptime>(const DOMAttr* pAttr) {
			if (pAttr == nullptr)
				return ptime(not_a_date_time);
			else
				return boost::date_time::parse_delimited_time<ptime>(quickW28(wstring(aStr(pAttr->getTextContent()))), 'T');
		}

	}
}
