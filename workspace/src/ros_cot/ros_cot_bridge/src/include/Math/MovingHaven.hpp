#pragma once

#include "Types.hpp"

namespace ARL {
	namespace Math {
		using namespace std;

		class MovingHaven2D {
		public:
			MovingHaven2D(double forward = 10, double backward = 10, double eitherSide = 5, LengthUnits units = Meters)
				: mForward(forward, units), mBackward(backward, units), mEitherSide(eitherSide, units) { }

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

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(mForward, mBackward, mEitherSide);
			}
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

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(cereal::base_class<MovingHaven2D>(this), mUpward, mDownward);
			}
		};
	}
}