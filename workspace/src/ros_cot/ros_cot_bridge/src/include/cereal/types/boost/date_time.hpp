#pragma once

#include <boost/date_time.hpp>
#include <cereal/types/string.hpp>
#include <Utility/Exception.hpp>

/// external serialization functions for boost::date_time entities;
namespace cereal {
	template<class Archive>
	void save(Archive & archive,
		const boost::posix_time::ptime & time)
	{
		try {
			archive(boost::posix_time::to_iso_extended_string(time));
		}
		catch (...) {
			throw ARL::Exception("Unable to serialize ptime in void save<Archive>(...)");
		}
	}

	template<class Archive>
	void load(Archive & archive,
		boost::posix_time::ptime & time)
	{
		std::string s;
		archive(s);
		try {
			time = boost::date_time::parse_delimited_time<boost::posix_time::ptime>(s, 'T');
		}
		catch (...) {
			if (s == "not-a-date-time") time = boost::posix_time::ptime(boost::date_time::special_values::not_a_date_time);
			else if (s == "-infinity") time = boost::posix_time::ptime(boost::date_time::special_values::neg_infin);
			else if (s == "+infinity") time = boost::posix_time::ptime(boost::date_time::special_values::pos_infin);
			else throw ARL::Exception("Unable to deserialize ptime in void load<Archive>(...)");
		}
	}

	template<class Archive>
	void save(Archive & archive,
		const boost::posix_time::time_duration & duration)
	{
		archive(duration.total_microseconds());
	}

	template<class Archive>
	void load(Archive & archive,
		boost::posix_time::time_duration & duration)
	{
		boost::posix_time::time_duration::tick_type ticks;
		archive(ticks);
		duration = boost::posix_time::microsec(ticks);
	}

}