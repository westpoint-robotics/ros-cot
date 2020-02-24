#pragma once
#include "Sensors/ArdupilotMega/MavLink/ardupilotmega/mavlink.h"
#include <Sensors/ArdupilotMega/MavLink/CustomArduPilotMessages.hpp>

namespace cereal {

 template<class Archive>
 void serialize(Archive & archive, mavlink_heartbeat_t& d) {
	 archive(d.custom_mode, d.type, d.autopilot, d.base_mode, d.system_status, d.mavlink_version);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_sys_status_t& d) {
	 archive(d.onboard_control_sensors_present,
		 d.onboard_control_sensors_enabled,
		 d.onboard_control_sensors_health,
		 d.load,
		 d.voltage_battery,
		 d.current_battery,
		 d.drop_rate_comm,
		 d.errors_comm,
		 d.errors_count1,
		 d.errors_count2,
		 d.errors_count3,
		 d.errors_count4,
		 d.battery_remaining);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_system_time_t& d) {
	 archive(d.time_unix_usec, d.time_boot_ms);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_gps_raw_int_t& d) {
	 archive(d.time_usec,
		 d.lat,
		 d.lon,
		 d.alt,
		 d.eph,
		 d.epv,
		 d.vel,
		 d.cog,
		 d.fix_type,
		 d.satellites_visible);
 }

template<class Archive>
 void serialize(Archive & archive, mavlink_global_position_int_t& d) {
	 archive(d.time_boot_ms,
		 d.lat,
		 d.lon,
		 d.alt,
		 d.relative_alt,
		 d.vx,
		 d.vy,
		 d.vz,
		 d.hdg);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_raw_imu_t& d) {
	 archive(d.time_usec,
		 d.xacc,
		 d.yacc,
		 d.zacc,
		 d.xgyro,
		 d.ygyro,
		 d.zgyro,
		 d.xmag,
		 d.ymag,
		 d.zmag);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_attitude_t& d) {
	 archive(d.time_boot_ms,
		 d.roll,
		 d.pitch,
		 d.yaw,
		 d.rollspeed,
		 d.pitchspeed,
		 d.yawspeed);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_rc_channels_raw_t& d) {
	 archive(d.time_boot_ms,
		 d.chan1_raw,
		 d.chan2_raw,
		 d.chan3_raw,
		 d.chan4_raw,
		 d.chan5_raw,
		 d.chan6_raw,
		 d.chan7_raw,
		 d.chan8_raw,
		 d.port,
		 d.rssi);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_servo_output_raw_t& d) {
	 archive(d.time_usec,
		 d.servo1_raw,
		 d.servo2_raw,
		 d.servo3_raw,
		 d.servo4_raw,
		 d.servo5_raw,
		 d.servo6_raw,
		 d.servo7_raw,
		 d.servo8_raw,
		 d.port);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_mission_current_t& d) {
	 archive(d.seq);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_nav_controller_output_t& d) {
	 archive(d.nav_roll,
		 d.nav_pitch,
		 d.alt_error,
		 d.aspd_error,
		 d.xtrack_error,
		 d.nav_bearing,
		 d.target_bearing,
		 d.wp_dist);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_meminfo_t& d) {
	 archive(d.brkval, d.freemem);
 }

 template<class Archive>
 void serialize(Archive & archive, mavlink_hwstatus_t& d) {
	 archive(d.Vcc, d.I2Cerr);
 }

}