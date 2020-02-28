// Bridge.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "CoTClient.hpp"

#include "ros/ros.h"
#include "sensor_msgs/NavSatFix.h"


//  CoT Multicast
//ATAK default for SA Multicast 239.2.3.1:6969
//
//MPU5 CoT Default 239.23.12.230:18999

using namespace std;

AIDTR::CoTClient *client = NULL;


void chatterCallback(const sensor_msgs::NavSatFix::ConstPtr& msg)
{
  //ROS_INFO("ROS heard: [Lat: %f, Long: %f, Alt: %f]", msg->latitude, msg->longitude, msg->altitude);
  if (client!= NULL)
	client->sendPositionReport(msg->latitude, msg->longitude, msg->altitude);
  else
	  ROS_INFO("Error: CoTClient not initialized, ROS is unable to forward message to CoTClient");
}



int main(int argc, char **argv)
{
    try
    {
        boost::asio::io_service io_service;
        io_service.run();

		client = new AIDTR::CoTClient(io_service,
			boost::asio::ip::address::from_string("239.2.3.1"), 6969);

		ros::init(argc, argv, "listener");

		ros::NodeHandle n;

		ros::Subscriber sub = n.subscribe("fix", 100, chatterCallback);

		ros::spin();
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
	

    
    return 0;
}
