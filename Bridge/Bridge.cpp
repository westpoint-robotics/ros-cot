// Bridge.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "CoTClient.hpp"

//  CoT Multicast
//ATAK default for SA Multicast 239.2.3.1:6969
//
//MPU5 CoT Default 239.23.12.230:18999

using namespace std;

int main()
{
    try
    {
        boost::asio::io_service io_service;
        io_service.run();

        AIDTR::CoTClient client(io_service,
            boost::asio::ip::address::from_string("239.2.3.1"), 6969);

        client.sendPositionReport(40.45880, -79.78777, 335.28);

        this_thread::sleep_for(std::chrono::seconds(2));

        AIDTR::CoTClient client1(io_service,
            boost::asio::ip::address::from_string("239.2.3.1"), 6969,
            "AIDTR UAV 1", "a-f-G-U-C-V-U-R");
        client1.sendPositionReport(40.46058, -79.78637, 345);

        this_thread::sleep_for(std::chrono::seconds(2));

        cout << "Sent   " << client.getSendCount() << " messages." << endl
            << "Errors " << client.getErrorCount() << endl;
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}
