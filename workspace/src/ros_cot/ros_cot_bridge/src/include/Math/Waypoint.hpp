#pragma once

#include "Geometry.hpp"
#include <vector>
#include <cereal/types/string.hpp>
#include <Utility/NamedEntity.hpp>
#include <Messaging/XmlSerializable.hpp>

namespace ARL {
	namespace Database { class NavPlanLogger; } //forward declare for friending
	namespace PathPlanning { //forward declare
		template <class T> class NavPlan;
	}

	namespace Math {
		using namespace std;

		template <class T>
		class Waypoint : public T, public Utility::NamedEntity {
		public:
			Waypoint(const T& point, const ptime& expectedTime, const wstring& id = L"(anonymous)")
				: T(point), Utility::NamedEntity(id), mTimeConstraints(expectedTime) {
			}
			Waypoint(const T& point, const ptime& expectedTime, const ptime& endingTime, const wstring& id = L"(anonymous)")
				: T(point), Utility::NamedEntity(id), mTimeConstraints(expectedTime, ptime(not_a_date_time), endingTime) {
			}
			Waypoint(const T& point, const ptime& expectedTime, const ptime& startingTime, const ptime& endingTime, const wstring& id = L"(anonymous)")
				: T(point), Utility::NamedEntity(id), mTimeConstraints(expectedTime, startingTime, endingTime) {
			}
			Waypoint(const T& point, const ExpectedTimeConstraints& tc, const wstring& id = L"(anonymous)")
				: T(point), Utility::NamedEntity(id), mTimeConstraints(tc) {}
			Waypoint(const XERCES_CPP_NAMESPACE::DOMElement* pElement)
				: T(pElement),
				mTimeConstraints(Utility::DOMHelper::parseDOMAttribute<ptime>(pElement, Utility::xStr(L"expectedTime")),
								Utility::DOMHelper::parseDOMAttribute<ptime>(pElement, Utility::xStr(L"startingTime")),
								Utility::DOMHelper::parseDOMAttribute<ptime>(pElement, Utility::xStr(L"endingTime"))) {
				mIdentifier = Utility::aStr(pElement->getAttribute(Utility::xStr(L"Identifier")));
			}

			const ExpectedTimeConstraints& getTimeConstraints() const { return mTimeConstraints; }

			void setTimeConstraints(ExpectedTimeConstraints& tc) { 
				mTimeConstraints.setEndingTime(tc.getEndingTime());
				mTimeConstraints.setStartingTime(tc.getStartingTime());
				mTimeConstraints.setExpectedTime(tc.getExpectedTime()); 
			}

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"Waypoint")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = T::createDOMElement(pDoc, name);
				return setDOMAttributeGroup(pElement);
			}

			void timeTranslate(const boost::posix_time::time_duration& deltaT) {
				mTimeConstraints.timeTranslate(deltaT);
			}

		protected:
			XERCES_CPP_NAMESPACE::DOMElement* setDOMAttributeGroup(XERCES_CPP_NAMESPACE::DOMElement* pElement) const {
				pElement->setAttribute(Utility::xStr(L"Identifier"), Utility::xStr(mIdentifier.c_str()));
				return mTimeConstraints.setDOMAttributeGroup(pElement);
			}

			ExpectedTimeConstraints mTimeConstraints;

			Waypoint() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<T>(this), mIdentifier, mTimeConstraints);
			}
		};

		class Waypoint2D : public Waypoint < GeoCoordinate > {
		public:
			Waypoint2D(const GeoCoordinate& point = GeoCoordinate(), const ptime& expectedTime = ptime(not_a_date_time), const wstring& id = L"(anonymous)")
				: Waypoint<GeoCoordinate>(point, expectedTime, id) {}
			Waypoint2D(const GeoCoordinate& point, const ptime& expectedTime, const ptime& endingTime, const wstring& id = L"(anonymous)")
				: Waypoint<GeoCoordinate>(point, expectedTime, endingTime, id) {}
			Waypoint2D(const GeoCoordinate& point, const ptime& expectedTime, const ptime& startingTime, const ptime& endingTime, const wstring& id = L"(anonymous)")
				: Waypoint<GeoCoordinate>(point, expectedTime, startingTime, endingTime, id) {}
			Waypoint2D(const GeoCoordinate& point, const ExpectedTimeConstraints& tc, const wstring& id = L"(anonymous)")
				: Waypoint<GeoCoordinate>(point, tc, id) {}
			Waypoint2D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Waypoint < GeoCoordinate >(pElement) { }
		protected:
		};

		class Waypoint3D : public Waypoint < GeoCoordinate3D > {
		public:
			Waypoint3D(const GeoCoordinate3D& point = GeoCoordinate3D(), const ptime& expectedTime = ptime(not_a_date_time), const wstring& id = L"(anonymous)")
				: Waypoint<GeoCoordinate3D>(point, expectedTime, id) {}
			Waypoint3D(const GeoCoordinate3D& point, const ptime& expectedTime, const ptime& endingTime, const wstring& id = L"(anonymous)")
				: Waypoint<GeoCoordinate3D>(point, expectedTime, endingTime, id) {}
			Waypoint3D(const GeoCoordinate3D& point, const ptime& expectedTime, const ptime& startingTime, const ptime& endingTime, const wstring& id = L"(anonymous)")
				: Waypoint<GeoCoordinate3D>(point, expectedTime, startingTime, endingTime, id) {}
			Waypoint3D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Waypoint < GeoCoordinate3D >(pElement) { }
			operator Waypoint2D() const { return Waypoint2D(GeoCoordinate(*this), mTimeConstraints, mIdentifier); }
		protected:
		};

		class SphericalWaypoint : public Waypoint < SphericalCoordinate > {
		public:
			SphericalWaypoint(const SphericalCoordinate& point, const ptime& expectedTime = ptime(not_a_date_time), const wstring& id = L"(anonymous)")
				: Waypoint<SphericalCoordinate>(point, expectedTime, id) {}
			SphericalWaypoint(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Waypoint < SphericalCoordinate >(pElement) { }
		protected:
		};

		class CylindricalWaypoint : public Waypoint < CylindricalCoordinate > {
		public:
			CylindricalWaypoint(const CylindricalCoordinate& point, const ptime& expectedTime = ptime(not_a_date_time), const wstring& id = L"(anonymous)")
				: Waypoint<CylindricalCoordinate>(point, expectedTime, id) {}
			CylindricalWaypoint(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Waypoint < CylindricalCoordinate >(pElement) { }
		protected:
		};

	}
}