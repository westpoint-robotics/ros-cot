#pragma once

#include "CylindricalPolygon.hpp"
#include "PolygonalPrism.hpp"

namespace ARL {
	namespace Math {

		class CylindricalPolygonalPrism : public PolygonalPrism<CylindricalPolygon, Distance<>> {
		public:
			CylindricalPolygonalPrism(const CylindricalPolygon& cp, const Distance<> min, const Distance<> max)
				: PolygonalPrism<CylindricalPolygon, Distance<>>(cp, min, max) { }

			CylindricalPolygonalPrism(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : PolygonalPrism<CylindricalPolygon, Distance<>>(pElement) { }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"CylindricalPolygonalPrism")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(Polygon<CylindricalCoordinate>::createDOMElement(pDoc, Utility::xStr(L"CylindricalPolygon")));
				pElement->appendChild(mMin.createDOMElement(pDoc, Utility::xStr(L"MinimumAltitude")));
				pElement->appendChild(mMax.createDOMElement(pDoc, Utility::xStr(L"MaximumAltitude")));
				return pElement;
			}

		protected:
			CylindricalPolygonalPrism() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<GeoPolygon>(this), mMin, mMax);
			}
		};

	}
}