#!/bin/bash

g++ ROSCOTBridge.cpp ../src/Messaging/XmlMessagingBase.cpp \
	-I ../workspace/devel/include \
	-I/opt/ros/melodic/include/ \
	-I../include -I../../dependencies/xerces/xerces-c-3.2.2/src \
	-I../../dependencies/xerces/src \
	-L/opt/ros/melodic/lib/ \
	-L../workspace/devel/lib/ \
	-lroscpp -lrostime -lrosconsole -lroscpp_serialization -lboost_system -lxerces-c-3.2 \
	-o roscot_bridge
