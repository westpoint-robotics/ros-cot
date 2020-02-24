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
		class Current : public ScaledValue < T, CurrentUnits > {
		public:
			Current() {}
			Current(const T& v, CurrentUnits u = Amps) {
				setValue(v, u);
			}

			Current(const DOMElement* pElement) : T(pElement) {
				units = ARL::Utility::DOMHelper::parseDOMAttribute(pElement, xStr("units"));
				setValue(magnitude, units);
			}

			void setValue(const T& v, CurrentUnits u = Amps) { //always store internally in Amps, preferred units stored in Units
				this->units = u;
				switch (u) {
				case Amps: this->magnitude = v; break;
				case milliAmps: this->magnitude = 1e-3 * v; break;
				default: throw Exception("unknown distance units in Current::setValue()");
				}
			}

			T getValue(const CurrentUnits u = Amps) const override {
				switch (u)  {
				case Amps: return this->magnitude;
				case milliAmps: return this->magnitude * 1e3; break;
				default: throw Exception("unknown distance units in Current::getValue()");
				}
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Current")) const override {
				DOMElement* pElement = ARL::Utility::DOMHelper::createDOMElement(getValueInPreferredUnits(), pDoc, name);
				return ARL::Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("Units"));
			}
		};
	}
}