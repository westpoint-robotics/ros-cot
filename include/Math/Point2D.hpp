#pragma once

#include <Eigen/Dense>
#include <Messaging/XmlSerializable.hpp>
#include <Filter/InfiniteImpulseResponseDigitalFilter.hpp>
#include <Utility/DOMHelper.hpp>
#include <Utility/xstr.hpp>

namespace ARL {
	namespace Math {

		using XERCES_CPP_NAMESPACE::DOMElement;
		using XERCES_CPP_NAMESPACE::DOMDocument;
		using ARL::Utility::xStr;
		using ARL::Messaging::XmlSerializable;
		using ARL::Filter::InfiniteImpulseResponseDigitalFilter;

		using namespace std;

		struct Point2D : public XmlSerializable {
			Point2D(double x = 0, double y = 0) { this->x = x; this->y = y; }
			Point2D(const Eigen::Vector2d& v)  {
				x = v.x(); y = v.y();
			}
			Point2D(const DOMElement* pElement) {
				x = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("x"))->item(0)));
				y = Utility::DOMHelper::parseDOMElement<double>(static_cast<DOMElement*>(pElement->getElementsByTagName(xStr("y"))->item(0)));
			}
			double x, y;

			double magnitude() const { return sqrt(x*x + y*y); }
			Point2D normalize() const { return *this / magnitude(); }
			Point2D& normalize() { *this /= magnitude(); return *this; }
			double distance(const Point2D& point) const { return sqrt((x - point.x) * (x - point.x) + (y - point.y) * (y - point.y)); }

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr("Point2D")) const override {
				DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(Utility::DOMHelper::createDOMElement(x, pDoc, xStr("x")));
				pElement->appendChild(Utility::DOMHelper::createDOMElement(y, pDoc, xStr("y")));
				return pElement;
			}

			friend Point2D operator+(const Point2D& p1, const Point2D& p2) {
				return Point2D(p1.x + p2.x, p1.y + p2.y);
			}
			friend Point2D operator-(const Point2D& p1, const Point2D& p2) {
				return Point2D(p1.x - p2.x, p1.y - p2.y);
			}
			friend Point2D operator*(const Point2D& p, double v) {
				return Point2D(p.x*v, p.y*v);
			}
			friend Point2D operator*(double v, const Point2D& p) {
				return Point2D(p.x*v, p.y*v);
			}

			Point2D& operator*=(double v) {
				x *= v; y *= v;
				return *this;
			}

			friend Point2D operator/(const Point2D& p, double v) {
				return Point2D(p.x / v, p.y / v);
			}
			friend Point2D& operator/=(Point2D& p, double v) {
				p.x /= v; p.y /= v;
				return p;
			}

			Point2D filteredUpdate(const Point2D& p, const double alpha = 1.0 / 4) {
				x = InfiniteImpulseResponseDigitalFilter<>::computeResponse(p.x, x, alpha);
				y = InfiniteImpulseResponseDigitalFilter<>::computeResponse(p.y, y, alpha);
				return *this;
			}

			operator Eigen::Vector2d() const {
				return Eigen::Vector2d(x, y);
			}

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(x, y);
			}
		};

	}
}