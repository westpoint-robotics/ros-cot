#pragma once

#include "ScaledValue.hpp"
#include "Units.hpp"
#include <Utility/xstr.hpp>
#include <Utility/DOMHelper.hpp>

namespace ARL {
	namespace Math {

		XERCES_CPP_NAMESPACE_USE;
		using XERCES_CPP_NAMESPACE::DOMDocument;
		using XERCES_CPP_NAMESPACE::DOMElement;
		using ARL::Utility::xStr;

		class Temperature : public ScaledValue < double, TemperatureUnits > {
		public:
			Temperature(const double temp = 0, const TemperatureUnits units = Celsius) {
				setValue(temp, units);
			}

			Temperature(const DOMElement* pElement) {
				magnitude = Utility::DOMHelper::parseDOMElement<double>(pElement);
				units = Utility::DOMHelper::parseDOMAttribute<TemperatureUnits>(pElement, xStr("Units"));
				setValue(magnitude, units);
			}

			void setValue(const double& v, const TemperatureUnits units = Celsius) {  //always store internally in MetersPerSecond, preferred units stored in Units
				this->units = units;
				switch (units) {
				case Celsius: magnitude = v; break;
				case Kelvin: magnitude = kelvinToCelsius(v); break;
				case Fahrenheit: magnitude = fahrenheitToCelsius(v); break;
				default: throw Exception("unknown temperature units in Temperature::setValue()");
				}
			}

			double getValue(const TemperatureUnits units = Celsius) const override { //always store internally in MetersPerSecond, preferred units stored in Units
				switch (units) {
				case Celsius: return magnitude;
				case Kelvin: return celsiusToKelvin(magnitude);
				case Fahrenheit: return celsiusToFahrenheit(magnitude);
				default: throw Exception("unknown temperature units in Temperature::getValue()");
				}
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Temperature")) const override {
				return Utility::DOMHelper::setDOMAttribute(units, Utility::DOMHelper::createDOMElement(magnitude, pDoc, name), xStr("Units"));
			}

		protected:
			friend class cereal::access;
		};
	}
}