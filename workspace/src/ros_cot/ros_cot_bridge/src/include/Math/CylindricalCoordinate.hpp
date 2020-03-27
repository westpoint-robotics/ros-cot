#pragma once

#include "PolarCoordinate.hpp"

namespace ARL {
	namespace Math {

		class CylindricalCoordinate : public PolarCoordinate {
		public:
			CylindricalCoordinate(const Angle& bearing, const Distance<> distance, const Distance<>& height) : PolarCoordinate(bearing, distance), height(height) { }
			CylindricalCoordinate(const PolarCoordinate& polar, const Distance<>& height) : PolarCoordinate(polar), height(height) {}
			CylindricalCoordinate(const Coordinate3D& point)
				: PolarCoordinate(Angle(atan2(point.getValue(Meters).x, point.getValue(Meters).y), Radians), Distance<>(sqrt(point.getValue(Meters).x*point.getValue(Meters).x + point.getValue(Meters).y*point.getValue(Meters).y), Meters)),
				height(Distance<>(point.getValue(Meters).z, Meters)) {}
			CylindricalCoordinate(const DOMElement* pElement)
				: PolarCoordinate(pElement),
				height(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("height"))->item(0))) { }

			Coordinate3D toCartesian() const {
				auto distanceMeters = getDistance().getValue(Meters);
				auto bearingRadians = getAzimuth().getValue(Radians);
				return Coordinate3D(distanceMeters * sin(bearingRadians), distanceMeters * cos(bearingRadians), height.getValue(Meters), Meters);
			}

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = xStr(L"CylindricalCoordinate")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = PolarCoordinate::createDOMElement(pDoc, name);
				pElement->appendChild(height.createDOMElement(pDoc, xStr(L"height")));
				return pElement;
			}
		protected:
			Distance<> height;

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<PolarCoordinate>(this), height);
			}
		};
	}
}