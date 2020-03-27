#pragma once

#include "DOMHelper.hpp"
#include <string>

namespace Utility {

	///@todo refactor derived classes to use (common XML Ser-des functionality from here)... currently child classes duplicate this functionality
	class NamedEntity { //interface for uniform wstring-based identification of concepts
	public:
		using wstring = std::wstring;

		virtual const wstring& getIdentifier() const { return mIdentifier; }
		wstring& getIdentifier() { return mIdentifier; }

		string getIdentifierAsString() const { return string(mIdentifier.begin(), mIdentifier.end()); }
		void setIdentifier(const wstring &id) { mIdentifier = id; }

	protected:
		NamedEntity(const wstring& identifier = L"") : mIdentifier(identifier) {}
		NamedEntity(const XERCES_CPP_NAMESPACE::DOMElement* pElement) : mIdentifier(ARL::Utility::DOMHelper::parseDOMAttribute<wstring>(pElement, xStr("Identifier"))) {}

		void setIdAttribute(XERCES_CPP_NAMESPACE::DOMElement* pElement) const {
			Utility::DOMHelper::setDOMAttribute(mIdentifier, pElement, xStr("Identifier"));
		}

		wstring mIdentifier;
	};
}
