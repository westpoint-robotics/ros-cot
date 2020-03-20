#pragma once

#include "Angle.hpp"
#include "Distance.hpp"
#include "Coordinate2D.hpp"

namespace ARL {
	namespace Math {
		using Utility::xStr;

		class PolarCoordinate : public XmlSerializable {
		public:
			PolarCoordinate(const Angle& azimuth, const Distance<> distance) : azimuth(azimuth), distance(distance) { }
			PolarCoordinate(const Coordinate2D& point)
				: PolarCoordinate(Angle(atan2(point.getValue(Meters).x, point.getValue(Meters).y), Radians), Distance<>(sqrt(point.getValue(Meters).x*point.getValue(Meters).x + point.getValue(Meters).y*point.getValue(Meters).y), Meters)) {}
			PolarCoordinate(const DOMElement* pElement)
				: azimuth(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("azimuth"))->item(0))),
				distance(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("distance"))->item(0))) { }

			const Angle getAzimuth() const { return azimuth; }
			const Distance<> getDistance() const { return distance; }
			Coordinate2D toCartesian() const {
				auto distanceMeters = getDistance().getValue(Meters);
				auto bearingRadians = getAzimuth().getValue(Radians);
				return Coordinate2D(distanceMeters * sin(bearingRadians), distanceMeters * cos(bearingRadians), Meters);
			}

			operator Coordinate2D() const {
				return toCartesian();
			}

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = xStr(L"PolarCoordinate")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(azimuth.createDOMElement(pDoc, xStr(L"azimuth")));
				pElement->appendChild(distance.createDOMElement(pDoc, xStr(L"distance")));
				return pElement;
			}
		protected:
			Angle azimuth;
			Distance<> distance;

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(azimuth, distance);
			}
		};

	}
}