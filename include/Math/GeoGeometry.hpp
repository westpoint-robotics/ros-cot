#pragma once

#include "Geometry.hpp"
#include <vector>

namespace ARL {
	namespace Math {

		using namespace std;

		class GeoPolygon : public Polygon < GeoCoordinate > {
		public:
			GeoPolygon(initializer_list<GeoCoordinate> list) : Polygon(list) {}
			
			// Bob's new overloaded constructor
			GeoPolygon(const vector<GeoCoordinate>& geoVector) : Polygon(geoVector) {}

			GeoPolygon(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Polygon(pElement)
			{

			}

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"GeoPolygon")) const {
				return Polygon<GeoCoordinate>::createDOMElement(pDoc, name);
			}
		protected:
		};

		class GeoPolygonalPrism : public GeoPolygon, public ARL::Messaging::XmlSerializable {
		public:
			GeoPolygonalPrism(const GeoPolygon& gp, const Distance<> min, const Distance<> max)
				: GeoPolygon(gp), mMin(min), mMax(max) {
				if (min >= max) throw Exception("minimum must be less than maximum in GeoPolygonalPrism constructor");
			}

			GeoPolygonalPrism(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : GeoPolygon(pElement->getFirstElementChild()),
				mMin(pElement->getFirstElementChild()->getNextElementSibling()), 
				mMax(pElement->getFirstElementChild()->getNextElementSibling()->getNextElementSibling())
			{
				// assumes min and max altitude come after GeoPolygon in order
				if (mMin >= mMax) throw Exception("minimum must be less than maximum in GeoPolygonalPrism constructor");
			}

			const Distance<>& getMinimumAltitude() const { return mMin; }
			const Distance<>& getMaximumAltitude() const { return mMax; }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"GeoPolygonalPrism")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(Polygon<GeoCoordinate>::createDOMElement(pDoc, Utility::xStr(L"GeoPolygon")));
				pElement->appendChild(mMin.createDOMElement(pDoc, Utility::xStr(L"MinimumAltitude")));
				pElement->appendChild(mMax.createDOMElement(pDoc, Utility::xStr(L"MaximumAltitude")));
				return pElement;
			}
		protected:
			Distance<> mMin;
			Distance<> mMax;
		};

		class TacticalSegment : public GeoPolygonalPrism {
		public:
			TacticalSegment(const GeoPolygon& gp, const Distance<> min = Distance<>::NotADistance(), const Distance<> max = Distance<>::NotADistance(), const wstring& id = L"(unlabeled)", const TimeConstraints& timeConstraints = TimeConstraints())
				: GeoPolygonalPrism(gp, min, max), mIdentifier(id), mTimeConstraints(timeConstraints) {}
			TacticalSegment(const GeoPolygonalPrism& prism, const wstring& id = L"(unlabeled)", const TimeConstraints& timeConstraints = TimeConstraints())
				: GeoPolygonalPrism(prism), mIdentifier(id), mTimeConstraints(timeConstraints) {}

			TacticalSegment(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : GeoPolygonalPrism(pElement) {
				mIdentifier = Utility::aStr(pElement->getAttribute(Utility::xStr(L"Identifier")));
			}

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"TacticalSegment")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = GeoPolygonalPrism::createDOMElement(pDoc, name);
				pElement->setAttribute(Utility::xStr(L"Identifier"), Utility::xStr(mIdentifier.c_str()));
				return mTimeConstraints.setDOMAttributeGroup(pElement);
			}

			const wstring& getIdentifier() const { return mIdentifier; }
			const TimeConstraints& getTimeConstraints() const { return mTimeConstraints; }

		protected:
			wstring mIdentifier;
			TimeConstraints mTimeConstraints;
		};

		class ComplexGeoGeometry {
		public:
			ComplexGeoGeometry(initializer_list<TacticalSegment> list) : mTacticalSegments(list) {
				if (numTacticalSegments() < 1) throw Exception("A ComplexGeoGeometry object must contain at least 1 TacticalSegment.");
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

			const GeoPolygonalPrism& operator[](size_t i) const { return mTacticalSegments[i]; }
			size_t numTacticalSegments() const { return mTacticalSegments.size(); }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"ComplexGeoGeometry")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				for (size_t i = 0; i < mTacticalSegments.size(); i++)
					pElement->appendChild(mTacticalSegments[i].createDOMElement(pDoc, Utility::xStr(L"TacticalSegment")));
				return pElement;
			}

		protected:
			vector<TacticalSegment> mTacticalSegments;
		};

		class TacticalArea : public ComplexGeoGeometry, public ARL::Messaging::XmlSerializable {
		public:
			TacticalArea(const ComplexGeoGeometry& area, const TimeConstraints& timeConstraints, const wstring& id = L"TacticalArea")
				: ComplexGeoGeometry(area), mTimeConstraints(timeConstraints), mIdentifier(id) {
			}

			TacticalArea(const XERCES_CPP_NAMESPACE::DOMElement* pElement) :
				ComplexGeoGeometry(pElement),
				mTimeConstraints(pElement->getAttributeNode(Utility::xStr(L"startingTime")), pElement->getAttributeNode(Utility::xStr(L"endingTime")))
			{
				mIdentifier = Utility::aStr(pElement->getAttribute(Utility::xStr(L"Identifier")));
			}

			const wstring& getIdentifier() const { return mIdentifier; }
			const TimeConstraints& getTimeConstraints() const { return mTimeConstraints; }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"TacticalArea")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = ComplexGeoGeometry::createDOMElement(pDoc, name);
				pElement->setAttribute(Utility::xStr(L"Identifier"), Utility::xStr(mIdentifier.c_str()));
				return mTimeConstraints.setDOMAttributeGroup(pElement);
			}

		protected:
			wstring mIdentifier;
			TimeConstraints mTimeConstraints;
		};



		class MovingHaven2D {
		public:
			MovingHaven2D(double forward = 10, double backward = 10, double eitherSide = 5, LengthUnits units = Meters) 
				: mForward(forward, units), mBackward(backward, units), mEitherSide(eitherSide, units) {

			}

			const NonNegativeDistance& getForward() const { return mForward; }
			const NonNegativeDistance& getBackward() const { return mBackward; }
			const NonNegativeDistance& getEitherSide() const { return mEitherSide; }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"MovingHaven2D")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				pElement->appendChild(mForward.createDOMElement(pDoc, Utility::xStr(L"forward")));
				pElement->appendChild(mBackward.createDOMElement(pDoc, Utility::xStr(L"backward")));
				pElement->appendChild(mEitherSide.createDOMElement(pDoc, Utility::xStr(L"eitherSide")));
				return pElement;
			}
		protected:
			NonNegativeDistance mForward;
			NonNegativeDistance mBackward;
			NonNegativeDistance mEitherSide;
		};

		class MovingHaven3D : public MovingHaven2D {
		public:
			MovingHaven3D(double forward = 10, double backward = 10, double eitherSide = 5, double upward = 5, double downward = 5, LengthUnits units = Meters)
				: MovingHaven2D(forward, backward, eitherSide, units), mUpward(upward, units), mDownward(downward, units) {
			}

			const NonNegativeDistance& getUpward() const { return mUpward; }
			const NonNegativeDistance& getDownward() const { return mDownward; }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"MovingHaven3D")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = MovingHaven2D::createDOMElement(pDoc, name);
				pElement->appendChild(mUpward.createDOMElement(pDoc, Utility::xStr(L"upward")));
				pElement->appendChild(mDownward.createDOMElement(pDoc, Utility::xStr(L"downward")));
				return pElement;
			}
		protected:
			NonNegativeDistance mUpward;
			NonNegativeDistance mDownward;
		};

		template <class T>
		class Waypoint : public T {
		public:
			Waypoint(const T& point, const ptime& expectedTime, const wstring& id = Utility::xStr(L"(anonymous)"))
				: T(point), mIdentifier(id), mTimeConstraints(expectedTime) {
			}
			Waypoint(const T& point, const ptime& expectedTime, const ptime& endingTime, const wstring& id = Utility::xStr(L"(anonymous)"))
				: T(point), mIdentifier(id), mTimeConstraints(expectedTime, ptime(not_a_date_time), endingTime) {
			}
			Waypoint(const T& point, const ptime& expectedTime, const ptime& startingTime, const ptime& endingTime, const wstring& id = Utility::xStr(L"(anonymous)"))
				: T(point), mIdentifier(id), mTimeConstraints(expectedTime, startingTime, endingTime) {
			}
			Waypoint(const XERCES_CPP_NAMESPACE::DOMElement* pElement) 
				: T(pElement), 
					mTimeConstraints(pElement->getAttributeNode(Utility::xStr(L"expectedTime")),
									pElement->getAttributeNode(Utility::xStr(L"startingTime")),
									pElement->getAttributeNode(Utility::xStr(L"endingTime"))) {
								mIdentifier = Utility::aStr(pElement->getAttribute(Utility::xStr(L"Identifier")));
			}
			const wstring& getIdentifier() const { return mIdentifier; }
			const ExpectedTimeConstraints& getTimeConstraints() const { return mTimeConstraints; }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"Waypoint")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = T::createDOMElement(pDoc, name);
				return setDOMAttributeGroup(pElement);
			}
		protected:
			XERCES_CPP_NAMESPACE::DOMElement* setDOMAttributeGroup(XERCES_CPP_NAMESPACE::DOMElement* pElement) const {
				pElement->setAttribute(Utility::xStr(L"Identifier"), Utility::xStr(mIdentifier.c_str()));
				return mTimeConstraints.setDOMAttributeGroup(pElement);
			}

			wstring mIdentifier;
			ExpectedTimeConstraints mTimeConstraints;
		};

		class Waypoint2D : public Waypoint < GeoCoordinate > {
		public:
			Waypoint2D(const GeoCoordinate& point, const ptime& expectedTime, const wstring& id = L"(anonymous)")
				: Waypoint<GeoCoordinate>(point, expectedTime, id) {}
			Waypoint2D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Waypoint < GeoCoordinate >(pElement) { }
		protected:
		};

		class Waypoint3D : public Waypoint < GeoCoordinate3D > {
		public:
			Waypoint3D(const GeoCoordinate3D& point, const ptime& expectedTime, const wstring& id = L"(anonymous)")
				: Waypoint<GeoCoordinate3D>(point, expectedTime, id) {}
			Waypoint3D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Waypoint < GeoCoordinate3D >(pElement) { }
		protected:
		};

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
		};

		class AltitudeLimits {
		public:
			AltitudeLimits(const Distance<>& minimumAltitude = Distance<>::NotADistance(), const Distance<>& maximumAltitude = Distance<>::NotADistance())
				: mMinimumAltitude(minimumAltitude), mMaximumAltitude(maximumAltitude) {
				if (hasMinimumAltitude() && hasMaximumAltitude() && mMinimumAltitude >= mMaximumAltitude) throw Exception("minimum altitude must be less than maximum altitude.");
			}
			const Distance<>& getMinimumAltitude() const { return mMinimumAltitude; }
			const Distance<>& getMaximumAltitude() const { return mMaximumAltitude; }
			bool hasMinimumAltitude() const { return mMinimumAltitude != Distance<>::NotADistance(); }
			bool hasMaximumAltitude() const { return mMaximumAltitude != Distance<>::NotADistance(); }
			bool hasAltitudeLimits() const { return hasMinimumAltitude() || hasMaximumAltitude(); }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"TimeConstraints")) const {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				if (hasMinimumAltitude()) pElement->appendChild(mMinimumAltitude.createDOMElement(pDoc, Utility::xStr(L"MinimumAltitude")));
				if (hasMaximumAltitude()) pElement->appendChild(mMaximumAltitude.createDOMElement(pDoc, Utility::xStr(L"MaximumAltitude")));
				return pElement;
			}

		protected:
			Distance<> mMinimumAltitude;
			Distance<> mMaximumAltitude;
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

		protected:
			AltitudeLimits mAltitudeLimits;
		};

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
				const XERCES_CPP_NAMESPACE::DOMNodeList* pNodes = pElement->getChildNodes();

				while (pPathElement) {
					try {
						wstring name = Utility::aStr(pPathElement->getNodeName());
						if (name == L"PathSegment") {
							Tsubsequent pathSegment(pPathElement);
							mPathSegments.push_back(pathSegment);
							pPathElement = pPathElement->getNextElementSibling();
						} else {
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
		protected:
			Tstart mStartingPoint;
			vector<Tsubsequent> mPathSegments;
			Tmh mMovingHaven;
			bool mMovingHavenValid;
		};

		class Path2D : public Path < Waypoint2D, PathSegment2D, MovingHaven2D > {
		public:
			Path2D(const Waypoint2D& startingPoint, const vector<PathSegment2D>& segments)
				: Path < Waypoint2D, PathSegment2D, MovingHaven2D >(startingPoint, segments) {}
			Path2D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Path < Waypoint2D, PathSegment2D, MovingHaven2D >(pElement) {
			}
			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"Path2D")) const {
				return Path < Waypoint2D, PathSegment2D, MovingHaven2D >::createDOMElement(pDoc, name);
			}
		protected:
		};

		class Path3D : public Path < Waypoint3D, PathSegment3D, MovingHaven3D > {
		public:
			Path3D(const Waypoint3D& startingPoint, const vector<PathSegment3D>& segments)
				: Path < Waypoint3D, PathSegment3D, MovingHaven3D >(startingPoint, segments) {}
			Path3D(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Path < Waypoint3D, PathSegment3D, MovingHaven3D >(pElement) {
			}
			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"Path3D")) const {
				return Path < Waypoint3D, PathSegment3D, MovingHaven3D >::createDOMElement(pDoc, name);
			}
		protected:
		};

	}
}
