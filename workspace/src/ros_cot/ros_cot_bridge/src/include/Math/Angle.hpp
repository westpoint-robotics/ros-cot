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
		using ARL::Utility::xStr;
		using namespace std;

		class Angle : public ScaledValue<double, AngleUnits> {
		public:
			Angle(double v = 0, AngleUnits u = Degrees) { setValue(v, u); }
			Angle(const DOMElement* pElement) {
				magnitude = ARL::Utility::DOMHelper::parseDOMElement<double>(pElement);
				units = ARL::Utility::DOMHelper::parseDOMAttribute<AngleUnits>(pElement, xStr("Units"));
				setValue(magnitude, units);
			}
			void setValue(double v, AngleUnits u = Degrees) {
				units = u;
				switch (u) {
				case Degrees: magnitude = degreesToRadians(v); break;
				case Radians: magnitude = v; break;
				default: throw Exception("Unknown Angle units in Angle::setValue()");
				}
			}
			double getValue(const AngleUnits u = Degrees) const override {
				switch (u) {
				case Degrees: return radiansToDegrees(magnitude);
				case Radians: return magnitude;
				default: throw Exception("Unknown Angle units in Angle::getValue()");
				}
			}
			void normalize() { *this = normalize(*this); }

			/*			friend bool operator<(const Angle& a1, const Angle& a2) {
			return a1.inRadians() < a2.inRadians();
			}
			friend bool operator>(const Angle& a1, const Angle& a2) {
			return a1.inRadians() > a2.inRadians();
			}
			friend bool operator==(const Angle& a1, const Angle& a2) {
			return a1.inRadians() == a2.inRadians();
			}
			friend bool operator!=(const Angle& a1, const Angle& a2) {
			return a1.inRadians() != a2.inRadians();
			}
			friend bool operator<=(const Angle& a1, const Angle& a2) {
			return a1.inRadians() <= a2.inRadians();
			}
			friend bool operator>=(const Angle& a1, const Angle& a2) {
			return a1.inRadians() >= a2.inRadians();
			}
			*/

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Angle")) const override {
				DOMElement* pElement = ARL::Utility::DOMHelper::createDOMElement(getValueInPreferredUnits(), pDoc, name);
				return ARL::Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("Units"));
			}

			static Angle normalize(const Angle a) {
				Angle a_norm = a;
				while (a_norm.magnitude < -pi) a_norm.magnitude += 2 * pi;
				while (a_norm.magnitude >= pi) a_norm.magnitude -= 2 * pi;
				return a_norm;
			}

			void AlignWith(const Angle &refAngle)
			{
				while ((refAngle.getValue(AngleUnits::Radians) - getValue(AngleUnits::Radians)) < -pi) setValue(getValue(AngleUnits::Radians) - 2 * pi, AngleUnits::Radians);
				while ((refAngle.getValue(AngleUnits::Radians) - getValue(AngleUnits::Radians)) >  pi) setValue(getValue(AngleUnits::Radians) + 2 * pi, AngleUnits::Radians);
			}

		protected:
			///normalize to range (-pi, pi]
			friend Angle operator+(const Angle& a1, const Angle& a2) {
				return normalize(Angle(a1.getValueInPreferredUnits() + a2.getValue(a1.getPreferredUnits()), a1.getPreferredUnits()));
			}

			friend Angle operator-(const Angle& a1, const Angle& a2) {
				return normalize(Angle(a1.getValueInPreferredUnits() - a2.getValue(a1.getPreferredUnits()), a1.getPreferredUnits()));
			}

			friend Angle operator-(const Angle& a) {
				return Angle(a.getValueInPreferredUnits(), a.getPreferredUnits());
			}

			friend Angle operator*(const Angle& a1, double x) {
				return normalize(Angle(a1.getValueInPreferredUnits() * x, a1.getPreferredUnits()));
			}

			friend Angle operator*(double x, const Angle& a1) {
				return a1 * x;
			}

			friend Angle operator/(const Angle& a1, double x) {
				return normalize(Angle(a1.getValueInPreferredUnits() / x, a1.getPreferredUnits()));
			}

			friend Angle operator+=(Angle& a1, const Angle& a2) {
				a1 = normalize(Angle(a1.getValueInPreferredUnits() + a2.getValue(a1.getPreferredUnits()), a1.getPreferredUnits()));
				return a1;
			}

			friend Angle operator-=(Angle& a1, const Angle& a2) {
				a1 = normalize(Angle(a1.getValueInPreferredUnits() - a2.getValue(a1.getPreferredUnits()), a1.getPreferredUnits()));
				return a1;
			}

			friend Angle operator*=(Angle& a1, double x) {
				a1 = normalize(Angle(a1.getValueInPreferredUnits() * x, a1.getPreferredUnits()));
				return a1;
			}

			friend Angle operator/=(Angle& a1, double x) {
				a1 = normalize(Angle(a1.getValueInPreferredUnits() / x, a1.getPreferredUnits()));
				return a1;
			}

			friend ostream& operator<<(ostream& o, const Angle& a) {
				o << a.getValueInPreferredUnits() << ' ' << AngleUnitHelper::ToString(a.getPreferredUnits());
				return o;
			}

			friend class cereal::access;
		};

	}
}