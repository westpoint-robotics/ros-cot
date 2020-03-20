#pragma once
#include <xercesc/dom/DOM.hpp>
#include "macro.h"

namespace Messaging {

	XERCES_CPP_NAMESPACE_USE;
	using XERCES_CPP_NAMESPACE::DOMDocument;
	using XERCES_CPP_NAMESPACE::DOMElement;
	using namespace std;

	class XmlSerializable {
	public:
		virtual DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name) const = 0;
	protected:
	};
}
