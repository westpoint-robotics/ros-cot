#pragma once

namespace ARL {
	namespace Math {

		/** A class to represent an interval from min to max.
		*
		*	Intervals may be open or closed at either limit:
		*
		*	(min,max)
		*	(min,max]
		*	[min,max)
		*	[min,max]
		*/
		template <class T> 
		class Interval {
		public:
			Interval(const T& value, bool open = false)
				: mMin(-value), mMax(value), minOpen(open), maxOpen(open) {}
			Interval(const T& min, const T& max, bool minOpen = false, bool maxOpen = false)
				: mMin(min), mMax(max), minOpen(minOpen), maxOpen(maxOpen) {}

			const T& getMinimum() const { return mMin; }
			const T& getMaximum() const { return mMax; }
			bool minimumIsOpen() const { return minOpen; }
			bool maximumIsOpen() const { return maxOpen; }

			bool contains(const T& value) {
				return minOpen ? value > mMin : value >= mMin
					&& maxOpen ? value < mMax : value <= mMax;
			}
		protected:
			T mMin, mMax;
			bool minOpen, maxOpen;
		};
	}
}