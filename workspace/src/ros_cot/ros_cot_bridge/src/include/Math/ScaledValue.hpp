#pragma once

#include <Messaging/XmlSerializable.hpp>
#include <cereal/access.hpp>

namespace ARL {
	namespace Math {

		using ARL::Messaging::XmlSerializable;

		/// An abstract base class to represent a scaled value - that is, a value with magnitude and units.
		/// the derived classes should ALWAYS store the value in specific, uniform units (e.g. a length might always store in units of Meters)
		/// this allows mathematical operations to be performed easily, without having to deal with conversions and other complexity
		/// the interface allows a principaled way to interact with scaled values, making the units explicit at both initialization and retrieval.
		template <typename V, typename T>
		class ScaledValue : public XmlSerializable { //parent class for scaled values. Magnitude is always stored by children classes in pre-determined units to aid computations on values. Units indicate preferred units
		public:
			ScaledValue(const T& u = (T)0) : units(u) { }
			ScaledValue(const V& m, const T& u = (T)0) { magnitude = m; units = u; }

			/// retrieve a scaled value in specified units.
			/// @param units  the units in which the value is to be retrieved.
			virtual V getValue(const T units = T(0)) const = 0;
			/// retrieve the value in the units returned by getPreferredUnits()
			V getValueInPreferredUnits() const { return getValue(units); }
			/// retrieve the preferred units for this scaled value
			T getPreferredUnits() const { return units; }
		protected:
			V magnitude;
			T units;

			friend bool operator<(const ScaledValue& d1, const ScaledValue& d2) {
				return d1.magnitude < d2.magnitude;
			}
			friend bool operator>(const ScaledValue& d1, const ScaledValue& d2) {
				return d1.magnitude > d2.magnitude;
			}
			friend bool operator==(const ScaledValue& d1, const ScaledValue& d2) {
				return d1.magnitude == d2.magnitude;
			}
			friend bool operator!=(const ScaledValue& d1, const ScaledValue& d2) {
				return d1.magnitude != d2.magnitude;
			}
			friend bool operator<=(const ScaledValue& d1, const ScaledValue& d2) {
				return d1.magnitude <= d2.magnitude;
			}
			friend bool operator>=(const ScaledValue& d1, const ScaledValue& d2) {
				return d1.magnitude >= d2.magnitude;
			}
			friend ScaledValue& operator*=(ScaledValue& d1, double d) {
				d1.magnitude *= d;
				return d1;
			}
			friend ScaledValue& operator/=(ScaledValue& d1, double d) {
				d1.magnitude /= d;
				return d1;
			}

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(magnitude, units);
			}
		};


	}
}