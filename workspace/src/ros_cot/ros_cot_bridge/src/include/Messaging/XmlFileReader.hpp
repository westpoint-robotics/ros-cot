#pragma once
#include "XmlMessagingBase.hpp"
#include "macro.h"
#include "../Utility/xstr.hpp"

namespace Messaging {

	class XmlFileReader: public XmlMessagingBase {
	public:
		XmlFileReader(const std::string& uri = "") {
			mpParser = createLSParser();
		}

		XERCES_CPP_NAMESPACE::DOMDocument *OpenFile(const std::string& uri) {
			return mpParser->parseURI(uri.c_str());
		}

		static XERCES_CPP_NAMESPACE::DOMDocument* ReadXMLfromURI(const std::string& uri) {
			return createLSParser()->parseURI(uri.c_str());
		}

		DISALLOW_COPY_AND_ASSIGN(XmlFileReader);
	private:
		XERCES_CPP_NAMESPACE::DOMLSParser* mpParser;
//			XERCES_CPP_NAMESPACE::DOMDocument *domDocument; // this member has no purpose.
	};

}
