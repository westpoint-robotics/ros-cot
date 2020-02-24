#pragma once

#include "PolarCoordinate.hpp"

namespace ARL {
	namespace Math {

		class SphericalCoordinate : public PolarCoordinate {
		public:
			SphericalCoordinate(const Angle& bearing, const Distance<> distance, const Angle& elevation) : PolarCoordinate(bearing, distance), elevation(elevation) { }
			SphericalCoordinate(const DOMElement* pElement)
				: PolarCoordinate(pElement),
				elevation(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("elevation"))->item(0))) { }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = xStr(L"SphericalCoordinate")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = PolarCoordinate::createDOMElement(pDoc, name);
				pElement->appendChild(distance.createDOMElement(pDoc, xStr(L"elevation")));
				return pElement;
			}
		protected:
			Angle elevation;

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<PolarCoordinate>(this), elevation);
			}
		};
	}
}