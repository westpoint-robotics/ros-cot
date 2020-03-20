#pragma once

#include "ComplexGeoGeometry.hpp"

namespace ARL {
	namespace Math {
		using namespace std;
		class SpatialConstraints; ///forward declare for friending
		class WarningArea;///forward declare for friending

		class TacticalArea : public ComplexGeoGeometry, public Utility::NamedEntity {
		public:
			TacticalArea(const ComplexGeoGeometry& area, const TimeConstraints& timeConstraints, const wstring& id = L"TacticalArea")
				: ComplexGeoGeometry(area), mTimeConstraints(timeConstraints), Utility::NamedEntity(id) {
			}

			TacticalArea(const XERCES_CPP_NAMESPACE::DOMElement* pElement) :
				ComplexGeoGeometry(pElement),
				mTimeConstraints(pElement->getAttributeNode(Utility::xStr(L"startingTime")), pElement->getAttributeNode(Utility::xStr(L"endingTime")))
			{
				mIdentifier = Utility::aStr(pElement->getAttribute(Utility::xStr(L"Identifier")));
			}

			const TimeConstraints& getTimeConstraints() const { return mTimeConstraints; }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"TacticalArea")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = ComplexGeoGeometry::createDOMElement(pDoc, name);
				pElement->setAttribute(Utility::xStr(L"Identifier"), Utility::xStr(mIdentifier.c_str()));
				return mTimeConstraints.setDOMAttributeGroup(pElement);
			}

			///@returns true iff space-time point within at least one TacticalSegment
			/// this is the sums level of the canonical product-of-sums form of spatial-temporal constraints
			bool within(const ARL::Math::GeoCoordinate& geocoordinate, const boost::posix_time::ptime& time_constraint) const {
				// check for time constraints on the tactical area (rather than its tactical segment components) 
				if ((!mTimeConstraints.getStartingTime().is_special()) || (!mTimeConstraints.getEndingTime().is_special())){
					boost::posix_time::time_period tactical_segment_time_period(mTimeConstraints.getStartingTime(), mTimeConstraints.getEndingTime());
					return tactical_segment_time_period.contains(time_constraint);
				}
				for (const auto& segment : this->mTacticalSegments) 
					if (segment.within(geocoordinate, time_constraint)) return true;
				return false;
			}

		protected:
			TimeConstraints mTimeConstraints;

			void timeTranslate(const boost::posix_time::time_duration& deltaT) {
				mTimeConstraints.timeTranslate(deltaT);
				for (auto& it : mTacticalSegments)
					it.timeTranslate(deltaT);
			}

			TacticalArea() {} // for cereal deserialization
			friend class ARL::Math::SpatialConstraints;
			friend class ARL::Math::WarningArea;
			friend class ARL::PathPlanning::NavPlan<Path2D>;
			friend class ARL::PathPlanning::NavPlan<Path3D>;
			friend class ARL::Database::NavPlanLogger;
			friend class std::vector<TacticalArea>;
			friend class std::allocator<TacticalArea>;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<ComplexGeoGeometry>(this), mIdentifier, mTimeConstraints);
			}
		};

	}
}
