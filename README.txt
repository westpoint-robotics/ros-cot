# ROS <--> Cursor on Target Bridge


## Build Docker Image
The bridge can be built inside a docker image buy executing the build script in the "docker" folder:

    $ ./docker/build_image.sh

## Network Setup

The ATAK interface we are using requires Multicast to be available on the network that connects the ATAK tablet to the system that runs this bridge.

## Run Bridge

A ROS Master server must be running on the local system first. If you don't have one available already, execut this command to run one in Docker in the background:

    $ docker run --rm -d --network=host ros:melodic roscore


Then to run the docker bridge, execute this command:

    $ docker run --rm -it --network=host --name=ros-cot-bridge aidtr-ros-cot-bridge /home/aidtr/ros-cot/Bridge/roscot_bridge

If the image was pulled from the registry, use this command:

    $ docker run --rm -it --network=host --name=ros-cot-bridge unicorn.rec.ri.cmu.edu:5000/aidtr-ros-cot-bridge:unstable /home/aidtr/ros-cot/Bridge/roscot_bridge



