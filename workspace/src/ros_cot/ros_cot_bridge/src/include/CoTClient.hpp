#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <iostream>
#include <iomanip>
#include "Messaging/XmlMessagingBase.hpp"
#include "Utility/xstr.hpp"
#include "Utility/StringConversions.hpp"
#include "Utility/timeStrings.hpp"

namespace AIDTR {
	//
	class CoTClient : protected Messaging::XmlMessagingBase
	{
	public:
		CoTClient(boost::asio::io_service& io_service,
			const boost::asio::ip::address& multicast_address,
			const short multicast_port = 30001,
			const char* uid = "AIDTR Gator 1",
			const char* type = "a-f-G-E-V",
			const char* how = "m-f",
			bool simulation = true
		)
			: XmlMessagingBase(), 
			endpoint(multicast_address, multicast_port),
			socket(io_service, endpoint.protocol()),
			errorCount(0), sendCount(0) {

			std::lock_guard<std::mutex> lock(positionMutex);
			//create position report XML document
			auto r = createCoTDocument(uid,type,how,simulation);
			pPositionDoc = std::get<0>(r);
			pPointEl = std::get<1>(r);
			pDetailEl = std::get<2>(r);

			setPosition(pPointEl, 40.45932, -79.78582);

			pSerializer = pImplementationLS->createLSSerializer();
			pOutput = pImplementationLS->createLSOutput();
			pTarget = new xercesc_3_2::MemBufFormatTarget();
			pOutput->setByteStream(pTarget);
		}

		~CoTClient() {
			pOutput->release();
			delete pTarget;
		}

		void sendPositionReport(const double lat, const double lon, const double hae, const double ce = 10, const double le = 0.5) {
			{
				std::lock_guard<std::mutex> lock(positionMutex); //positionMutex protects against race conditions on the data in pPointEl... Only lock when changing this data.
				setPosition(pPointEl, lat, lon, hae, ce, le);
			}
			send(pPositionDoc);
		}

		void sendContactReport(const char* uid, const char* type,
			const double lat, const double lon, const double hae, const double ce = 10, const double le = 0.5,
			const char* how = "m-f", bool simulation = true)
		{
			auto r = createCoTDocument(uid, type, how, simulation);
			setPosition(std::get<1>(r), lat, lon, hae, ce, le);
			send(std::get<0>(r));
		}

		unsigned int getSendCount() const { return sendCount; }
		unsigned int getErrorCount() const { return errorCount; }
	protected:

		void send(xercesc_3_2::DOMDocument* pDoc) {
			std::stringstream os;
			{
				std::lock_guard<std::mutex> lock(serializerMutex);
				pSerializer->write(pDoc, pOutput);
				os.write((const char*)pTarget->getRawBuffer(), pTarget->getLen());
				pTarget->reset();
			}

			std::cout << os.str() << std::endl << std::endl;

			socket.async_send_to(boost::asio::buffer(os.str()),
				endpoint,
				boost::bind(&CoTClient::handle_send_to, this,
					boost::asio::placeholders::error));
			sendCount++;

		}

		static std::tuple<xercesc_3_2::DOMDocument*, xercesc_3_2::DOMElement *, xercesc_3_2::DOMElement *> createCoTDocument(const char* uid = "AIDTR Gator 1",
			const char* type = "a-f-G-E-V",
			const char* how = "m-f",
			bool simulation = true)
		{
			auto pPositionDoc = createDocument();
			pPositionDoc->setXmlStandalone(true);

			auto pEventEl = pPositionDoc->createElement(XMLString::transcode("event")); //root element

			pEventEl->setAttribute(XMLString::transcode("version"), XMLString::transcode("2.0"));
			pEventEl->setAttribute(XMLString::transcode("type"), XMLString::transcode(type));
			pEventEl->setAttribute(XMLString::transcode("access"), XMLString::transcode("unrestricted"));
			pEventEl->setAttribute(XMLString::transcode("qos"), XMLString::transcode("7-r-c"));
			pEventEl->setAttribute(XMLString::transcode("opex"), XMLString::transcode(simulation ? "s" : "e"));
			pEventEl->setAttribute(XMLString::transcode("uid"), XMLString::transcode(uid));

			namespace bpt = boost::posix_time;
			auto now = bpt::second_clock::universal_time();
			auto stale = now + bpt::seconds(60);
			pEventEl->setAttribute(XMLString::transcode("time"), Utility::xStr(Utility::ISOTimeStringZ(now)));
			pEventEl->setAttribute(XMLString::transcode("start"), Utility::xStr(Utility::ISOTimeStringZ(now)));
			pEventEl->setAttribute(XMLString::transcode("stale"), Utility::xStr(Utility::ISOTimeStringZ(stale)));

			pEventEl->setAttribute(XMLString::transcode("how"), XMLString::transcode(how));

			auto pPointEl = pPositionDoc->createElement(XMLString::transcode("point"));

			pEventEl->appendChild(pPointEl);

			auto pDetailEl = pPositionDoc->createElement(XMLString::transcode("point"));
			pEventEl->appendChild(pDetailEl);
			pPositionDoc->appendChild(pEventEl);

			return std::make_tuple(pPositionDoc, pPointEl, pDetailEl);
		}

		void handle_send_to(const boost::system::error_code& error) {
			if (error)
				errorCount++;
		}

		static void setPosition(xercesc_3_2::DOMElement* pPointEl, const double lat, const double lon, const double hae = 328.7, const double ce = 10, const double le = 0.5) {
			std::stringstream ss;
			ss << std::fixed << std::setprecision(6) << lat;
			pPointEl->setAttribute(XMLString::transcode("lat"), XMLString::transcode(ss.str().c_str()));
			ss.str(""); ss.clear(); ss << std::fixed << std::setprecision(6) << lon;
			pPointEl->setAttribute(XMLString::transcode("lon"), XMLString::transcode(ss.str().c_str()));
			ss.str(""); ss.clear(); ss << std::fixed << std::setprecision(3) << hae;
			pPointEl->setAttribute(XMLString::transcode("hae"), XMLString::transcode(ss.str().c_str())); //meters
			ss.str(""); ss.clear(); ss << std::fixed << std::setprecision(2) << ce;
			pPointEl->setAttribute(XMLString::transcode("ce"), XMLString::transcode(ss.str().c_str())); //meters
			ss.str(""); ss.clear(); ss << std::fixed << std::setprecision(2) << le;
			pPointEl->setAttribute(XMLString::transcode("le"), XMLString::transcode(ss.str().c_str())); //meters
		}

		xercesc_3_2::DOMDocument* pPositionDoc;
		xercesc_3_2::DOMElement* pPointEl;
		xercesc_3_2::DOMElement* pDetailEl;

		xercesc_3_2::DOMLSSerializer* pSerializer;
		xercesc_3_2::DOMLSOutput* pOutput;
		xercesc_3_2::MemBufFormatTarget* pTarget;

		std::mutex positionMutex, serializerMutex;
		
	private:
		boost::asio::ip::udp::endpoint endpoint;
		boost::asio::ip::udp::socket socket;

		unsigned int errorCount, sendCount;
	};
}