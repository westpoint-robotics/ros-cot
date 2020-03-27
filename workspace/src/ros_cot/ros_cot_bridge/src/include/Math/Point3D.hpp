#pragma once

#include "Point2D.hpp"
#include "cereal/cereal.hpp"

namespace ARL {
	namespace Math {

		struct Point3D : public Point2D {
			Point3D(double x = 0, double y = 0, double z = 0) : Point2D(x, y) {
				this->z = z;
			}
			Point3D(const Point2D& p, const double z = 0) : Point2D(p), z(z) { }
			Point3D(const Eigen::Vector3d& v) : Point2D(v.x(), v.y()) {
				z = v.z();
			}
			Point3D(const DOMElement* pElement) : Point2D(pElement) {
				z = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("z"))->item(0)));
			}
			double z;

			double magnitude() const { return sqrt(x*x + y*y + z*z); }
			Point3D normalize() const { return *this / magnitude(); }
			Point3D& normalize() { *this /= magnitude(); return *this; }

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr("Point2D")) const override {
				DOMElement* pElement = Point2D::createDOMElement(pDoc, name);
				pElement->appendChild(Utility::DOMHelper::createDOMElement(z, pDoc, xStr("z")));
				return pElement;
			}

			friend Point3D operator+(const Point3D& p1, const Point3D& p2) {
				return Point3D(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
			}
			friend Point3D operator-(const Point3D& p1, const Point3D& p2) {
				return Point3D(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
			}
			Point3D& operator+=(const Point3D& p) {
				x += p.x;
				y += p.y;
				z += p.z;
				return *this;
			}

			Point3D& operator-=(const Point3D& p) {
				x -= p.x;
				y -= p.y;
				z -= p.z;
				return *this;
			}

			friend Point3D operator*(const Point3D& p, double v) {
				return Point3D(p.x*v, p.y*v, p.z*v);
			}
			friend Point3D operator*(double v, const Point3D& p) {
				return Point3D(p.x*v, p.y*v, p.z*v);
			}
			Point3D& operator*=(double v) {
				x *= v; y *= v; z *= v;
				return *this;
			}
			friend Point3D operator/(const Point3D& p, double v) {
				return Point3D(p.x / v, p.y / v, p.z / v);
			}
			Point3D& operator/=(double v) {
				x /= v; y /= v; z /= v;
				return *this;
			}
			friend ostream& operator<<(ostream& o, const Point3D& p) {
				o << '<' << p.x << ',' << p.y << ',' << p.z << '>';
				return o;
			}

			Point3D filteredUpdate(const Point3D& p, const double alpha = 1.0 / 4) {
				x = InfiniteImpulseResponseDigitalFilter<>::computeResponse(p.x, x, alpha);
				y = InfiniteImpulseResponseDigitalFilter<>::computeResponse(p.y, y, alpha);
				z = InfiniteImpulseResponseDigitalFilter<>::computeResponse(p.z, z, alpha);
				return *this;
			}

			operator Eigen::Vector3d() const {
				return Eigen::Vector3d(x, y, z);
			}

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<Point2D>(this), z);
			}
		};

	}
}