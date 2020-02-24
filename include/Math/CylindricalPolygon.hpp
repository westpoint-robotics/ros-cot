#pragma once

#include "Geometry.hpp"

namespace ARL {
	namespace Math {

		class CylindricalPolygon : public Polygon < CylindricalCoordinate > {
		public:
			CylindricalPolygon(initializer_list<CylindricalCoordinate> list) : Polygon(list) {}
			CylindricalPolygon(const vector<CylindricalCoordinate>& geoVector) : Polygon(geoVector) {}
			CylindricalPolygon(const Polygon<CylindricalCoordinate> &list) : Polygon(list) {}

			CylindricalPolygon(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Polygon(pElement) { }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"CylindricalPolygon")) const override {
				return Polygon<CylindricalCoordinate>::createDOMElement(pDoc, name);
			}

		protected:
			CylindricalPolygon() {} //for cereal deserialization
		};
	}
}