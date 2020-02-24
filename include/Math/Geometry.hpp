#pragma once

#include "Types.hpp"
#include "Geodetics.hpp"
#include <initializer_list>
#include <cereal/types/vector.hpp>

namespace ARL {
	namespace Database { class NavPlanLogger; } //forward declare for friending

	namespace Math {

		using namespace std;

		template <class T>
		class Polygon : public XmlSerializable {
		public:
			Polygon(const vector<T>& vertices) : mPoints(vertices) {
				if (numVertices() < 3) throw Exception("A polygon must have at least 3 vertices.");
				for (std::vector<T>::const_iterator iter = mPoints.begin(); iter != mPoints.end(); ++iter){
					if (iter == --mPoints.end()){
						mSegments.push_back(std::make_pair(*iter, mPoints[0]));
					}
					else{
						mSegments.push_back(std::make_pair(*iter, *(iter + 1)));
					}
				}
			}

			Polygon(const XERCES_CPP_NAMESPACE::DOMElement* pElement) 
			{
				XERCES_CPP_NAMESPACE::DOMElement* pVertex = pElement->getFirstElementChild();
				while (pVertex) {
					T v(pVertex);
					mPoints.push_back(v);
					pVertex = pVertex->getNextElementSibling();
				}

				if (mPoints.size() < 3) throw Exception("A polygon must have at least 3 vertices.");
				for (std::vector<T>::const_iterator iter = mPoints.begin(); iter != mPoints.end(); ++iter){
					if (iter == --mPoints.end()){
						mSegments.push_back(std::make_pair(*iter, mPoints[0]));
					}
					else{
						mSegments.push_back(std::make_pair(*iter, *(iter + 1)));
					}
				}
			}

			const T& operator[](size_t i) const { return mPoints[i]; }
			size_t numVertices() const { return mPoints.size(); }
			const vector<T>& getPoints() const { return mPoints; }


			/**
			* This method returns a vector containing the polygons's boundary segments, represented as pairs of vertices
			* @return A vector, each element is a pair of points, representing one of the polygon's boundary segments
			*/
			const std::vector<std::pair<T, T>>& getSegments() const { return mSegments; }

			/**
			* This method reverses the orientation of the polygon (i.e. clockwise to counter-clockwise)
			* @return Void
			*/
			const void reverseOrientation() { std::reverse(mPoints.begin() + 1, mPoints.end()); }


			XERCES_CPP_NAMESPACE::DOMElement* createDOMElement(XERCES_CPP_NAMESPACE::DOMDocument* pDoc, const XMLCh* name = Utility::xStr(L"Polygon")) const override {
				XERCES_CPP_NAMESPACE::DOMElement* pElement = pDoc->createElement(name);
				for (size_t i = 0; i < mPoints.size(); i++)
					pElement->appendChild((*this)[i].createDOMElement(pDoc, Utility::xStr(L"vertex")));
				return pElement;
			}

			operator const vector<T>&() const { return mPoints; }
		protected:
			vector<T> mPoints;
			std::vector<std::pair<T, T>> mSegments;

			Polygon() {} // for cereal deserialization
			friend class ARL::Database::NavPlanLogger;
			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive, std::uint32_t const version) {
				archive(mPoints);
			}
		};

	}
}