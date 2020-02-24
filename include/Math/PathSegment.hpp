#pragma once

#include "Waypoint.hpp"
#include "AltitudeLimits.hpp"

namespace ARL {
	namespace Math {
		using namespace std;

		template <class T>
		class PathSegment : public T {
		public:
			PathSegment(const T& point, const NonNegativeDistance& crossTrackLimit = NonNegativeDistance::NotADistance())
				: T(point), mCrossTrackLimit(crossTrackLimit) {
			}

			bool hasCrossTrackLimit() const { return mCrossTrackLimit != NonNegativeDistance::NotADistance(); }
			const NonNegativeDistance getCrossTrackLimit() const { return mCrossTrackLimit; }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"PathSegment")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = T::createDOMElement(pDoc, name);
				if (hasCrossTrackLimit()) pElement->appendChild(mCrossTrackLimit.createDOMElement(pDoc, xStr(L"CrossTrackLimit")));
				return pElement;
			}

			PathSegment(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : T(pElement)	{
				mCrossTrackLimit = NonNegativeDistance::NotADistance();
				const XERCES_CPP_NAMESPACE::DOMNodeList* pNodes = pElement->getElementsByTagName(xStr(L"CrossTrackLimit"));
				if (pNodes->getLength() > 0) {
					mCrossTrackLimit = NonNegativeDistance(static_cast<const DOMElement*>(pNodes->item(0)));
				}
			}

		protected:
			NonNegativeDistance mCrossTrackLimit;

			PathSegment() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<T>(this), mCrossTrackLimit);
			}
		};

		class PathSegment2D : public PathSegment < Waypoint2D > {
		public:
			PathSegment2D(const Waypoint2D& point, const NonNegativeDistance& crossTrackLimit = NonNegativeDistance())
				: PathSegment<Waypoint2D>(point, crossTrackLimit) {}

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"PathSegment2D")) const {
				return PathSegment < Waypoint2D >::createDOMElement(pDoc, name);
			}

			PathSegment2D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : PathSegment<Waypoint2D>(pElement) {}

		protected:
			PathSegment2D() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class std::vector<PathSegment2D>;
			friend class std::allocator<PathSegment2D>;
		};

		class PathSegment3D : public PathSegment < Waypoint3D > {
		public:
			PathSegment3D(const Waypoint3D& point, const NonNegativeDistance& crossTrackLimit = NonNegativeDistance(), const AltitudeLimits& altitudeLimits = AltitudeLimits())
				: PathSegment < Waypoint3D >(point, crossTrackLimit), mAltitudeLimits(altitudeLimits) {
				}
			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"PathSegment2D")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = PathSegment < Waypoint3D >::createDOMElement(pDoc, name);
				if (mAltitudeLimits.hasAltitudeLimits()) pElement->appendChild(mAltitudeLimits.createDOMElement(pDoc, Utility::xStr(L"AltitudeLimits")));
				return pElement;
			}

			PathSegment3D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : PathSegment<Waypoint3D>(pElement) {}
			const AltitudeLimits &getAltitudeLimits(void) const { return(mAltitudeLimits); }

		protected:
			AltitudeLimits mAltitudeLimits;

			PathSegment3D() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class std::vector<PathSegment3D>;
			friend class std::allocator<PathSegment3D>;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<PathSegment < Waypoint3D > >(this), mAltitudeLimits);
			}
		};

	}
}
