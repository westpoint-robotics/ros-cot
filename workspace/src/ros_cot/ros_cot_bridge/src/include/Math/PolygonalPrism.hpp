#pragma once

#include "Geometry.hpp"

namespace ARL {
	namespace Math {

		/**
		* PolygonT, AltitudeT must both implement XmlSerializable
		*/
		template <class PolygonT, class AltitudeT>
		class PolygonalPrism : public PolygonT {
		public:
			PolygonalPrism(const PolygonT& poly, const AltitudeT& min, const AltitudeT& max) : PolygonT(poly), mMin(min), mMax(max) {
				if (mMin >= mMax) throw Exception("minimum must be less than maximum in PolygonalPrism constructor");
			}

			PolygonalPrism(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : PolygonT(pElement->getFirstElementChild()),
				mMin(pElement->getFirstElementChild()->getNextElementSibling()),
				mMax(pElement->getFirstElementChild()->getNextElementSibling()->getNextElementSibling())
			{
				// assumes min and max altitude come after GeoPolygon in order
				if (mMin >= mMax) throw Exception("minimum must be less than maximum in GeoPolygonalPrism constructor");
			}

			const AltitudeT& getMinimumAltitude() const { return mMin; }
			const AltitudeT& getMaximumAltitude() const { return mMax; }

			void setMinimumAltitude(const AltitudeT& alt) { mMin = alt; }
			void setMaximumAltitude(const AltitudeT& alt) { mMax = alt; }

		protected:
			AltitudeT mMin;
			AltitudeT mMax;

			PolygonalPrism() {} // for cerial deserialization
			friend class cereal::access;
		};
	}
}