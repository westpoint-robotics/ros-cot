# CMakeList.txt : CMake project for Bridge, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Bridge")

include_directories("../include"  $ENV{BOOST_DIR} $ENV{XERCES_DIR}/out/install/x64-Debug/include)
link_directories($ENV{XERCES_DIR}/out/install/x64-Debug/lib $ENV{BOOST_DIR}/bin.v2/libs/regex/build/msvc-14.2/debug/address-model-64/link-static/threading-multi $ENV{BOOST_DIR}/bin.v2/libs/date_time/build/msvc-14.2/debug/address-model-64/link-static/threading-multi)
add_library(MXMLMB "../src/Messaging/XmlMessagingBase.cpp")

# Add source to this project's executable.
add_executable (Bridge "Bridge.cpp")
target_link_libraries(Bridge PUBLIC "xerces-c_3D.lib" MXMLMB)

# TODO: Add tests and install targets if needed.
