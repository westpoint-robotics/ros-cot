#pragma once

#include "TacticalSegment.hpp"

namespace ARL {
	namespace Math {
		using namespace std;

		class ComplexGeoGeometry : public ARL::Messaging::XmlSerializable {
		public:
			ComplexGeoGeometry(initializer_list<TacticalSegment> list) : mTacticalSegments(list) {
				if (numTacticalSegments() < 1) throw Exception("A ComplexGeoGeometry object must contain at least 1 TacticalSegment.");
			}

			ComplexGeoGeometry(const vector<TacticalSegment> &tacticalSegments) {
				mTacticalSegments = tacticalSegments;
				if (numTacticalSegments() < 1)
					throw Exception("Assignment of empty tactical segment.");
			}

			ComplexGeoGeometry(const XERCES_CPP_NAMESPACE::DOMElement* pElement)
			{
				XERCES_CPP_NAMESPACE::DOMElement* pTacticalSegmentItems = pElement->getFirstElementChild();
				while (pTacticalSegmentItems) {
					TacticalSegment ts(pTacticalSegmentItems);
					mTacticalSegments.push_back(ts);
					pTacticalSegmentItems = pTacticalSegmentItems->getNextElementSibling();
				}
			}

			const TacticalSegment& operator[](size_t i) const { return mTacticalSegments[i]; }
			size_t numTacticalSegments() const { return mTacticalSegments.size(); }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"ComplexGeoGeometry")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				for (size_t i = 0; i < mTacticalSegments.size(); i++)
					pElement->appendChild(mTacticalSegments[i].createDOMElement(pDoc, Utility::xStr(L"TacticalSegment")));
				return pElement;
			}

			const vector<TacticalSegment>& getSegments() const { return mTacticalSegments; }

		protected:
			vector<TacticalSegment> mTacticalSegments;

			ComplexGeoGeometry() {} // for cereal deserialization
			friend class ARL::PathPlanning::NavPlan<Path2D>;
			friend class ARL::PathPlanning::NavPlan<Path3D>;
			friend class ARL::Database::NavPlanLogger;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(mTacticalSegments);
			}
		};

	}
}