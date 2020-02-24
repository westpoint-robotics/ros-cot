#pragma once

#include "Types.hpp"

namespace ARL {
	namespace Math {
		using namespace std;

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

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(mMinimumAltitude, mMaximumAltitude);
			}
		};

	}
}