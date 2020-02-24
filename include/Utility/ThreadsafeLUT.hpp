#pragma once
#include <cstddef>
#include <algorithm>
#include <functional>
#include <list>
#include <mutex>
#include <vector>
#include <utility>
#include <boost/thread.hpp>

namespace ARL {
	namespace Utility {

		/* 
		 * Adopted from Anthony Williams' C++ Concurrency in Action
		 * This implementation takes advantage of the opportunity for concurrency
		 * across hash-table buckets, and allows multiple readers (but one writer)
		 * on each bucket.
		 *
		 * For simplicity, I omit iterator access to the container, instead favoring
		 * a purely associative interface.
		*/

		template <typename Key, typename Value,
			typename Hash = std::hash<Key>>
		class ThreadsafeLUT {

		public:

			// For best performance, set numBuckets to be prime.
			explicit ThreadsafeLUT(const Hash &hashFunction = Hash(),
				std::size_t numBuckets = 257)
			: mHashFunction(hashFunction), mBucket(numBuckets) {}

			ThreadsafeLUT(const ThreadsafeLUT &rhs) = delete;
			ThreadsafeLUT &operator=(const ThreadsafeLUT &rhs) = delete;
			
			Value valueFor(const Key &key, const Value &defaultValue = Value()) const {
				return getBucket(key)->valueFor(key, defaultValue);
			}

			void addOrUpdateMapping(const Key &key, const Value &value) {
				getBucket(key)->addOrUpdateMapping(key, value);
			}

			void removeMapping(const Key &key) {
				getBucket(key)->removeMapping(key);
			}

		private:

			class Bucket {

			public:
				Value valueFor(const Key &key, const Value &defaultValue) const {
					boost::shared_lock<boost::shared_mutex> lock(mMtx);
					auto itr = findEntryFor(key);
					return (itr == mData.end()) ?
					defaultValue : itr->second;
				}

				void addOrUpdateMapping(const Key &key, const Value &value) {
					std::unique_lock<boost::shared_mutex> lock(mMtx);
					auto itr = findEntryFor(key);
					if (itr == mData.end()) {
						mData.push_back(BucketValue(key, value));
					}
					else {
						itr->second = value;
					}
				}

				void removeMapping(const Key &key) {
					std::unique_lock<boost::shared_mutex> lock(mMtx);
					auto itr = findEntryFor(key);
					if (itr != mData.end()) {
						mData.erase(itr);
					}
				}

			private:

				using BucketValue = std::pair<Key, Value>;
				using BucketData = std::list<BucketValue>;
				using BucketItr = typename BucketData::iterator;
				using ConstBucketItr = typename BucketData::const_iterator;
				
				ConstBucketItr findEntryFor(const Key &key) const {
					return std::find_if(mData.begin(), mData.end(),
						[&](const BucketValue &item)
					{ return item.first == key; });
				}

				// Linear search through the bucket
				BucketItr findEntryFor(const Key &key) {
					return std::find_if(mData.begin(), mData.end(),
						[&](const BucketValue &item)
					{ return item.first == key; });
				}

				BucketData mData;
				mutable boost::shared_mutex mMtx; // Allows for concurrent reads on a bucket
			}; // Bucket

			Bucket *getBucket(const Key &key) {
				auto bucketIdx = mHashFunction(key) % mBucket.size();
				return &mBucket[bucketIdx];
			}

			const Bucket *getBucket(const Key &key) const {
				auto bucketIdx = mHashFunction(key) % mBucket.size();
				return &mBucket[bucketIdx];
			}

			Hash mHashFunction;
			std::vector<Bucket> mBucket;
		};

	}
}
