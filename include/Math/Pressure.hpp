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

		class Pressure : public ScaledValue < double, PressureUnits > {
		public:
			Pressure(const double temp = 0, const PressureUnits units = Pascals) {
				setValue(temp, units);
			}

			Pressure(const DOMElement* pElement) {
				magnitude = Utility::DOMHelper::parseDOMElement<double>(pElement);
				units = Utility::DOMHelper::parseDOMAttribute<PressureUnits>(pElement, xStr("Units"));
				setValue(magnitude, units);
			}

			void setValue(const double& v, const PressureUnits units = Pascals) { //always store internally in MetersPerSecond, preferred units stored in Units
				this->units = units;
				switch (units) {
				case Pascals: magnitude = v; break;
				case psi: magnitude = psiToPascals(v); break;
				default: throw Exception("unknown pressure units in Pressure::setValue()");
				}
			}

			double getValue(const PressureUnits units = Pascals) const override { //always store internally in MetersPerSecond, preferred units stored in Units
				switch (units) {
				case Pascals: return magnitude;
				case psi: return pascalsToPSI(magnitude);
				default: throw Exception("unknown temperature units in Pressure::getValue()");
				}
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Pressure")) const override {
				return Utility::DOMHelper::setDOMAttribute(units, Utility::DOMHelper::createDOMElement(magnitude, pDoc, name), xStr("Units"));
			}

		protected:
			friend class cereal::access;
		};
	}
}