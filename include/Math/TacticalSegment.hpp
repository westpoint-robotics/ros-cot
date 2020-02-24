#pragma once

#include "GeoPolygonalPrism.hpp"
#include <Utility/NamedEntity.hpp>
#include <cereal/types/memory.hpp>

namespace ARL {
	namespace Mission { class MissionObjective; } ///forward declare for friending
	namespace Math {
		using namespace std;
		class TacticalArea; //forward declare for friending

		class TacticalSegment : public GeoPolygonalPrism, public Utility::NamedEntity {
		public:
			TacticalSegment(const GeoPolygon& gp, const Distance<> min = Distance<>::NotADistance(), const Distance<> max = Distance<>::NotADistance(), const wstring& id = L"(unlabeled)", const TimeConstraints& timeConstraints = TimeConstraints())
				: GeoPolygonalPrism(gp, min, max), Utility::NamedEntity(id), mTimeConstraints(timeConstraints) {}
			TacticalSegment(const GeoPolygonalPrism& prism, const wstring& id = L"(unlabeled)", const TimeConstraints& timeConstraints = TimeConstraints())
				: GeoPolygonalPrism(prism), Utility::NamedEntity(id), mTimeConstraints(timeConstraints) {}
//			TacticalSegment(const TacticalSegment& ts)
//				: GeoPolygonalPrism(ts), Utility::NamedEntity(ts), mTimeConstraints(ts.mTimeConstraints) {}
			TacticalSegment(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : GeoPolygonalPrism(pElement), mTimeConstraints(pElement) {
				mIdentifier = Utility::aStr(pElement->getAttribute(Utility::xStr(L"Identifier")));
			}

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"TacticalSegment")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = GeoPolygonalPrism::createDOMElement(pDoc, name);
				pElement->setAttribute(Utility::xStr(L"Identifier"), Utility::xStr(mIdentifier.c_str()));
				return mTimeConstraints.setDOMAttributeGroup(pElement);
			}

			bool within(const ARL::Math::GeoCoordinate& geocoordinate, const boost::posix_time::ptime& time_constraint) const {
				boost::posix_time::time_period tactical_segment_time_period(mTimeConstraints.getStartingTime(), mTimeConstraints.getEndingTime());
				return tactical_segment_time_period.contains(time_constraint) && GeoPolygonalPrism::within(geocoordinate); //evaluate time constraint first since it is constant complexity; spatial constraint is linear complexity
			}

			bool within(const ARL::Math::GeoCoordinate& geocoordinate, const ARL::Math::ExpectedTimeConstraints& time_constraints) const {

				boost::posix_time::time_period tactical_segment_time_period(mTimeConstraints.getStartingTime(), mTimeConstraints.getEndingTime());
				
				if (!time_constraints.getExpectedTime().is_special() && time_constraints.getStartingTime().is_special() && time_constraints.getEndingTime().is_special()){
					return tactical_segment_time_period.contains(time_constraints.getExpectedTime()) && GeoPolygonalPrism::within(geocoordinate);
				}
				else if (!time_constraints.getStartingTime().is_special() || !time_constraints.getEndingTime().is_special()){
					boost::posix_time::time_period start_end_time_period(time_constraints.getStartingTime(), time_constraints.getEndingTime());
					if (time_constraints.getExpectedTime().is_special()){
						return tactical_segment_time_period.intersects(start_end_time_period) && GeoPolygonalPrism::within(geocoordinate);
					}
					else{
						if (start_end_time_period.contains(time_constraints.getExpectedTime()) && tactical_segment_time_period.contains(time_constraints.getExpectedTime())){
							return tactical_segment_time_period.intersects(start_end_time_period) && GeoPolygonalPrism::within(geocoordinate);
						}
					}
				}
				else
					return false; 
			}

			const TimeConstraints& getTimeConstraints() const { return mTimeConstraints; }

			
			const void setTimeConstraints(const TimeConstraints& tc) {
				mTimeConstraints.setStartingTime(tc.getStartingTime());
				mTimeConstraints.setEndingTime(tc.getEndingTime());
			}

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<GeoPolygonalPrism>(this), mIdentifier, mTimeConstraints);
			}

		protected:
			TimeConstraints mTimeConstraints;

			void timeTranslate(const boost::posix_time::time_duration& deltaT) {
				mTimeConstraints.timeTranslate(deltaT);
			}

			TacticalSegment() {} // for cereal deserialization
			friend class ARL::Math::TacticalArea;
			friend class ARL::Mission::MissionObjective;
			friend class ARL::PathPlanning::NavPlan<Path2D>;
			friend class ARL::PathPlanning::NavPlan<Path3D>;
			friend class ARL::Database::NavPlanLogger;
			friend class std::vector<TacticalSegment>;
			friend class std::allocator<TacticalSegment>;
			friend class cereal::access;

		};

	}
}