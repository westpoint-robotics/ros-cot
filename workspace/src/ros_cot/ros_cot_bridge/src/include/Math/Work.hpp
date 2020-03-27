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
		class Work : public ScaledValue < T, WorkUnits > {
		public:
			Work() {}
			Work(const T& v, WorkUnits u = Joules) {
				setValue(v, u);
			}

			Work(const DOMElement* pElement) : T(pElement) {
				units = ARL::Utility::DOMHelper::parseDOMAttribute(pElement, xStr("units"));
				setValue(magnitude, units);
			}

			void setValue(const T& v, WorkUnits u = Joules) { //always store internally in Joules, preferred units stored in Units
				this->units = u;
				switch (u) {
				case Joules: this->magnitude = v; break;
				case KiloWattHours: this->magnitude = kiloWattHoursToJoules(v); break;
				case WattHours: this->magnitude = wattHoursToJoules(v); break;
				default: throw Exception("unknown distance units in Work::setValue()");
				}
			}

			T getValue(const WorkUnits u = Joules) const override {
				switch (u)  {
				case Joules: return this->magnitude;
				case KiloWattHours: return joulesToKiloWattHours(this->magnitude);
				case WattHours: return joulesToWattHours(this->magnitude);
				default: throw Exception("unknown distance units in Work::getValue()");
				}
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Work")) const override {
				DOMElement* pElement = Utility::DOMHelper::createDOMElement(getValueInPreferredUnits(), pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("Units"));
			}
		};
	}
}