#pragma once

#include "PathSegment.hpp"
#include "MovingHaven.hpp"

namespace ARL {
	namespace Math {
		using namespace std;

		template < class Tstart, class Tsubsequent, class Tmh >
		class Path {
		public:
			/// segments has to have at least 1 point - the ending point.
			Path(const Tstart& startingPoint, const vector<Tsubsequent>& segments)
				: mStartingPoint(startingPoint), mPathSegments(segments), mMovingHavenValid(false) {
				if (mPathSegments.size() < 1) throw Exception("There must be at least 1 path segment, from starting point to ending point.");
			}
			Path(const Tstart& startingPoint, const vector<Tsubsequent>& segments, const Tmh& movingHaven)
				: mStartingPoint(startingPoint), mPathSegments(segments), mMovingHavenValid(true), mMovingHaven(movingHaven) {
			}
			Path(const XERCES_CPP_NAMESPACE::DOMElement* pElement)
				: mStartingPoint(pElement->getFirstElementChild()), mMovingHavenValid(false) {

				// start at second child - skip startingPoint
				// mMovingHaven initialized to false

				XERCES_CPP_NAMESPACE::DOMElement* pPathElement = pElement->getFirstElementChild()->getNextElementSibling();
//				const XERCES_CPP_NAMESPACE::DOMNodeList* pNodes = pElement->getChildNodes();

				while (pPathElement) {
					try {
						wstring name = Utility::aStr(pPathElement->getNodeName());
						if (name == L"PathSegment") {
							Tsubsequent pathSegment(pPathElement);
							mPathSegments.push_back(pathSegment);
							pPathElement = pPathElement->getNextElementSibling();
						}
						else {
							// optional MovingHave ?
							throw Exception("Only handling PathSegment");
						}
					}
					catch (Exception e) {
						stringstream ss;
						ss << "Unknown Node Name in Path: " << e.what();
						throw Exception(ss.str().c_str());
					}
				}
			}

			bool hasMovingHaven() const { return mMovingHavenValid; }
			const Tmh   & getMovingHaven() const { return(mMovingHaven); }
			const Tstart& getStartingPoint() const { return mStartingPoint; }
			const vector<Tsubsequent>& getPathSegments() const { return mPathSegments; }
			const Tsubsequent& getEndingPoint() const { return mPathSegments.back(); }
			bool hasIntermediatePathSegments() const { return mPathSegments.size() > 1; }
			size_t numPathSegments() const { return mPathSegments.size(); } /// a path always have at least one path segment from starting point to ending point

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"Path")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(mStartingPoint.createDOMElement(pDoc, Utility::xStr(L"StartingPoint")));
				for (size_t i = 0; i < mPathSegments.size(); i++)
					pElement->appendChild(mPathSegments[i].createDOMElement(pDoc, Utility::xStr(L"PathSegment")));
				if (hasMovingHaven()) pElement->appendChild(mMovingHaven.createDOMElement(pDoc, Utility::xStr(L"MovingHaven")));
				return pElement;
			}

			void timeTranslate(const boost::posix_time::time_duration& deltaT) {
				mStartingPoint.timeTranslate(deltaT);
				for (vector<Tsubsequent>::iterator it = mPathSegments.begin(); it != mPathSegments.end(); it++)
					it->timeTranslate(deltaT);
			}

		protected:
			Tstart mStartingPoint;
			vector<Tsubsequent> mPathSegments;
			Tmh mMovingHaven;
			bool mMovingHavenValid;

			Path() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(mStartingPoint, mPathSegments, mMovingHaven, mMovingHavenValid);
			}
		};

		class Path2D : public Path < Waypoint2D, PathSegment2D, MovingHaven2D > {
		public:
			Path2D(const Waypoint2D& startingPoint, const vector<PathSegment2D>& segments)
				: Path < Waypoint2D, PathSegment2D, MovingHaven2D >(startingPoint, segments) {}
			Path2D(const Waypoint2D& startingPoint, const vector<PathSegment2D>& segments, const MovingHaven2D &movingHaven)
				: Path < Waypoint2D, PathSegment2D, MovingHaven2D >(startingPoint, segments, movingHaven) {}
			Path2D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Path < Waypoint2D, PathSegment2D, MovingHaven2D >(pElement) {
			}
			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"Path2D")) const {
				return Path < Waypoint2D, PathSegment2D, MovingHaven2D >::createDOMElement(pDoc, name);
			}
		protected:
			Path2D() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class ARL::PathPlanning::NavPlan<Path2D>;
			friend class cereal::access;
		};

		class Path3D : public Path < Waypoint3D, PathSegment3D, MovingHaven3D > {
		public:
			Path3D(const Waypoint3D& startingPoint, const vector<PathSegment3D>& segments)
				: Path < Waypoint3D, PathSegment3D, MovingHaven3D >(startingPoint, segments) {}
			Path3D(const Waypoint3D& startingPoint, const vector<PathSegment3D>& segments, const MovingHaven3D &movingHaven)
				: Path < Waypoint3D, PathSegment3D, MovingHaven3D >(startingPoint, segments, movingHaven) {}
			Path3D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Path < Waypoint3D, PathSegment3D, MovingHaven3D >(pElement) {
			}
			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"Path3D")) const {
				return Path < Waypoint3D, PathSegment3D, MovingHaven3D >::createDOMElement(pDoc, name);
			}
		protected:
			Path3D() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class ARL::PathPlanning::NavPlan<Path3D>;
			friend class cereal::access;
		};

	}
}
