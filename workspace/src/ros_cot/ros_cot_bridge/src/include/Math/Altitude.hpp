#pragma once
#include "Distance.hpp"

namespace ARL {
	namespace Math {

		using XERCES_CPP_NAMESPACE::DOMElement;
		using XERCES_CPP_NAMESPACE::DOMDocument;
		using ARL::Utility::xStr;
		using ARL::Utility::quickW28;
		using namespace std;

		class Altitude : public Distance < double > {
		public:
			Altitude(const double& d = std::numeric_limits<double>::quiet_NaN(), LengthUnits u = Meters, AltitudeReferenceUnits ar = WGS84)
				: Distance<double>(d, u), altitudeReference(ar) {}

			Altitude(const DOMElement* pElement) : Distance<double>(pElement), altitudeReference(WGS84) {
				if (pElement->hasAttribute(xStr("AltitudeReference")))
					altitudeReference = Utility::DOMHelper::parseDOMAttribute<AltitudeReferenceUnits>(pElement, xStr("AltitudeReference"));
			}

			Altitude(const Distance<double>& distance, const AltitudeReferenceUnits aru = WGS84)
				: Distance<double>(distance), altitudeReference(aru) {}

			Altitude operator-() const {
				Altitude neg = *this;
				neg.magnitude = -magnitude;
				return neg;
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Distance")) const {
				DOMElement* pElement = Distance<double>::createDOMElement(pDoc, name);
				if (altitudeReference != WGS84)
					Utility::DOMHelper::setDOMAttribute(altitudeReference, pElement, xStr("AltitudeReference"));
				return pElement;
			}

			const AltitudeReferenceUnits& getAltitudeReference() const { return altitudeReference; }
			AltitudeReferenceUnits getAltitudeReference() { return altitudeReference; }

		protected:
			AltitudeReferenceUnits altitudeReference;
		};

	}
}