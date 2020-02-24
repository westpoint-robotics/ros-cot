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
		class Power : public ScaledValue < T, PowerUnits > {
		public:
			Power() {}
			Power(const T& v, PowerUnits u = Watts) {
				setValue(v, u);
			}

			Power(const DOMElement* pElement) : T(pElement) {
				units = ARL::Utility::DOMHelper::parseDOMAttribute(pElement, xStr("units"));
				setValue(magnitude, units);
			}

			void setValue(const T& v, PowerUnits u = Watts) { //always store internally in Watts, preferred units stored in Units
				this->units = u;
				switch (u) {
				case Watts: this->magnitude = v; break;
				case KiloWatts: this->magnitude = 1e3 * v; break;
				default: throw Exception("unknown distance units in Power::setValue()");
				}
			}

			T getValue(PowerUnits u = Watts) const {
				switch (u)  {
				case Watts: return this->magnitude;
				case KiloWatts: return this->magnitude * 1e-3; break;
				default: throw Exception("unknown distance units in Power::getValue()");
				}
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Power")) const override {
				DOMElement* pElement = ARL::Utility::DOMHelper::createDOMElement(getValueInPreferredUnits(), pDoc, name);
				return ARL::Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("Units"));
			}
		};
	}
}