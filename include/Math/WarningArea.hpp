#pragma once

#include "TacticalArea.hpp"

namespace ARL {
	namespace Math {
		using namespace std;
		class SpatialConstraints; ///forward declare for friending

		class WarningArea : public Utility::NamedEntity, public ARL::Messaging::XmlSerializable {
		public:
			using WarningAreas = std::vector<TacticalArea>;
			using DOMElement = XERCES_CPP_NAMESPACE::DOMElement;
			using DOMDocument = XERCES_CPP_NAMESPACE::DOMDocument;
			enum class Warning { CommercialTrafficArea, HighTrafficDensityArea, MilitaryOperationArea, TrawlerField, other, none };

			WarningArea(const TacticalArea& area, const Warning& primaryWarning, const Warning& secondaryWarning = Warning::none) 
				: primary(primaryWarning), secondary(secondaryWarning)
			{
				assert(primary != Warning::none); // at least one warning must be specified
				areas.push_back(area);
			}

			WarningArea(const WarningAreas& areas, const Warning& primaryWarning, const Warning& secondaryWarning = Warning::none)
				: areas(areas), primary(primaryWarning), secondary(secondaryWarning) {}

			WarningArea(const DOMElement* pElement)
				: NamedEntity(pElement), primary(toWarning(Utility::DOMHelper::parseDOMAttribute<std::string>(pElement, L"Warning"))), secondary(Warning::none)
			{
				auto s = Utility::DOMHelper::parseDOMAttribute<std::string>(pElement, L"SecondaryWarning");
				if (s != "") secondary = toWarning(s);
				auto nodes = pElement->getElementsByTagName(L"TacticalArea");
				for (unsigned long long i = 0; i < nodes->getLength(); i++) {
					auto node = static_cast<DOMElement*>(nodes->item(i));
					areas.push_back(TacticalArea(node));
				}
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name) const override {
				DOMElement* pElement = pDoc->createElement(name);
				setIdAttribute(pElement);
				setDOMWarningAttributes(pElement);
				for (auto a : areas) pElement->appendChild(a.createDOMElement(pDoc, L"TacticalArea"));
				return pElement;
			}
			
			const TacticalArea& operator[](const size_t i) const { return areas[i]; }
			size_t size() const { return areas.size(); }
			const WarningAreas& getWarningAreas() const { return areas; }
			void addArea(const TacticalArea& area) { areas.push_back(area); }

			static const char* toString(const Warning& warning) {
				switch (warning) {
				case Warning::CommercialTrafficArea: return "Commercial Traffic Area";
				case Warning::HighTrafficDensityArea: return "High Traffic Density Area";
				case Warning::MilitaryOperationArea: return "Military Operation Area";
				case Warning::TrawlerField: return "Trawler Field";
				case Warning::other: return "other"; 
				case Warning::none: return "none";
				default: throw Exception("Unkown Warning in WarningArea::toString(...)");
				}
			}

			static Warning toWarning(const string& s) {
				if (s.length() < 1) throw Exception("Invalid string in Warning::toWarning(...)");
				switch (s[0]) {
				case 'C':
					if (s == "Commercial Traffic Area") return Warning::CommercialTrafficArea;
					break;
				case 'H':
					if (s == "High Traffic Density Area") return Warning::HighTrafficDensityArea;
					break;
				case 'M':
					if (s == "Military Operation Area") return Warning::MilitaryOperationArea;
					break;
				case 'n':
					if (s == "none") return Warning::none;
					break;
				case 'o':
					if (s == "other") return Warning::other;
					break;
				case 'T':
					if (s == "Trawler Field") return Warning::TrawlerField;
					break;
				}
				return Warning::other; //default unknown warnings to other
			}

			DOMElement* setDOMWarningAttributes(DOMElement* pElement) const {
				pElement->setAttribute(L"Warning", xStr(toString(primary)));
				if (secondary != Warning::none) pElement->setAttribute(L"SecondaryWarning", xStr(toString(secondary)));
				return pElement;
			}

			const Warning& getPrimaryWarning() const { return primary; }
			const Warning& getSecondaryWarning() const { return secondary; }
		protected:
			Warning primary, secondary;
			WarningAreas areas;

			void timeTranslate(const boost::posix_time::time_duration& deltaT) {
				for (auto& it : areas)
					it.timeTranslate(deltaT);
			}

			WarningArea() {} // for cereal deserialization
			friend class ARL::Math::SpatialConstraints;
			friend class ARL::Database::NavPlanLogger;
			friend class cereal::access;
			friend class std::vector<WarningArea>;
			friend class std::allocator<WarningArea>;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(areas, primary, secondary);
			}
		};
	}
}