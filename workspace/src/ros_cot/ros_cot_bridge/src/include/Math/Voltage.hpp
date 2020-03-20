#pragma once

#include "ScaledValue.hpp"
#include "Units.hpp"
#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <cereal/access.hpp>
#include <Utility/DOMHelper.hpp>

namespace ARL {
	namespace Math {

		using XERCES_CPP_NAMESPACE::DOMElement;
		using XERCES_CPP_NAMESPACE::DOMDocument;

		template <class T = double>
		class Voltage : public ScaledValue < T, VoltageUnits > {
		public:
			Voltage() {}
			Voltage(const T& v, VoltageUnits u = Volts) {
				setValue(v, u);
			}

			Voltage(const DOMElement* pElement) : T(pElement) {
				units = ARL::Utility::DOMHelper::parseDOMAttribute(pElement, xStr("units"));
				setValue(magnitude, units);
			}

			void setValue(const T& v, VoltageUnits u = Volts) { //always store internally in Volts, preferred units stored in Units
				this->units = u;
				switch (u) {
				case Volts: this->magnitude = v; break;
				default: throw Exception("unknown distance units in Voltage::setValue()");
				}
			}

			T getValue(const VoltageUnits u = Volts) const override {
				switch (u)  {
				case Volts: return this->magnitude;
				default: throw Exception("unknown distance units in Voltage::getValue()");
				}
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Voltage")) const override {
				DOMElement* pElement = Utility::DOMHelper::createDOMElement(getValueInPreferredUnits(), pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("Units"));
			}
		};
	}
}