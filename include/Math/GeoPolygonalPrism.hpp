#pragma once

#include "GeoPolygon.hpp"
#include "PolygonalPrism.hpp"

namespace ARL {
	namespace Math {
		using namespace std;

		using WGS84Altitude = Distance<>;

		class GeoPolygonalPrism : public PolygonalPrism<GeoPolygon, WGS84Altitude> {
		public:
			GeoPolygonalPrism(const GeoPolygon& gp, const WGS84Altitude& min, const WGS84Altitude& max, AltitudeReferenceUnits ar = WGS84)
				: PolygonalPrism<GeoPolygon, WGS84Altitude>(gp, min, max), altitudeReference(ar) { }

			GeoPolygonalPrism(const XERCES_CPP_NAMESPACE::DOMElement* pElement)
				: PolygonalPrism<GeoPolygon, WGS84Altitude>(pElement), altitudeReference(WGS84) {
				if (pElement->hasAttribute(xStr("AltitudeReference")))
					altitudeReference = Utility::DOMHelper::parseDOMAttribute<AltitudeReferenceUnits>(pElement, xStr("AltitudeReference"));
			}

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"GeoPolygonalPrism")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(Polygon<GeoCoordinate>::createDOMElement(pDoc, Utility::xStr(L"GeoPolygon")));
				pElement->appendChild(mMin.createDOMElement(pDoc, Utility::xStr(L"MinimumAltitude")));
				pElement->appendChild(mMax.createDOMElement(pDoc, Utility::xStr(L"MaximumAltitude")));
				if (altitudeReference != WGS84)
					Utility::DOMHelper::setDOMAttribute(altitudeReference, pElement, xStr("AltitudeReference"));
				return pElement;
			}

			const AltitudeReferenceUnits& getAltitudeReference() const { return altitudeReference; }
			AltitudeReferenceUnits getAltitudeReference() { return altitudeReference; }
			const void setAltitudeReference(const AltitudeReferenceUnits& aru) { altitudeReference = aru; }
		protected:
			GeoPolygonalPrism() : altitudeReference(WGS84) {} // for cereal deserialization

			AltitudeReferenceUnits altitudeReference;

			friend class ARL::PathPlanning::NavPlan<Path2D>;
			friend class ARL::PathPlanning::NavPlan<Path3D>;
			friend class ARL::Database::NavPlanLogger;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive, std::uint32_t const version) { 
				archive(cereal::base_class<GeoPolygon>(this), mMin, mMax);
				if (version > 0)
					archive(altitudeReference);
			}
		};

	}
}

CEREAL_CLASS_VERSION(ARL::Math::GeoPolygonalPrism, 1);
