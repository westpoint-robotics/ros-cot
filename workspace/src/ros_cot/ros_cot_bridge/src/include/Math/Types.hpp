#pragma once

#include "PercentValue.hpp"
#include "NormalizedValue.hpp"
#include "NonNegativeDouble.hpp"
#include "Speed.hpp"
#include "Voltage.hpp"
#include "Work.hpp"
#include "Current.hpp"
#include "Power.hpp"
#include "Angle.hpp"
#include "NonNegativeDistance.hpp"
#include "Point3D.hpp"
#include "Coordinate2D.hpp"
#include "SphericalCoordinate.hpp"
#include "CylindricalCoordinate.hpp"
#include "Temperature.hpp"
#include "Pressure.hpp"
#include "Altitude.hpp"

#include <Utility/DOMHelper.hpp>
#include <iomanip>
#include <Messaging/XmlSerializable.hpp>
#include <Filter/InfiniteImpulseResponseDigitalFilter.hpp>
#include <limits>
#include <cereal/types/boost/date_time.hpp>

namespace ARL {
	namespace Math {
		const char XMLTimeDurationFormatString[] = "%H:%M:%S%F%Q";

		using namespace std;
		using XERCES_CPP_NAMESPACE::DOMDocument;
		using XERCES_CPP_NAMESPACE::DOMElement;
		using XERCES_CPP_NAMESPACE::DOMAttr;

		using boost::posix_time::ptime;
		using boost::posix_time::not_a_date_time;
		using ARL::Filter::InfiniteImpulseResponseDigitalFilter;
		using ARL::Filter::InfiniteImpulseResponseDigitalAngleFilter;
		using ARL::Messaging::XmlSerializable;
		using Utility::xStr;
		using Utility::aStr;
		using ARL::Utility::StringSwitchSet;
		using ARL::Utility::WStringSwitchSet;

		class Frequency : public ScaledValue<NonNegativeDouble, FrequencyUnits> {
		public:
			Frequency() {}
			Frequency(const NonNegativeDouble& v, FrequencyUnits u = Hertz) {
				setValue(v, u);
			}
			Frequency(const DOMElement* pElement) {
				magnitude = Utility::DOMHelper::parseDOMElement<NonNegativeDouble>(pElement);
				units = Utility::DOMHelper::parseDOMAttribute<FrequencyUnits>(pElement, xStr("units"));
				setValue(magnitude, units);
			}
			void setValue(const NonNegativeDouble& v, FrequencyUnits u = Hertz) { //always store internally in MetersPerSecond, preferred units stored in Units
				this->units = u;
				switch (u) {
				case Hertz: this->magnitude = v; break;
				case KiloHertz: this->magnitude = v * 1e3; break;
				case MegaHertz: this->magnitude = v * 1e6; break;
				case GigaHertz: this->magnitude = v * 1e9; break;
				case TeraHertz: this->magnitude = v * 1e12; break;
				default: throw Exception("unknown Frequency units in Frequency::setValue()");
				}
			}
			NonNegativeDouble getValue(const FrequencyUnits u = Hertz) const override {
				switch (u)  {
				case Hertz: return magnitude;
				case KiloHertz: return magnitude * 1e-3;
				case MegaHertz: return magnitude * 1e-6; break;
				case GigaHertz: return magnitude * 1e-9; break;
				case TeraHertz: return magnitude * 1e-12; break;
				default: throw Exception("unknown Frequency units in Frequency::getValue()");
				}
			}
			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Frequency")) const override {
				DOMElement* pElement = Utility::DOMHelper::createDOMElement(getValueInPreferredUnits(), pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("Units"));
			}
		};

		/// @note WGS84 geocoordinates required.
		class Latitude : protected RestrictedValue<Angle, -90, 90>, public XmlSerializable {
		public:
			Latitude(double lat = 0, ARL::Math::AngleUnits units = ARL::Math::Degrees) : RestrictedValue(Angle(units == ARL::Math::Degrees ? lat : radiansToDegrees(lat) , ARL::Math::Degrees)) { }
			Latitude(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : RestrictedValue<Angle, -90, 90>(pElement) { }
			void setValue(double v, AngleUnits u = Degrees) {
				switch (u) {
				case Degrees:
					RestrictedValue::setValue(Angle(v, Degrees)); break;
				case Radians:
					RestrictedValue::setValue(Angle(radiansToDegrees(v), Degrees)); break;
				default: throw Exception("Unknown Angle units in Latitude::setValue()");
				}
			}
			double getValue(const AngleUnits u = Degrees) const {
				return RestrictedValue::getValue().getValue(u);
			}
			Latitude& filteredUpdate(const Latitude& lat, const double alpha = 1.0 / 3) {
				setValue(InfiniteImpulseResponseDigitalFilter<>::computeResponse(lat.getValue(Degrees), getValue(Degrees), alpha));
				return *this;
			}
			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"latitude")) const override {
				return RestrictedValue::getValue().createDOMElement(pDoc, name);
			}
		protected:

			friend class cereal::access;
		};

		class Longitude : protected RestrictedValue<Angle, -180, 180>, public XmlSerializable {
		public:
			Longitude(double lon = 0, ARL::Math::AngleUnits units = ARL::Math::Degrees) : RestrictedValue(Angle(units == ARL::Math::Degrees ? lon : radiansToDegrees(lon), ARL::Math::Degrees)) { }
			Longitude(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : RestrictedValue<Angle, -180, 180>(pElement) { }
			void setValue(double v, AngleUnits u = Degrees) {
				switch (u) {
				case Degrees:
					RestrictedValue::setValue(Angle(v, Degrees)); break;
				case Radians:
					RestrictedValue::setValue(Angle(radiansToDegrees(v), Degrees)); break;
				default: throw Exception("Unknown Angle units in Longitude::setValue()");
				}
			}
			double getValue(const AngleUnits u = Degrees) const {
				return RestrictedValue::getValue().getValue(u);
			}
			Longitude& filteredUpdate(const Longitude& lon, const double alpha = 1.0 / 3) {
				setValue(InfiniteImpulseResponseDigitalFilter<>::computeResponse(lon.getValue(Degrees), getValue(Degrees), alpha));
				return *this;
			}
			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr("longitude")) const {
				return RestrictedValue::getValue().createDOMElement(pDoc, name);
			}
		protected:

			friend class cereal::access;
		};

		struct EulerAngles1D : public XmlSerializable {
			EulerAngles1D(double yaw = 0) { this->yaw = yaw; }
			EulerAngles1D(const DOMElement* pElement) {
				yaw = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("yaw"))->item(0)));
			}
			double yaw;

			EulerAngles1D& normalize(AngleUnits units = Radians) {
				double lim = 0;
				switch (units) {
				case Radians: lim = pi; break;
				case Degrees: lim = 180; break;
				default: throw Exception("unknown AngleUnits in SphericalPoint3D::normalize()");
				}
				// normalize axis into range (-pi,pi] Radians or (-180,180] degrees
				while (yaw <= -lim) yaw += 2 * lim; while (yaw > lim) yaw -= 2 * lim;
				return *this;
			}

			EulerAngles1D operator-() const { return EulerAngles1D(-yaw); }
			friend EulerAngles1D operator+(const EulerAngles1D& s1, const EulerAngles1D& s2) {
				return EulerAngles1D(s1.yaw + s2.yaw); // since Speed is always stored internally in MetersPerSecond, adding two speeds is always simple. Take preferred units as the left hand side.
			}
			friend EulerAngles1D operator-(const EulerAngles1D& s1, const EulerAngles1D& s2) {
				return EulerAngles1D(s1.yaw - s2.yaw);
			}
			friend EulerAngles1D& operator+=(EulerAngles1D& s1, const EulerAngles1D& s2) {
				s1.yaw += s2.yaw;
				return s1;
			}
			friend EulerAngles1D& operator-=(EulerAngles1D& s1, const EulerAngles1D& s2) {
				s1.yaw -= s2.yaw;
				return s1;
			}
			friend EulerAngles1D operator*(const EulerAngles1D& s, double a){
				return EulerAngles1D(s.yaw*a);
			}
			friend EulerAngles1D operator*(double a, const EulerAngles1D& s){
				return EulerAngles1D(s.yaw*a);
			}
			friend EulerAngles1D operator/(const EulerAngles1D& s, double a){
				return EulerAngles1D(s.yaw / a);
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"EulerAngles1D")) const override {
				DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(Utility::DOMHelper::createDOMElement(yaw, pDoc, xStr(L"yaw")));
				return pElement;
			}

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(yaw);
			}
		};

		/// Represents the orientation of a rigid body with respect to local ENU cartesian coordinate system
		/// roll rotation about the unit X axis
		/// pitch rotation about the unit Y axis
		/// yaw rotation about the unit Z axis
		struct EulerAngles3D : public EulerAngles1D { ///@todo represent internally as quaternion?
			using Quaternion = Eigen::Quaterniond;
			using AngleAxis = Eigen::AngleAxisd;

			EulerAngles3D(double roll = 0, double pitch = 0, double yaw = 0) : EulerAngles1D(yaw) { this->roll = roll; this->pitch = pitch; }

			EulerAngles3D(const DOMElement* pElement) : EulerAngles1D(pElement) {
				roll = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("roll"))->item(0)));
				pitch = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("pitch"))->item(0)));
			}

			/// construct a EulerAngles3D from a quaternion constructed from NED convention.
			EulerAngles3D(const Quaternion& quaternion) {
				auto eulerAngles = quaternion.toRotationMatrix().eulerAngles(2,1,0);
				pitch = eulerAngles[2]; roll = eulerAngles[1], yaw = -eulerAngles[0];
				if (pitch > 0.5*pi) {
					pitch = -pi + pitch; // map [0,pi) to [-pi/2,pi)
					assert(-0.5*pi <= pitch && pitch <= 0.5*pi);
					if (roll < 0)
						roll = -pi - roll;
					else
						roll = pi - roll;
					assert(-pi <= roll && roll <= pi);
					if (yaw <= 0)
						yaw = pi + yaw;
					else
						yaw = -pi + yaw;
					assert(-pi <= yaw && yaw <= pi);
				}
			}

			double roll, pitch;

			EulerAngles3D& normalize(AngleUnits units = Radians) {
				double lim = 0;
				switch (units) {
				case Radians: lim = pi; break;
				case Degrees: lim = 180; break;
				default: throw Exception("unknown AngleUnits in SphericalPoint3D::normalize()");
				}
				// normalize each axis into range (-pi,pi] Radians or (-180,180] degrees
				// start with pitch
				while (pitch <= -lim) pitch += 2 * lim; while (pitch > lim) pitch -= 2 * lim;
				// normalize pitch into range (-pi/2,pi/2] or (-90,90] degrees, with requist changes in yaw and roll
				if (pitch <= -0.5*lim) {
					pitch += lim;
					roll += lim;
					yaw += lim;
				}
				else if (pitch > 0.5*lim) {
					pitch -= lim;
					roll += lim;
					yaw += lim;
				}
				while (roll <= -lim) roll += 2 * lim; while (roll > lim) roll -= 2 * lim;
				while (yaw <= -lim) yaw += 2 * lim; while (yaw > lim) yaw -= 2 * lim;
				return *this;
			}

			operator Quaternion () const { ///convert Orientation3D into Quaternion from NED convention
				return AngleAxis(-yaw, Eigen::Vector3d::UnitZ()) *
					AngleAxis(roll, Eigen::Vector3d::UnitY()) *
					AngleAxis(pitch, Eigen::Vector3d::UnitX());
			}

//			EulerAngles3D& filteredUpdate(const EulerAngles3D& o, const double alpha = 1.0 / 3) {
//				Quaternion q = o, p = *this;
//				p.w() = ARL::Filter::InfiniteImpulseResponseDigitalFilter<double>::computeResponse(q.w(), p.w(), alpha);
//				p.x() = ARL::Filter::InfiniteImpulseResponseDigitalFilter<double>::computeResponse(q.x(), p.x(), alpha);
//				p.y() = ARL::Filter::InfiniteImpulseResponseDigitalFilter<double>::computeResponse(q.y(), p.y(), alpha);
//				p.z() = ARL::Filter::InfiniteImpulseResponseDigitalFilter<double>::computeResponse(q.z(), p.z(), alpha);
//				p.normalize();
//				cout << o.yaw << " " << yaw << endl;
//				*this = p;
//				return *this;
//			}

			EulerAngles3D operator-() const { return EulerAngles3D(-roll, -pitch, -yaw); }
			friend EulerAngles3D operator+(const EulerAngles3D& s1, const EulerAngles3D& s2) {
				return EulerAngles3D(s1.roll + s2.roll, s1.pitch + s2.pitch, s1.yaw + s2.yaw); // since Speed is always stored internally in MetersPerSecond, adding two speeds is always simple. Take preferred units as the left hand side.
			}
			friend EulerAngles3D operator-(const EulerAngles3D& s1, const EulerAngles3D& s2) {
				return EulerAngles3D(s1.roll - s2.roll, s1.pitch - s2.pitch, s1.yaw - s2.yaw);
			}
			friend EulerAngles3D& operator+=(EulerAngles3D& s1, const EulerAngles3D& s2) {
				s1.roll += s2.roll; s1.pitch += s2.pitch; s1.yaw += s2.yaw;
				return s1;
			}
			friend EulerAngles3D& operator-=(EulerAngles3D& s1, const EulerAngles3D& s2) {
				s1.roll -= s2.roll; s1.pitch -= s2.pitch; s1.yaw -= s2.yaw;
				return s1;
			}
			friend EulerAngles3D operator*(const EulerAngles3D& s, double a){
				return EulerAngles3D(s.roll * a, s.pitch * a, s.yaw*a);
			}
			friend EulerAngles3D operator*(const EulerAngles3D& s, const EulerAngles3D& a){
				return EulerAngles3D(s.roll * a.roll, s.pitch * a.pitch, s.yaw*a.yaw);
			}
			friend EulerAngles3D operator*(double a, const EulerAngles3D& s){
				return EulerAngles3D(s.roll * a, s.pitch * a, s.yaw*a);
			}
			friend EulerAngles3D operator/(const EulerAngles3D& s, double a){
				return EulerAngles3D(s.roll / a, s.pitch / a, s.yaw / a);
			}
			friend EulerAngles3D& operator*=(EulerAngles3D& s1, double d) {
				s1.roll *= d; s1.pitch *= d; s1.yaw *= d;
				return s1;
			}
			friend EulerAngles3D& operator/=(EulerAngles3D& s1, double d) {
				s1.roll /= d; s1.pitch /= d; s1.yaw /= d;
				return s1;
			}
			friend ostream& operator<<(ostream& o, const EulerAngles3D& p) {
				o << "<" << p.roll << "," << p.pitch << "," << p.yaw << ">";
				return o;
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"EulerAngles3D")) const {
				DOMElement* pElement = EulerAngles1D::createDOMElement(pDoc, name);
				pElement->appendChild(Utility::DOMHelper::createDOMElement(pitch, pDoc, xStr(L"pitch")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement(roll, pDoc, xStr(L"roll")));
				return pElement;
			}

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(roll, pitch, cereal::base_class<EulerAngles1D>(this));
			}
		};

		class Orientation1D : public ScaledValue<EulerAngles1D, AngleUnits> {
		public:
			Orientation1D(double yaw = 0, AngleUnits u = Radians) { setValue(EulerAngles1D(yaw), u); }
			Orientation1D(const EulerAngles1D& v, AngleUnits u = Radians) { setValue(v, u); }
			void setValue(const EulerAngles1D& v, AngleUnits u = Radians) {
				units = u;
				switch (u) {
				case Radians: magnitude = v; break;
				case Degrees: magnitude = degreesToRadians(v); break;
				default: throw Exception("Unknown Angle Units in Orientation1D");
				}
				magnitude.normalize(Radians);
			}

			EulerAngles1D getValue(const AngleUnits u = Radians) const override {
				switch (u) {
				case Radians: return magnitude;
				case Degrees: return radiansToDegrees(magnitude); break;
				default: throw Exception("Unknown Angle Units in Orientation1D");
				}
			}

			Orientation1D& filteredUpdate(const Orientation1D& o, const double alpha = 1.0 / 3) {
				magnitude.yaw = InfiniteImpulseResponseDigitalAngleFilter::computeAngleResponse(o.magnitude.yaw, magnitude.yaw, alpha);
				return *this;
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr("Orientation1D")) const override {
				DOMElement* pElement = magnitude.createDOMElement(pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("units"));
			}

			Orientation1D& operator+=(const Orientation1D& s2) {
				magnitude += s2.magnitude;
				magnitude.normalize(Radians);
				return *this;
			}
			Orientation1D& operator-=(const Orientation1D& s2) {
				magnitude -= s2.magnitude;
				magnitude.normalize(Radians);
				return *this;
			}
		protected:
			Orientation1D operator-() const { return Orientation1D(-getValueInPreferredUnits(), units); }
			friend Orientation1D operator+(const Orientation1D& s1, const Orientation1D& s2) {
				return Orientation1D(s1.magnitude + s2.magnitude, Radians);
			}
			friend Orientation1D operator-(const Orientation1D& s1, const Orientation1D& s2) {
				return Orientation1D(s1.magnitude - s2.magnitude, Radians);
			}
			friend Orientation1D operator*(const Orientation1D& s, double a){
				return Orientation1D(s.magnitude*a, Radians);
			}
			friend Orientation1D operator*(double a, const Orientation1D& s){
				return Orientation1D(s.magnitude*a, Radians);
			}
			friend Orientation1D operator/(const Orientation1D& s, double a){
				return Orientation1D(s.magnitude / a, Radians);
			}
//			friend ostream& operator<<(ostream& o, const Orientation1D& v) {
//				o << v.getValueInPreferredUnits() << " " << AngleUnitHelper::ToString(v.units);
//				return o;
//			}

			friend class cereal::access;
		};

		/**
		* Representation of platform orientation using East-North-Up:
		* Positive X-Axis => East
		* Positive Y-Axis => North
		* Positive Z-Axis => Up
		*/
		class Orientation3D : public ScaledValue<EulerAngles3D, AngleUnits> {
		public:
			using Quaternion = Eigen::Quaterniond;

			Orientation3D(double roll = 0, double pitch = 0, double yaw = 0, AngleUnits u = Radians) { setValue(EulerAngles3D(roll, pitch, yaw), u); }
			Orientation3D(const EulerAngles3D& v, AngleUnits u = Radians) { setValue(v, u); }
			Orientation3D(const DOMElement* pElement) : ScaledValue<EulerAngles3D, AngleUnits>(pElement) {
				units = Utility::DOMHelper::parseDOMAttribute<AngleUnits>(pElement, xStr("units"));
				setValue(magnitude, units);
			}
			Orientation3D(const Orientation1D& o) : Orientation3D(0, 0, o.getValueInPreferredUnits().yaw, o.getPreferredUnits()) {}
			Orientation3D(const Quaternion& q) {
				units = Radians;
				magnitude = q;
			}

			void setValue(const EulerAngles3D& v, AngleUnits u = Radians) {
				units = u;
				switch (u) {
				case Radians: magnitude = v; break;
				case Degrees: magnitude = degreesToRadians(v); break;
				default: throw Exception("Unknown Angle Units in Orientation3D::setValue()");
				}
				magnitude.normalize(Radians);
			}

			EulerAngles3D getValue(const AngleUnits u = Radians) const override {
				switch (u) {
				case Radians: return magnitude;
				case Degrees: return radiansToDegrees(magnitude); break;
				default: throw Exception("Unknown Angle Units in Orientation3D::getValue()");
				}
			}

			Orientation3D& filteredUpdate(const Orientation3D& o, const double alpha = 1.0 / 3) {
				magnitude.roll = InfiniteImpulseResponseDigitalAngleFilter::computeAngleResponse(o.magnitude.roll, magnitude.roll, alpha);
				magnitude.pitch = InfiniteImpulseResponseDigitalAngleFilter::computeAngleResponse(o.magnitude.pitch, magnitude.pitch, alpha);
				magnitude.yaw = InfiniteImpulseResponseDigitalAngleFilter::computeAngleResponse(o.magnitude.yaw, magnitude.yaw, alpha);
				magnitude.normalize(Radians);
				return *this;
			}

			operator Quaternion() const {
				return magnitude;
			}

			Orientation1D getOrientation1D() const { return Orientation1D(this->getValueInPreferredUnits().yaw, this->getPreferredUnits()); }
			operator Orientation1D() const { return Orientation1D(this->getValueInPreferredUnits().yaw, this->getPreferredUnits()); }

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr("Orientation3D")) const override {
				DOMElement* pElement = magnitude.createDOMElement(pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("units"));
			}

		protected:
			Orientation3D operator-() const { return Orientation3D(-getValueInPreferredUnits(), units); }
			friend Orientation3D operator+(const Orientation3D& s1, const Orientation3D& s2) {
				return Orientation3D(s1.magnitude + s2.magnitude, Radians);
			}
			friend Orientation3D operator-(const Orientation3D& s1, const Orientation3D& s2) {
				return Orientation3D(s1.magnitude - s2.magnitude, Radians);
			}
			friend Orientation3D& operator+=(Orientation3D& s1, const Orientation3D& s2) {
				s1.setValue(s1.magnitude + s2.magnitude, Radians);
				s1.magnitude.normalize(Radians);
				return s1;
			}
			friend Orientation3D& operator-=(Orientation3D& s1, const Orientation3D& s2) {
				s1.magnitude -= s2.magnitude;
				s1.magnitude.normalize(s1.getPreferredUnits());
				return s1;
			}
			friend Orientation3D operator*(const Orientation3D& s, double a){
				return Orientation3D(s.magnitude*a, Radians);
			}
			friend Orientation3D operator*(double a, const Orientation3D& s){
				return Orientation3D(s.magnitude*a, Radians);
			}
			friend Orientation3D operator/(const Orientation3D& s, double a){
				return Orientation3D(s.magnitude / a, Radians);
			}
			friend ostream& operator<<(ostream& o, const Orientation3D& v) {
				o << v.getValueInPreferredUnits() << " " << AngleUnitHelper::ToString(v.units);
				return o;
			}

			friend class cereal::access;
		};

		struct GeoCoordinate : public XmlSerializable {
			GeoCoordinate(const Latitude& lat = Latitude(40.7926733),
				const Longitude& lon = Longitude(-77.8659007))
			: latitude(lat), longitude(lon) {}

			GeoCoordinate(const DOMElement* pElement)
				: latitude(pElement->getFirstElementChild()), longitude(pElement->getFirstElementChild()->getNextElementSibling()) { }

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr("GeoCoordinate")) const override {
				DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(latitude.createDOMElement(pDoc, xStr("latitude")));
				pElement->appendChild(longitude.createDOMElement(pDoc, xStr("longitude")));
				return pElement;
			}

			Latitude latitude;
			Longitude longitude;

			GeoCoordinate& filteredUpdate(const GeoCoordinate& geoCoordinate, const double alpha = 1.0/3) {
				latitude.filteredUpdate(geoCoordinate.latitude, alpha);
				longitude.filteredUpdate(geoCoordinate.longitude, alpha);
				return *this;
			}

			friend ostream& operator<<(ostream& o, const GeoCoordinate& c) {
				o << c.latitude.getValue(Degrees) << ',' << c.longitude.getValue(Degrees);
				return o;
			}

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(latitude, longitude);
			}

			/// Calculates the distance between two lat/long pairs.
			/// Uses a true north-based azimuth.
			/// @param p1 The lat/long of the first (this) point.
			/// @param p2 The lat/long of the second point.
			/// @return Distance between p1 and p2 in meters.
			Distance<double> GreatCircleDistance(const GeoCoordinate &p2) const {

				ARL::Math::AngleUnits rad = ARL::Math::Radians;

				auto deltaLat = p2.latitude.getValue(rad) - latitude.getValue(rad);
				auto deltaLon = p2.longitude.getValue(rad) - longitude.getValue(rad);

				auto a = pow(sin(deltaLat / 2.0F), 2)
					+ cos(latitude.getValue(rad))
					* cos(p2.latitude.getValue(rad))
					* pow(sin(deltaLon / 2.0F), 2);

				auto c = 2 * atan2(sqrt(a), sqrt(1 - a));

				return ARL::Math::Distance<double>(Geodetic::R1 * c, ARL::Math::Meters);
			}

			/// Calculates the great-circle bearing between two lat/long pairs.
			/// Uses a true north-based azimuth.
			/// @param p1 The lat/long of the first (this) point.
			/// @param p2 The lat/long of the second point.
			/// @return Angle between p1 and p2 in normalized [0, 360) degrees.
			Angle ForwardAzimuth(const GeoCoordinate &p2) const {

				ARL::Math::AngleUnits rad = ARL::Math::Radians;

				// Method from SunEarthTools

				//auto a = log(tan(p2.latitude.getValue(rad) / 2 + ARL::Math::pi / 4)
				//	/ tan(p1.latitude.getValue(rad) / 2 + pi / 4));

				//auto b = abs(p1.longitude.getValue(rad) - p2.longitude.getValue(rad));

				//if (b > 180.0F)
				//	b = fmod(b, 180.0F);

				//return ARL::Math::Angle(atan2(b, a), ARL::Math::Radians);

//				auto deltaLat = p2.latitude.getValue(rad) - latitude.getValue(rad);
				auto deltaLon = p2.longitude.getValue(rad) - longitude.getValue(rad);

				auto bearing = atan2(
					sin(deltaLon) * cos(p2.latitude.getValue(rad)),
					cos(latitude.getValue(rad)) * sin(p2.latitude.getValue(rad)) -
					sin(latitude.getValue(rad)) * cos(p2.latitude.getValue(rad)) *
					cos(deltaLon)
					);

				ARL::Math::Angle angle(bearing, rad);
				auto deg = angle.getValue(ARL::Math::Degrees);

				deg = fmod(deg + 360, 360);
				return ARL::Math::Angle(deg, ARL::Math::Degrees);
			}
			Angle GeodeticToEgocentricBearing(const Orientation1D &orientation, const GeoCoordinate &point) const {

				auto heading = orientation.getValue(ARL::Math::AngleUnits::Degrees).yaw;
				heading = 90 - heading;
				auto bearing = ForwardAzimuth(point);
				auto deltaTheta = bearing.getValue(ARL::Math::Degrees) - heading;

				if (-180 < deltaTheta && deltaTheta <= 180)
					return ARL::Math::Angle(deltaTheta, ARL::Math::Degrees);
				else if (deltaTheta > 180)
					return ARL::Math::Angle(-360 + deltaTheta, ARL::Math::Degrees);
				else
					return ARL::Math::Angle(360 + deltaTheta, ARL::Math::Degrees);
			}

			PolarCoordinate GeodeticToEgocentric(const Orientation1D &orientation, const GeoCoordinate &point) const {
				return PolarCoordinate(GeodeticToEgocentricBearing(orientation, point), GreatCircleDistance(point));
			}

		};

		class Velocity3D : public ScaledValue < Point3D, SpeedUnits > {
		public:
			Velocity3D(double x = 0, double y = 0, double z = 0, SpeedUnits u = MetersPerSecond) {
				setValue( Point3D(x, y, z), u);
			}
			Velocity3D(const Point3D& value, SpeedUnits u = MetersPerSecond) {
				setValue( value, u);
			}
			Velocity3D(const DOMElement* pElement) : ScaledValue<Point3D, SpeedUnits>(pElement) {
				units = Utility::DOMHelper::parseDOMAttribute<SpeedUnits>(pElement, xStr("units"));
			}
			void setValue(const Point3D& v, SpeedUnits u = MetersPerSecond) { //always store internally in MetersPerSecond, preferred units stored in Units
				units = u;
				switch (u) {
				case MetersPerSecond: magnitude = v; break;
				case Knots: magnitude = nauticalMilesToMeters(v * 3600); break;
				case FeetPerSecond: magnitude = feetToMeters(v); break;
				case MilesPerHour: magnitude = milesToMeters(v * 3600); break;
				case KilometersPerHour: magnitude = 1000 * v * 3600; break;
				default: throw Exception("unknown distance units in Speed::setValue()");
				}
			}
			Point3D getValue(const SpeedUnits u = MetersPerSecond) const override {
				switch (u)  {
				case MetersPerSecond: return magnitude;
				case Knots: return metersToNauticalMiles(magnitude / 3600);
				case FeetPerSecond: return metersToFeet(magnitude);
				case MilesPerHour: return metersToMiles(magnitude / 3600);
				case KilometersPerHour: return magnitude / 3600000;
				default: throw Exception("unknown distance units in Speed::getValue()");
				}
			}

			Speed<> getSpeed() const { return Speed<>(getValueInPreferredUnits().magnitude(), units); }

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Velocity3D")) const override {
				DOMElement* pElement = magnitude.createDOMElement(pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("units"));
			}

			Velocity3D filteredUpdate(const Velocity3D& v, const double alpha = 1.0 / 4) {
				magnitude.filteredUpdate(v.magnitude, alpha);
				return *this;
			}

			Velocity3D& operator+=(const Velocity3D& v) {
				magnitude += v.magnitude;
				return *this;
			}

			friend class cereal::access;
		};

		class Acceleration3D : public ScaledValue < Point3D, AccelerationUnits> {
		public:
			Acceleration3D(double x = 0, double y = 0, double z = 0, AccelerationUnits u = MetersPerSecondSquared) {
				setValue(Point3D(x, y, z), u);
			}
			Acceleration3D(const Point3D& value, AccelerationUnits u = MetersPerSecondSquared) {
				setValue(value, u);
			}
			Acceleration3D(const DOMElement* pElement) : ScaledValue<Point3D, AccelerationUnits>(pElement) {
				units = Utility::DOMHelper::parseDOMAttribute<AccelerationUnits>(pElement, xStr("units"));
			}

			void setValue(const Point3D& v, AccelerationUnits u = MetersPerSecondSquared) { //always store internally in MetersPerSecond, preferred units stored in Units
				units = u;
				switch (u) {
				case MetersPerSecondSquared: magnitude = v; break;
				case FeetPerSecondSquared: magnitude = feetToMeters(v); break;
				default: throw Exception("unknown distance units in Acceleration3D::setValue()");
				}
			}
			Point3D getValue(const AccelerationUnits u = MetersPerSecondSquared) const override {
				switch (u)  {
				case MetersPerSecondSquared: return magnitude;
				case FeetPerSecondSquared: return metersToFeet(magnitude);
				default: throw Exception("unknown distance units in Acceleration3D::getValue()");
				}
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Acceleration3D")) const override {
				DOMElement* pElement = magnitude.createDOMElement(pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("units"));
			}

			Acceleration3D filteredUpdate(const Acceleration3D& v, const double alpha = 1.0 / 4) {
				magnitude.filteredUpdate(v.magnitude, alpha);
				return *this;
			}

		protected:
			friend ostream& operator<<(ostream& o, const Acceleration3D& a) {
				o << a.getValueInPreferredUnits() << " " << AccelerationUnitHelper::ToString(a.units);
				return o;
			}

			friend class cereal::access;
		};

		class Velocity2D : public ScaledValue < Point2D, SpeedUnits > {
		public:
			Velocity2D(double x = 0, double y = 0, SpeedUnits u = MetersPerSecond) {
				setValue( Point2D(x, y), u );
			}
			Velocity2D(const Point2D& value, SpeedUnits u = MetersPerSecond) {
				setValue( value, u );
			}
			Velocity2D(const Velocity3D& value) {
				setValue(value.getValueInPreferredUnits(), value.getPreferredUnits());
			}
			Velocity2D(const DOMElement* pElement) : ScaledValue<Point2D, SpeedUnits>(pElement) {
				units = Utility::DOMHelper::parseDOMAttribute<SpeedUnits>(pElement, xStr("units"));
			}
			void setValue(const Point2D& v, SpeedUnits u = MetersPerSecond) { //always store internally in MetersPerSecond, preferred units stored in Units
				units = u;
				switch (u) {
				case MetersPerSecond: magnitude = v; break;
				case Knots: magnitude = nauticalMilesToMeters(v * 3600); break;
				case FeetPerSecond: magnitude = feetToMeters(v); break;
				case MilesPerHour: magnitude = milesToMeters(v * 3600); break;
				case KilometersPerHour: magnitude = 1000 * v * 3600; break;
				default: throw Exception("unknown distance units in Speed::setValue()");
				}
			}
			Point2D getValue(const SpeedUnits u = MetersPerSecond) const override {
				switch (u)  {
				case MetersPerSecond: return magnitude;
				case Knots: return metersToNauticalMiles(magnitude / 3600);
				case FeetPerSecond: return metersToFeet(magnitude);
				case MilesPerHour: return metersToMiles(magnitude / 3600);
				case KilometersPerHour: return magnitude / 3600000;
				default: throw Exception("unknown distance units in Speed::getValue()");
				}
			}

			Speed<> getSpeed() const { return Speed<>(getValueInPreferredUnits().magnitude(), units); }

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Velocity2D")) const override {
				DOMElement* pElement = magnitude.createDOMElement(pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("units"));
			}

			Velocity2D filteredUpdate(const Velocity2D& v, const double alpha = 1.0 / 4) {
				magnitude.filteredUpdate(v.magnitude, alpha);
				return *this;
			}

			static Velocity2D getNotAVelocity() { return Velocity2D(Point2D(numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN()), MetersPerSecond); }

			friend class cereal::access;
		};

		struct GeoCoordinate3D : public GeoCoordinate {
			GeoCoordinate3D(const Latitude& lat = Latitude(40.7926733),
				const Longitude& lon = Longitude(-77.8659007),
				Altitude alt = 370) : GeoCoordinate(lat, lon), altitude(alt) {}

			GeoCoordinate3D(const GeoCoordinate &geo, const Altitude &alt = 0.0) 
				: GeoCoordinate(geo), altitude(alt) { }

			GeoCoordinate3D(const GeoCoordinate3D& geo) : GeoCoordinate(geo), altitude(geo.altitude) { }

			GeoCoordinate3D(const DOMElement* pElement)
				: GeoCoordinate(pElement), altitude(pElement->getFirstElementChild()->getNextElementSibling()->getNextElementSibling()) { }

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr("GeoCoordinate3D")) const override {
				DOMElement* pElement = GeoCoordinate::createDOMElement(pDoc, name);
				pElement->appendChild(altitude.createDOMElement(pDoc, xStr(L"altitude")));
				return pElement;
			}

			Altitude altitude;

			GeoCoordinate3D& filteredUpdate(const GeoCoordinate3D& geoCoordinate, const double alpha = 1.0/3) {
				GeoCoordinate::filteredUpdate(geoCoordinate, alpha);
				altitude.filteredUpdate(geoCoordinate.altitude, alpha);
				return *this;
			}

			friend ostream& operator<<(ostream& o, const GeoCoordinate3D& c) {
				o << c.latitude.getValue(Degrees) << ',' << c.longitude.getValue(Degrees) << ',' << c.altitude.getValue(Meters);
				return o;
			}

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<GeoCoordinate>(this), altitude);
			}
		};

		class AngularVelocity3D : public ScaledValue < EulerAngles3D, AngularSpeedUnits > {
		public:
			AngularVelocity3D(double roll = 0, double pitch = 0, double yaw = 0, AngularSpeedUnits u = RadiansPerSecond) {
				setValue(EulerAngles3D(roll, pitch, yaw), u);
			}
			AngularVelocity3D(const EulerAngles3D& value, AngularSpeedUnits u = RadiansPerSecond) {
				setValue(value, u);
			}
			AngularVelocity3D(const DOMElement* pElement) : ScaledValue < EulerAngles3D, AngularSpeedUnits >(pElement) {
				units = Utility::DOMHelper::parseDOMAttribute<AngularSpeedUnits>(pElement, xStr("units"));
				setValue(magnitude, units);
			}
			void setValue(const EulerAngles3D& v, AngularSpeedUnits u = RadiansPerSecond) {
				units = u;
				switch (u) {
				case RadiansPerSecond: magnitude = v; break;
				case DegreesPerSecond: magnitude = degreesToRadians(v); break;
				default: throw Exception("Unknown Angle Units in AngularVelocity3D::setValue()");
				}
			}

			EulerAngles3D getValue(const AngularSpeedUnits u = RadiansPerSecond) const override {
				switch (u) {
				case RadiansPerSecond: return magnitude;
				case DegreesPerSecond: return radiansToDegrees(magnitude); break;
				default: throw Exception("Unknown Angle Units in AngularVelocity3D::getValue()");
				}
			}

			void scaleValue(const EulerAngles3D& scale) {
				magnitude.roll *= scale.roll;
				magnitude.pitch *= scale.pitch;
				magnitude.yaw *= scale.yaw;
			}

			AngularVelocity3D& filteredUpdate(const AngularVelocity3D& o, const double alpha = 1.0 / 3) {
				this->magnitude = ARL::Filter::InfiniteImpulseResponseDigitalFilter<EulerAngles3D>::computeResponse(o.magnitude, this->magnitude, alpha);
				return *this;
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"AngularVelocity3D")) const override {
				DOMElement* pElement = magnitude.createDOMElement(pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("units"));
			}

		protected:
			friend AngularVelocity3D operator*(const AngularVelocity3D& s, double a){
				return AngularVelocity3D(s.magnitude*a, s.units);
			}
			friend AngularVelocity3D operator*(double a, const AngularVelocity3D& s){
				return AngularVelocity3D(s.magnitude*a, s.units);
			}
			friend AngularVelocity3D operator/(const AngularVelocity3D& s, double a){
				return AngularVelocity3D(s.magnitude / a, s.units);
			}
			friend AngularVelocity3D& operator*=(AngularVelocity3D& s, double a){
				s.magnitude *= a;
				return s;
			}
			friend AngularVelocity3D& operator/=(AngularVelocity3D& s, double a){
				s.magnitude /= a;
				return s;
			}
			friend ostream& operator<<(ostream& o, const AngularVelocity3D& v) {
				o << v.magnitude << " " << AngularSpeedUnitHelper::ToString(v.units);
				return o;
			}

			friend class cereal::access;
		};

		class AngularVelocity1D : public ScaledValue < EulerAngles1D, AngularSpeedUnits > {
		public:
			AngularVelocity1D(double yaw = 0, AngularSpeedUnits u = RadiansPerSecond) {
				setValue(EulerAngles1D(yaw), u);
			}
			AngularVelocity1D(const EulerAngles1D& value, AngularSpeedUnits u = RadiansPerSecond) {
				setValue(value, u);
			}
			void setValue(const EulerAngles1D& v, AngularSpeedUnits u = RadiansPerSecond) {
				units = u;
				switch (u) {
				case RadiansPerSecond: magnitude = v; break;
				case DegreesPerSecond: magnitude = degreesToRadians(v); break;
				default: throw Exception("Unknown Angle Units in AngularVelocity1D::setValue()");
				}
			}

			EulerAngles1D getValue(const AngularSpeedUnits u = RadiansPerSecond) const override {
				switch (u) {
				case RadiansPerSecond: return magnitude;
				case DegreesPerSecond: return radiansToDegrees(magnitude); break;
				default: throw Exception("Unknown Angle Units in AngularVelocity1D::getValue()");
				}
			}

			AngularVelocity1D& filteredUpdate(const AngularVelocity1D& o, const double alpha = 1.0 / 3) {
				this->magnitude = ARL::Filter::InfiniteImpulseResponseDigitalFilter<EulerAngles1D>::computeResponse(o.magnitude, this->magnitude, alpha);
				return *this;
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = Utility::xStr("AngularVelocity3D")) const override {
				DOMElement* pElement = magnitude.createDOMElement(pDoc, name);
				return Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("units"));
			}

		protected:
			friend AngularVelocity1D operator*(const AngularVelocity1D& s, double a){
				return AngularVelocity1D(s.magnitude*a, s.units);
			}
			friend AngularVelocity1D operator*(double a, const AngularVelocity1D& s){
				return AngularVelocity1D(s.magnitude*a, s.units);
			}
			friend AngularVelocity1D operator/(const AngularVelocity1D& s, double a){
				return AngularVelocity1D(s.magnitude / a, s.units);
			}

			friend class cereal::access;
		};

		struct LocalizationError : public XmlSerializable {
			LocalizationError(double sigLat = 0, double sigLon = 0, double sigAlt = 0, double sigRoll = 0, double sigPitch = 0, double sigYaw = 0) {
				sigmaLatitude = sigLat; sigmaLongitude = sigLon; sigmaAltitude = sigAlt;
				sigmaRoll = sigRoll; sigmaPitch = sigPitch; sigmaYaw = sigYaw;
			}
			LocalizationError(const DOMElement* pElement) {
				sigmaLatitude = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaLatitude"))->item(0)));
				sigmaLongitude = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaLongitude"))->item(0)));
				sigmaAltitude = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaAltitude"))->item(0)));
				sigmaRoll = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaRoll"))->item(0)));
				sigmaPitch = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaPitch"))->item(0)));
				sigmaYaw = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaYaw"))->item(0)));
			}

			double magnitude2D() const { return sqrt(sigmaLatitude*sigmaLatitude + sigmaLongitude*sigmaLongitude); }
			double magnitude3D() const { return sqrt(sigmaLatitude*sigmaLatitude + sigmaLongitude*sigmaLongitude + sigmaAltitude*sigmaAltitude); }

			double sigmaLatitude, sigmaLongitude, sigmaAltitude, sigmaRoll, sigmaPitch, sigmaYaw;

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"LocalizationError")) const override {
				DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaLatitude, pDoc, xStr(L"sigmaLatitude")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaLongitude, pDoc, xStr(L"sigmaLongitude")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaAltitude, pDoc, xStr(L"sigmaAltitude")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaRoll, pDoc, xStr(L"sigmaRoll")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaPitch, pDoc, xStr(L"sigmaPitch")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaYaw, pDoc, xStr(L"sigmaYaw")));
				return pElement;
			}

			friend LocalizationError& operator+=(LocalizationError& e, const LocalizationError& e1) {
				e.sigmaLatitude += e1.sigmaLatitude;
				e.sigmaLongitude += e1.sigmaLongitude;
				e.sigmaAltitude += e1.sigmaAltitude;
				e.sigmaRoll += e1.sigmaRoll;
				e.sigmaPitch += e1.sigmaPitch;
				e.sigmaYaw += e1.sigmaYaw;
				return e;
			}

			friend LocalizationError operator+(LocalizationError& e0, const LocalizationError& e1) {
				LocalizationError e = e0;
				e.sigmaLatitude += e1.sigmaLatitude;
				e.sigmaLongitude += e1.sigmaLongitude;
				e.sigmaAltitude += e1.sigmaAltitude;
				e.sigmaRoll += e1.sigmaRoll;
				e.sigmaPitch += e1.sigmaPitch;
				e.sigmaYaw += e1.sigmaYaw;
				return e;
			}

			friend LocalizationError& operator-=(LocalizationError& e, const LocalizationError& e1) {
				e.sigmaLatitude -= e1.sigmaLatitude;
				e.sigmaLongitude -= e1.sigmaLongitude;
				e.sigmaAltitude -= e1.sigmaAltitude;
				e.sigmaRoll -= e1.sigmaRoll;
				e.sigmaPitch -= e1.sigmaPitch;
				e.sigmaYaw -= e1.sigmaYaw;
				return e;
			}

			friend LocalizationError operator-(LocalizationError& e0, const LocalizationError& e1) {
				LocalizationError e = e0;
				e.sigmaLatitude -= e1.sigmaLatitude;
				e.sigmaLongitude -= e1.sigmaLongitude;
				e.sigmaAltitude -= e1.sigmaAltitude;
				e.sigmaRoll -= e1.sigmaRoll;
				e.sigmaPitch -= e1.sigmaPitch;
				e.sigmaYaw -= e1.sigmaYaw;
				return e;
			}

			LocalizationError& filteredUpdate(const LocalizationError& error, const double alpha = 1.0 / 4) {
				sigmaLatitude = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaLatitude, sigmaLatitude, alpha);
				sigmaLongitude = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaLongitude, sigmaLongitude, alpha);
				sigmaAltitude = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaAltitude, sigmaAltitude, alpha);
				sigmaRoll = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaRoll, sigmaRoll, alpha);
				sigmaPitch = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaPitch, sigmaPitch, alpha);
				sigmaYaw = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaYaw, sigmaYaw, alpha);
				return *this;
			}

			friend LocalizationError operator*(const LocalizationError& error, const double& x) {
				LocalizationError e = error;
				e.sigmaLatitude *= x;
				e.sigmaLongitude *= x;
				e.sigmaAltitude *= x;
				e.sigmaRoll *= x;
				e.sigmaPitch *= x;
				e.sigmaYaw *= x;
				return e;
			}

			friend LocalizationError operator*(const double& x, const LocalizationError& error) {
				LocalizationError e = error;
				e.sigmaLatitude *= x;
				e.sigmaLongitude *= x;
				e.sigmaAltitude *= x;
				e.sigmaRoll *= x;
				e.sigmaPitch *= x;
				e.sigmaYaw *= x;
				return e;
			}

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(sigmaLatitude, sigmaLongitude, sigmaAltitude, sigmaRoll, sigmaPitch, sigmaYaw);
			}
		};

		struct VelocityError : public XmlSerializable {
			VelocityError(double sigX = 0, double sigY = 0, double sigZ = 0, double sigRollRate = 0, double sigPitchRate = 0, double sigYawRate = 0) {
				sigmaX = sigX; sigmaY = sigY; sigmaZ = sigZ;
				sigmaRollRate = sigRollRate; sigmaPitchRate = sigPitchRate; sigmaYawRate = sigYawRate;
			}
			VelocityError(const DOMElement* pElement) {
				sigmaX = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaX"))->item(0)));
				sigmaY = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaY"))->item(0)));
				sigmaZ = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaZ"))->item(0)));
				sigmaRollRate = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaRollRate"))->item(0)));
				sigmaPitchRate = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaPitchRate"))->item(0)));
				sigmaYawRate = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("sigmaYawRate"))->item(0)));
			}
			double sigmaX, sigmaY, sigmaZ, sigmaRollRate, sigmaPitchRate, sigmaYawRate;

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"LocalizationError")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaX, pDoc, xStr(L"sigmaX")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaY, pDoc, xStr(L"sigmaY")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaZ, pDoc, xStr(L"sigmaZ")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaRollRate, pDoc, xStr(L"sigmaRollRate")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaPitchRate, pDoc, xStr(L"sigmaPitchRate")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement<double>(sigmaYawRate, pDoc, xStr(L"sigmaYawRate")));
				return pElement;
			}

			friend VelocityError& operator+=(VelocityError& e, const VelocityError& e1) {
				e.sigmaX += e1.sigmaX;
				e.sigmaY += e1.sigmaY;
				e.sigmaZ += e1.sigmaZ;
				e.sigmaRollRate += e1.sigmaRollRate;
				e.sigmaPitchRate += e1.sigmaPitchRate;
				e.sigmaYawRate += e1.sigmaYawRate;
				return e;
			}

			friend VelocityError& operator-=(VelocityError& e, const VelocityError& e1) {
				e.sigmaX -= e1.sigmaX;
				e.sigmaY -= e1.sigmaY;
				e.sigmaZ -= e1.sigmaZ;
				e.sigmaRollRate -= e1.sigmaRollRate;
				e.sigmaPitchRate -= e1.sigmaPitchRate;
				e.sigmaYawRate -= e1.sigmaYawRate;
				return e;
			}

			VelocityError& filteredUpdate(const VelocityError& error, const double alpha = 1.0 / 4) {
				sigmaX = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaX, sigmaX, alpha);
				sigmaY = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaY, sigmaY, alpha);
				sigmaZ = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaZ, sigmaZ, alpha);
				sigmaRollRate = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaRollRate, sigmaRollRate, alpha);
				sigmaPitchRate = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaPitchRate, sigmaPitchRate, alpha);
				sigmaYawRate = InfiniteImpulseResponseDigitalFilter<>::computeResponse(error.sigmaYawRate, sigmaYawRate, alpha);
				return *this;
			}

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(sigmaX, sigmaY, sigmaZ, sigmaRollRate, sigmaPitchRate, sigmaYawRate);
			}
		};

		enum GPSFixType : uint8_t {
			NoFix0 = 0,
			NoFix1 = 1,
			Fix2D = 2,
			Fix3D = 3,
			DGPS = 4,
			RTK = 5
		};

		class GPSFixTypeHelper {
		public:
			static string toString(const GPSFixType fix) {
				switch (fix) {
				case NoFix0: return "NoFix0";
				case NoFix1: return "NoFix1";
				case Fix2D: return "Fix2D";
				case Fix3D: return "Fix3D";
				case DGPS: return "DGPS";
				case RTK: return "RTK";
				default: throw Exception("Unknown GPSFixType in toString(GPSFixType)");
				}
			}

			static wstring toWString(const GPSFixType fix) {
				switch (fix) {
				case NoFix0: return L"NoFix0";
				case NoFix1: return L"NoFix1";
				case Fix2D: return L"Fix2D";
				case Fix3D: return L"Fix3D";
				case DGPS: return L"DGPS";
				case RTK: return L"RTK";
				default: throw Exception("Unknown GPSFixType in toWString(GPSFixType)");
				}
			}

			static GPSFixType fromString(const string& s) {
				if (s == "NoFix0") return NoFix0;
				if (s == "NoFix1") return NoFix1;
				if (s == "Fix2D") return Fix2D;
				if (s == "Fix3D") return Fix3D;
				if (s == "DGPS") return DGPS;
				if (s == "RTK") return RTK;
				throw Exception("Unkown string in fromString()");
			}

			static GPSFixType fromString(const wstring& s) {
				if (s == L"NoFix0") return NoFix0;
				if (s == L"NoFix1") return NoFix1;
				if (s == L"Fix2D") return Fix2D;
				if (s == L"Fix3D") return Fix3D;
				if (s == L"DGPS") return DGPS;
				if (s == L"RTK") return RTK;
				throw Exception("Unkown string in fromString()");
			}
		};

		class TimeConstraints : public XmlSerializable {
		public:
			TimeConstraints(const ptime& startingTime = ptime(not_a_date_time), const ptime& endingTime = ptime(not_a_date_time))
				: mStartingTime(startingTime), mEndingTime(endingTime) {
				if (mStartingTime != ptime(not_a_date_time) && mEndingTime != ptime(not_a_date_time) && mStartingTime >= mEndingTime) throw Exception("starting time must come before ending time.");
			}

			TimeConstraints(const DOMAttr* pStartingAttr, const DOMAttr* pEndingAttr) {
				mStartingTime = Utility::DOMHelper::parseDOMAttribute<ptime>(pStartingAttr);
				mEndingTime = Utility::DOMHelper::parseDOMAttribute<ptime>(pEndingAttr);
			}

			/** Constructor to extract DOMAttributeGroup from pElement
			*/
			TimeConstraints(const DOMElement* pElement)
				: TimeConstraints(pElement->getAttributeNode(xStr(L"startingTime")), pElement->getAttributeNode(xStr(L"endingTime"))) {}

			const ptime& getStartingTime() const { return mStartingTime; }
			const ptime& getEndingTime() const { return mEndingTime; }
			void setStartingTime(const ptime& startingTime) {
				if (mEndingTime != ptime(not_a_date_time) && startingTime >= mEndingTime) throw Exception("starting time must come before ending time.");
				mStartingTime = startingTime;
			}
			void setEndingTime(const ptime& endingTime) {
				if (mStartingTime != ptime(not_a_date_time) && mStartingTime >= endingTime) throw Exception("starting time must come before ending time.");
				mEndingTime = endingTime;
			}
			bool hasStartingTime() const { return mStartingTime != ptime(not_a_date_time); }
			bool hasEndingTime() const { return mEndingTime != ptime(not_a_date_time); }

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"TimeConstraints")) const override {
				DOMElement* pElement = pDoc->createElement(name);
				if (hasStartingTime()) 
					pElement->appendChild(Utility::DOMHelper::createDOMElement(mStartingTime, pDoc, xStr(L"startingTime")));
				if (hasEndingTime()) 
					pElement->appendChild(Utility::DOMHelper::createDOMElement(mEndingTime, pDoc, xStr(L"endingTime")));
				return pElement;
			}

			DOMElement* setDOMAttributeGroup(DOMElement* pElement) const {
				if (hasStartingTime()) 
					Utility::DOMHelper::setDOMAttribute(mStartingTime, pElement, xStr(L"startingTime"));
				if (hasEndingTime())
					Utility::DOMHelper::setDOMAttribute(mEndingTime, pElement, xStr(L"endingTime"));
				return pElement;
			}

			void timeTranslate(const boost::posix_time::time_duration& deltaT) {
				if (hasStartingTime())
					mStartingTime += deltaT;
				if (hasEndingTime())
					mEndingTime += deltaT;
			}

		protected:
			ptime mStartingTime;
			ptime mEndingTime;

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(mStartingTime, mEndingTime);
			}
		};

		class ExpectedTimeConstraints : public TimeConstraints {
		public:
			ExpectedTimeConstraints(const ptime& expectedTime = ptime(not_a_date_time), const ptime& startingTime = ptime(not_a_date_time), const ptime& endingTime = ptime(not_a_date_time))
				: TimeConstraints(startingTime, endingTime), mExpectedTime(expectedTime) {
				if (hasExpectedTime()) {
					if (hasStartingTime() && mExpectedTime < mStartingTime) throw Exception("expected time must be no earlier than starting time.");
					if (hasEndingTime() && mExpectedTime > mEndingTime) throw Exception("expected time must come before ending time.");
				}
			}

			ExpectedTimeConstraints(const TimeConstraints& tc, const ptime& expectedTime = ptime(not_a_date_time))
				: TimeConstraints(tc), mExpectedTime(expectedTime) {
				if (hasExpectedTime()) {
					if (hasStartingTime() && mExpectedTime < mStartingTime) throw Exception("expected time must be no earlier than starting time.");
					if (hasEndingTime() && mExpectedTime > mEndingTime) throw Exception("expected time must come before ending time.");
				}
			}

			ExpectedTimeConstraints(const DOMAttr* pExpectedAttr, const DOMAttr* pStartingAttr, const DOMAttr* pEndingAttr)
				: TimeConstraints(pStartingAttr, pEndingAttr), mExpectedTime(Utility::DOMHelper::parseDOMAttribute<ptime>(pExpectedAttr)) { }

			const ptime& getExpectedTime() const { return mExpectedTime; }
			void setExpectedTime(const ptime& expectedTime) {
				mExpectedTime = expectedTime;
				if (hasStartingTime() && mExpectedTime < mStartingTime) throw Exception("expected time must be no earlier than starting time.");
				if (hasEndingTime() && mEndingTime < mExpectedTime) throw Exception("expected time must come before ending time.");
			}

			bool hasExpectedTime() const { return mExpectedTime != ptime(not_a_date_time); }

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"ExpectedTimeConstraints")) const override {
				DOMElement* pElement = TimeConstraints::createDOMElement(pDoc, name);
				pElement->appendChild(Utility::DOMHelper::createDOMElement(mExpectedTime, pDoc, xStr(L"expectedTime")));
				return pElement;
			}

			DOMElement* setDOMAttributeGroup(DOMElement* pElement) const {
				return TimeConstraints::setDOMAttributeGroup(Utility::DOMHelper::setDOMAttribute(mExpectedTime, pElement, xStr(L"expectedTime")));
			}

			void timeTranslate(const boost::posix_time::time_duration& deltaT) {
				TimeConstraints::timeTranslate(deltaT);
				if (hasExpectedTime())
					mExpectedTime += deltaT;
			}

		protected:
			ptime mExpectedTime;

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<TimeConstraints>(this), mExpectedTime);
			}
		};

		class PolarVelocity {
		public:
			PolarVelocity(const AngularVelocity1D& bearingRate, const Speed<>& distanceRate) : bearingRate(bearingRate), distanceRate(distanceRate) {}
			///
			///@param deltaT in seconds
			PolarVelocity(const PolarCoordinate& newCoord, const PolarCoordinate& oldCoord, const double deltaT) {
				distanceRate.setValue((newCoord.getDistance().getValue(Meters) - oldCoord.getDistance().getValue(Meters)) / deltaT, MetersPerSecond);
				Coordinate2D newCart = newCoord.toCartesian();
				Coordinate2D oldCart = oldCoord.toCartesian();
				auto bearingDegrees = newCoord.getAzimuth().getValue(Degrees);
				if (signbit(newCart.getValue(Meters).x) && signbit(oldCart.getValue(Meters).x) // if both x and xPrev values are negative
					&& signbit(newCart.getValue(Meters).y) != signbit(oldCart.getValue(Meters).y)) // AND signs of y and yPrev are different 
				{ // bearing crosses discontinuity at bearing = 180 = -180
					//un-normalize filteredBearing
					bearingDegrees += bearingDegrees < 0 ? 360 : -360;
				}
				bearingRate.setValue((bearingDegrees - oldCoord.getAzimuth().getValue(Degrees)) / deltaT, DegreesPerSecond);
			}

			const AngularVelocity1D getBearingRate() const { return bearingRate; }
			const Speed<> getDistanceRate() const { return distanceRate; }
		protected:
			AngularVelocity1D bearingRate;
			Speed<> distanceRate;

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(bearingRate, distanceRate);
			}
		};



	}
}

