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

double lat1[] = { 40.45771, 40.45913, 40.45989 };
double lon1[] = { -79.78910, -79.78931, -79.78935 };

double lat2[] = { 40.46168, 40.45840, 40.45756 };
double lon2[] = { -79.78581, -79.78670, -79.79065 };

int main()
{
    try
    {
        boost::asio::io_service io_service;
        io_service.run();

        AIDTR::CoTClient client(io_service,
            boost::asio::ip::address::from_string("239.2.3.1"), 6969);

        AIDTR::CoTClient client1(io_service,
            boost::asio::ip::address::from_string("239.2.3.1"), 6969,
            "AIDTR UAV 1", "a-f-G-U-C-V-U-R");

        client.sendPositionReport(lat1[0] , lon1[0], 335.28);
        client1.sendPositionReport(lat2[0] , lon2[0], 345);
        this_thread::sleep_for(std::chrono::milliseconds(10000));

        for (int i = 0; i < 2; i++) {
            int n = 200;
            double deltaLat1 = (lat1[i + 1] - lat1[i]) / n;
            double deltaLon1 = (lon1[i + 1] - lon1[i]) / n;
            double deltaLat2 = (lat2[i + 1] - lat2[i]) / n;
            double deltaLon2 = (lon2[i + 1] - lon2[i]) / n;

            for (int j = 0; j < n; j++) {
                client.sendPositionReport(lat1[i] + j * deltaLat1, lon1[i] + j * deltaLon1, 335.28);
                client1.sendPositionReport(lat2[i] + j * deltaLat2, lon2[i] + j * deltaLon2, 345);

                this_thread::sleep_for(std::chrono::milliseconds(125));
            }
        }
        cout << "Sent   " << client.getSendCount() << " messages." << endl
            << "Errors " << client.getErrorCount() << endl;
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}
