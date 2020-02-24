#pragma once
#pragma warning(disable: 4996)


#include "Path.hpp"
#include "cereal/types/vector.hpp"

#include <boost/foreach.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Polygon_2.h>

namespace ARL {
	namespace Math {
		using namespace std;

		class GeoPolygon : public Polygon < GeoCoordinate > {
		public:
			GeoPolygon(initializer_list<GeoCoordinate> list) : Polygon(list) {}
			GeoPolygon(const vector<GeoCoordinate>& geoVector) : Polygon(geoVector) {}
			GeoPolygon(const Polygon<GeoCoordinate> &list) : Polygon(list) {}

			GeoPolygon(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : Polygon(pElement) { }

			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"GeoPolygon")) const {
				return Polygon<GeoCoordinate>::createDOMElement(pDoc, name);
			}

		
			/**
			* This method returns a boolean, true if the geocoordinate is entirely within the calling geopolygon object.
			* @param p - the geocoordinate to check if it is within
			* @return A boolean, true if the geocoordinate is entirely within, false otherwise
			*/
			bool within(const GeoCoordinate& p) const {

				// [1] CGAL initializations  
				typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
				typedef K::Point_2											Point;
				
				// [2] Initialize an arbitary reference for geodesic to local cartesian transform
				ARL::Math::Geodetic::Origin origin(mPoints[0]);
				ARL::Math::Coordinate2D query_coordinate = origin.geodeticToENU(p);
				Point query_point(query_coordinate.getValue().x, query_coordinate.getValue().y);

				// [3] Iterate through the geopolygon's points, placing each into a vector as a CGAL point
				std::vector<Point> points;
				for (const auto& it : mPoints) {
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(it);
					points.push_back(Point(coordinate.getValue().x, coordinate.getValue().y)); 
				}

				// [4] Use CGAL's bounded_side_2 functionality to determine if the query_point is within the geopolygon 
				//		This CGAL function returns three enum types, where "ON_BOUNDED_SIDE" is equivalent to being within the geopolygon 
				CGAL::Bounded_side bs = CGAL::bounded_side_2(points.begin(), points.end(), query_point);

				// [5] If the point is "ON_BOUNDED_SIDE" equivilant being within the polygon, then return true, otherwise false 
				if (bs == CGAL::Bounded_side::ON_BOUNDED_SIDE) return true;
				else return false;
			}

			/**
			* This method returns a boolean, true if the geopolygon is entirely within the calling geopolygon object. 
			* Note, if two vertices of the two geopolygons are the same, the result will be true 
			* @param geopolygon - the geopolygon to check if it is within 
			* @return A boolean, true if the geopolygon is entirely within, false otherwise 
			*/
			bool within(const ARL::Math::GeoPolygon& geopolygon) const {
				for (const auto& point : geopolygon.getPoints()){
					if (this->within(point) || this->onSide(point)) continue;
					else return false;
				}
				return true;
			}

			/**
			* This method returns a boolean, true if the geocoordinate is on the boundary of the calling geopolygon object.
			* @param p - the geocoordinate to check if it is on the side of the geopolygon
			* @return A boolean, true if the point is on the boundary of the geopolygon, false otherwise
			*/
			bool onSide(const GeoCoordinate& p) const {

				// [1] CGAL initializations  
				typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
				typedef K::Point_2											Point;

				// [2] Initialize an arbitary reference for geodesic to local cartesian transform
				ARL::Math::Geodetic::Origin origin(mPoints[0]);
				ARL::Math::Coordinate2D query_coordinate = origin.geodeticToENU(p);
				Point query_point(query_coordinate.getValue().x, query_coordinate.getValue().y);

				// [3] Iterate through the geopolygon's points, placing each into a vector as a CGAL point
				std::vector<Point> points;
				for (const auto& it : mPoints) {
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(it);
					points.push_back(Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				// [4] Use CGAL's bounded_side_2 functionality to determine if the query_point is on the side of the geopolygon 
				//		This CGAL function returns three enum types, where "ON_BOUNDARY" is equivalent to being on the side of the geopolygon 
				CGAL::Bounded_side bs = CGAL::bounded_side_2(points.begin(), points.end(), query_point);

				// [5] If the point is "ON_BOUNDARY" equivilant being on the side of the geopolygon, then return true, otherwise false 
				if (bs == CGAL::Bounded_side::ON_BOUNDARY) return true;
				else return false;

			}


			/**
			* This method returns the intersection between two geopolygons. Note that some intersections may yield
			* disjoint results. Hence, the return type must return a vector of geopolygons to account for this
			* @param geopolygon - the geopolygon to take the intersection against 
			* @return A vector of geopolygons representing the intersection
			*/
			std::vector<ARL::Math::GeoPolygon> intersection(const ARL::Math::GeoPolygon& geopolygon) const {

				// [1] Arbitrarly choose the reference for geodesic to local cartesian to be the first point in the geopolygon
				ARL::Math::Geodetic::Origin origin(mPoints[0]);

				// [2] Boost type definitions 
				typedef boost::geometry::model::d2::point_xy<double>	Point;
				typedef boost::geometry::model::polygon<Point>			Polygon;

				// [3] Initialize the return vector of geopolygons 
				std::vector<ARL::Math::GeoPolygon> return_geopolygons;

				// [4] Form a boost polygon representation for this object and the input argument geopolygon 
				Polygon boost_polygon_1, boost_polygon_2;

				for (const auto& iter_1 : this->mPoints){
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(iter_1);
					boost::geometry::append(boost_polygon_1, Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				for (const auto& iter_2 : geopolygon.getPoints()){
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(iter_2);
					boost::geometry::append(boost_polygon_2, Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				// [5] Correct the boost polygons, fixing the orientation to be consistent with the boost standard (orientation matters) 
				boost::geometry::correct(boost_polygon_1);
				boost::geometry::correct(boost_polygon_2);

				// [6] Create the standard deque return object for the boost intersection function
				std::deque<Polygon> intersection_output;

				// [7] Use boost intersection to get the geopolygon intersection 
				boost::geometry::intersection(boost_polygon_1, boost_polygon_2, intersection_output);

				// [8] There may be more then one polygon resulting from the intersection. This will occur if the output is disjointed
				for (std::size_t i = 0; i < intersection_output.size(); ++i){
					std::vector<ARL::Math::GeoCoordinate> intersection_output_geocoordinates;
					BOOST_FOREACH(Point& point, intersection_output[i].outer()){
						ARL::Math::GeoCoordinate geocoordinate = origin.ENUToGeodetic(ARL::Math::Coordinate2D(point.x(), point.y()));
						intersection_output_geocoordinates.push_back(geocoordinate);
					}
					intersection_output_geocoordinates.erase(intersection_output_geocoordinates.end() - 1); // Erase the last geocoordinate in the vector, it is a redudant point (same as first)  
					return_geopolygons.push_back(ARL::Math::GeoPolygon(intersection_output_geocoordinates));
				}

				// [9] Return 
				return return_geopolygons;
			}

			/**
			* This method returns a boolean, true if the geopolygons have an intersection, false otherwise 
			* @param geopolygon - the geopolygon to take the intersection against
			* @return A boolean, true if the geopolygons have an intersection, false otherwise 
			*/
			bool intersects(const ARL::Math::GeoPolygon& geopolygon) const {

				// [1] Arbitrarly choose the reference for geodesic to local cartesian to be the first point in the geopolygon
				ARL::Math::Geodetic::Origin origin(mPoints[0]);

				// [2] Boost type definitions 
				typedef boost::geometry::model::d2::point_xy<double>	Point;
				typedef boost::geometry::model::polygon<Point>			Polygon;

				// [3] Form a boost polygon representation for this object and the input argument geopolygon 
				Polygon boost_polygon_1, boost_polygon_2;

				for (const auto& iter_1 : this->mPoints){
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(iter_1);
					boost::geometry::append(boost_polygon_1, Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				for (const auto& iter_2 : geopolygon.getPoints()){
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(iter_2);
					boost::geometry::append(boost_polygon_2, Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				// [4] Correct the boost polygons, fixing the orientation to be consistent with the boost standard (orientation matters) 
				boost::geometry::correct(boost_polygon_1);
				boost::geometry::correct(boost_polygon_2);

				// [5] Return 
				return boost::geometry::intersects(boost_polygon_1, boost_polygon_2);
			}

			/**
			* This method returns the area of a geopolygon, the value is in meters squared
			* @return A non-negative double, representing the value of the area in meters squared
			*/
			ARL::Math::NonNegativeDouble area() const{

				// [1] Boost type definitions 
				typedef boost::geometry::model::d2::point_xy<double> Point;
				typedef boost::geometry::model::polygon<Point>		 Polygon;

				// [2] Initialize the reference for geodesic to local cartesian transform (arbitrarly chosen) and the Boost polygon to represent the geopolygon 
				ARL::Math::Geodetic::Origin origin(mPoints[0]);
				Polygon polygon;

				// [3] Form a Boost polygon from the geopolygon's geodesic points 
				for (const auto& iter : this->mPoints){
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(iter);
					boost::geometry::append(polygon, Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				// [4] Boost requires a correction on polygons, to fix orientation to work properly within their functions 
				boost::geometry::correct(polygon);

				// [5] Call Boost's area functionality and return 
				double area = boost::geometry::area(polygon);
				return area;
			}

			/**
			* This method returns the centroid of the geopolygon
			* @return A geocoordinate that represents the centroid
			*/
			ARL::Math::GeoCoordinate centroid() const{

				// [1] Boost type definitions 
				typedef boost::geometry::model::d2::point_xy<double> Point;
				typedef boost::geometry::model::polygon<Point>		 Polygon;

				// [2] Initialize the reference for geodesic to local cartesian transform (arbitrarly chosen) and the Boost polygon to represent the geopolygon 
				ARL::Math::Geodetic::Origin origin(mPoints[0]);
				Polygon polygon;
				Point	centroid_point;

				// [3] Form a Boost polygon from the geopolygon's geodesic points 
				for (const auto& iter : this->mPoints){
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(iter);
					boost::geometry::append(polygon, Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				// [4] Boost requires a correction on polygons, to fix orientation to work properly within their functions 
				boost::geometry::correct(polygon);

				// [5] Call Boost's centroid functionality
				boost::geometry::centroid(polygon, centroid_point);

				// [6] Transform the centroid_point into an ARL Coordinate2D then into an ARL Geocoordinate and return 
				ARL::Math::Coordinate2D centroid_coordinate(centroid_point.x(), centroid_point.y());
				ARL::Math::GeoCoordinate centroid_geocoordinate = origin.ENUToGeodetic(centroid_coordinate);
				return centroid_geocoordinate;
			}

			/**
			* This method determines if a geopolygon's vertices have clockwise orientation
			* @return A boolean, true if the geopolygon has clockwise orientation, false if otherwise
			*/
			bool isClockwise() const{

				// [1] CGAL initializations  
				typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
				typedef CGAL::Point_2<K>									Point;
				typedef CGAL::Polygon_2<K>									Polygon;

				// [2] Initialize the reference for geodesic to local cartesian transform (arbitrarly chosen) and the Boost polygon to represent the geopolygon 
				ARL::Math::Geodetic::Origin origin(mPoints[0]);
				Polygon polygon;

				// [3] Form a CGAL polygon from the geopolygon's geodesic points 
				for (const auto& iter : this->mPoints){
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(iter);
					polygon.push_back(Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				// [4] Return 
				return (polygon.is_clockwise_oriented()) ? true : false;
			}

			/**
			* This method determines if a geopolygon's vertices have counter-clockwise orientation
			* @return A boolean, true if the geopolygon has counter-clockwise orientation, false if otherwise
			*/
			bool isCounterClockwise() const{

				// [1] CGAL initializations  
				typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
				typedef CGAL::Point_2<K>									Point;
				typedef CGAL::Polygon_2<K>									Polygon;

				// [2] Initialize the reference for geodesic to local cartesian transform (arbitrarly chosen) and the Boost polygon to represent the geopolygon 
				ARL::Math::Geodetic::Origin origin(mPoints[0]);
				Polygon polygon;

				// [3] Form a CGAL polygon from the geopolygon's geodesic points 
				for (const auto& iter : this->mPoints){
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(iter);
					polygon.push_back(Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				// [4] Return 
				return (polygon.is_counterclockwise_oriented()) ? true : false;
			}

			/**
			* This method determines if a geopolygon is convex
			* @return A boolean, true if the geopolygon is convex, false if otherwise
			*/
			bool isConvex() const{

				// [1] CGAL initializations  
				typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
				typedef CGAL::Point_2<K>									Point;
				typedef CGAL::Polygon_2<K>									Polygon;

				// [2] Initialize the reference for geodesic to local cartesian transform (arbitrarly chosen) and the Boost polygon to represent the geopolygon 
				ARL::Math::Geodetic::Origin origin(mPoints[0]);
				Polygon polygon;

				// [3] Form a CGAL polygon from the geopolygon's geodesic points 
				for (const auto& iter : this->mPoints){
					ARL::Math::Coordinate2D coordinate = origin.geodeticToENU(iter);
					polygon.push_back(Point(coordinate.getValue().x, coordinate.getValue().y));
				}

				// [4] Return 
				return (polygon.is_convex()) ? true : false;
			}



		protected:
			GeoPolygon() {} // for cereal deserialization
			friend class ARL::PathPlanning::NavPlan<Path2D>;
			friend class ARL::PathPlanning::NavPlan<Path3D>;
			friend class ARL::Database::NavPlanLogger;
		};

	}
}