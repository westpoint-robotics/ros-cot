#pragma once
#include "Coordinate3D.hpp"

namespace ARL {
	namespace Math {

		struct Coordinate2D : public ScaledValue < Point2D, LengthUnits > {
			Coordinate2D(double x = 0, double y = 0, LengthUnits u = Meters) {
				setValue(Point2D(x, y), u);
			}
			Coordinate2D(const Point2D& value, LengthUnits u = Meters) {
				setValue(value, u);
			}
			Coordinate2D(const Coordinate3D& value) {
				setValue(value.getValueInPreferredUnits(), value.getPreferredUnits());
			}
			Coordinate2D(const DOMElement* pElement) : ScaledValue <Point2D, LengthUnits>(pElement) {
				units = ARL::Utility::DOMHelper::parseDOMAttribute<LengthUnits>(pElement, xStr("Units"));
				setValue(magnitude, units);
			}
			void setValue(const Point2D& v, LengthUnits u = Meters) {
				units = u; //preferred units
				switch (u) { //always store internally in Meters
				case Meters: magnitude = v; break;
				case NauticalMiles: magnitude = nauticalMilesToMeters(v); break;
				case Miles: magnitude = milesToMeters(v); break;
				case Kilometers: magnitude = 1000 * v; break;
				case Feet: magnitude = feetToMeters(v); break;
				case Yards: magnitude = yardsToMeters(v); break;
				default: throw Exception("unknown distance units in Coordinate2D::setValue()");
				}
			}
			Coordinate2D normalize() const {
				Coordinate2D p = *this;
				p.normalize();
				return p;
			}
			Coordinate2D& normalize() { magnitude.normalize(); return *this; }
			operator Coordinate3D() const { Point3D p = getValueInPreferredUnits(); return Coordinate3D(p, getPreferredUnits()); }

			Point2D getValue(const LengthUnits u = Meters) const override {
				switch (u)  {
				case Meters: return magnitude;
				case NauticalMiles: return metersToNauticalMiles(magnitude);
				case Miles: return metersToMiles(magnitude);
				case Kilometers: return 0.001 * magnitude;
				case Feet: return metersToFeet(magnitude);
				case Yards: return metersToYards(magnitude);
				default: throw Exception("unknown distance units in Coordinate2D::getValue()");
				}
			}

			Distance<> X() const { return Distance<>(getValueInPreferredUnits().x, units); }
			Distance<> Y() const { return Distance<>(getValueInPreferredUnits().y, units); }

			Distance<> getDistance(const Coordinate2D& coord) const {
				return Distance<>(getValueInPreferredUnits().distance(coord.getValue(getPreferredUnits())), getPreferredUnits());
			}
			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Coordinate2D")) const override {
				DOMElement* pElement = magnitude.createDOMElement(pDoc, name);
				return ARL::Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("Units"));
			}

			friend class cereal::access;
		};

	}
}