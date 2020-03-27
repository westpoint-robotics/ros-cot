#pragma once

#include "WarningArea.hpp"
#include <cereal/types/vector.hpp>

namespace ARL {
	namespace Math {

		class SpatialConstraints : public ARL::Messaging::XmlSerializable {
		public:
			SpatialConstraints(const vector<TacticalArea>& inclusionAreas, const vector<TacticalArea>& exclusionAreas = vector<TacticalArea>())
			: mInclusionAreas(inclusionAreas), mExclusionAreas(exclusionAreas) { }

			///parses appropriate child elements into appropriate constraint members.
			SpatialConstraints(const DOMElement* pElement) {
				{
					const auto pCE = pElement->getElementsByTagName(L"InclusionAreas");
					assert(pCE->getLength() == 1); ///there may be exactly 1 InclusionAreas elements
					const DOMElement* pIAEl = static_cast<const DOMElement*>(pCE->item(0));
					for (DOMElement* pTacticalArea = pIAEl->getFirstElementChild(); pTacticalArea; pTacticalArea = pTacticalArea->getNextElementSibling())
						mInclusionAreas.push_back(TacticalArea(pTacticalArea));
				}
				{
					const auto pCE = pElement->getElementsByTagName(L"ExclusionAreas");
					assert(pCE->getLength() == 0 || pCE->getLength() == 1); ///there may be 0 or 1 ExclusionAreas elements
					if (pCE->getLength() == 1) {
						const DOMElement* pIAEl = static_cast<const DOMElement*>(pCE->item(0));
						for (DOMElement* pTacticalArea = pIAEl->getFirstElementChild(); pTacticalArea; pTacticalArea = pTacticalArea->getNextElementSibling())
							mExclusionAreas.push_back(TacticalArea(pTacticalArea));
					}
				}
				{
					const auto pCE = pElement->getElementsByTagName(L"WarningAreas");
					assert(pCE->getLength() == 0 || pCE->getLength() == 1); ///there may be 0 or 1 WarningAreas elements
					if (pCE->getLength() == 1) {
						const DOMElement* pIAEl = static_cast<const DOMElement*>(pCE->item(0));
						for (DOMElement* pWarningArea = pIAEl->getFirstElementChild(); pWarningArea; pWarningArea = pWarningArea->getNextElementSibling())
							warningAreas.push_back(WarningArea(pWarningArea));
					}
				}
			}

			const vector<TacticalArea>& getInclusionAreas() const { return mInclusionAreas; }
			const vector<TacticalArea>& getExclusionAreas() const { return mExclusionAreas; }
			const vector<WarningArea>& getWarningAreas() const { return warningAreas; }
			void updateInclusionAreas(const vector<TacticalArea>& areas) { mInclusionAreas = areas; }
			void updateExclusionAreas(const vector<TacticalArea>& areas) { mExclusionAreas = areas; }
			void updateWarningAreas(const vector<WarningArea>& areas) { warningAreas = areas; }

			vector<TacticalArea>& getInclusionAreas() { return mInclusionAreas; }
			vector<TacticalArea>& getExclusionAreas() { return mExclusionAreas; }
			vector<WarningArea>& getWarningAreas() { return warningAreas; }

			/// adds child elements to DOMElement pointed to by pElement
			void setDOMElements(DOMDocument* pDoc, DOMElement* pElement) const {
				DOMElement* pInclusionElement = pDoc->createElement(xStr("InclusionAreas"));
				for (size_t i = 0; i < mInclusionAreas.size(); i++)
					pInclusionElement->appendChild(mInclusionAreas[i].createDOMElement(pDoc, xStr("TacticalArea")));
				pElement->appendChild(pInclusionElement);

				if (mExclusionAreas.size() > 0) {
					DOMElement* pExclusionElement = pDoc->createElement(xStr("ExclusionAreas"));
					for (size_t i = 0; i < mExclusionAreas.size(); i++)
						pExclusionElement->appendChild(mExclusionAreas[i].createDOMElement(pDoc, xStr("TacticalArea")));
					pElement->appendChild(pExclusionElement);
				}

				if (warningAreas.size() > 0) {
					DOMElement* pWarningElement = pDoc->createElement(xStr("WarningAreas"));
					for (size_t i = 0; i < warningAreas.size(); i++)
						pWarningElement->appendChild(warningAreas[i].createDOMElement(pDoc, xStr("WarningArea")));
					pElement->appendChild(pWarningElement);
				}
			}

			///@returns true iff space-time point within all inclusionAreas and not within any exclusionArea
			/// this is the product level of the canonical product-of-sums form of spatial-temporal constraints
			bool within(const ARL::Math::GeoCoordinate& geocoordinate, const boost::posix_time::ptime time_constraint) const {
				for ( const auto& a : mInclusionAreas )
					if (!a.within(geocoordinate, time_constraint)) return false;
				for ( const auto& a : mExclusionAreas)
					if (a.within(geocoordinate, time_constraint)) return false;
				return true;
			}

		protected:
			void timeTranslate(const boost::posix_time::time_duration& deltaT) {
				for (auto& it : mInclusionAreas)
					it.timeTranslate(deltaT);
				for (auto& it : mExclusionAreas)
					it.timeTranslate(deltaT);
				for (auto& it : warningAreas)
					it.timeTranslate(deltaT);
			}

			vector<TacticalArea> mInclusionAreas;
			vector<TacticalArea> mExclusionAreas;
			vector<WarningArea> warningAreas;

			SpatialConstraints() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(mInclusionAreas, mExclusionAreas, warningAreas);
			}
		};
	}
}