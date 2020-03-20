#pragma once
#include "XmlMessagingBase.hpp"
#include <set>
#include <functional>
#include <algorithm>
#include <xercesc/framework/MemBufInputSource.hpp>

namespace Messaging {

	XERCES_CPP_NAMESPACE_USE;
	using namespace std;

	class XmlDecoder : public XmlMessagingBase {
	public:
		XmlDecoder() {
			pParser = pImplementationLS->createLSParser(
				DOMImplementationLS::DOMImplementationLSMode::MODE_SYNCHRONOUS,
				nullptr);
			pInput = pImplementationLS->createLSInput();
			XMLCh *str = XMLString::transcode("Zero length Buffer");
			pSource = new MemBufInputSource(static_cast<XMLByte*>(nullptr), 0, str);
			XMLString::release(&str);
			pSource->setCopyBufToStream(false);
			pInput->setByteStream(pSource);
		}

		~XmlDecoder() {
			pInput->release();
		}

		XERCES_CPP_NAMESPACE::DOMDocument* Decode(void *data, size_t dataSize) { ///@todo why the void pointer here? If you're only casting it to const XMLByte* data anyway, shouldn't the type be that?
			pSource->resetMemBufInputSource(static_cast<const XMLByte*>(data), dataSize);
			XERCES_CPP_NAMESPACE::DOMDocument* pDoc = pParser->parse(pInput);
			return(pDoc);
		}

	protected:
		DOMLSParser *pParser;
		DOMLSInput *pInput;
		MemBufInputSource *pSource;

	private:
		DISALLOW_COPY_AND_ASSIGN(XmlDecoder);
	};
}

