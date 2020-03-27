# ROS <--> Cursor on Target Bridge


## Build Docker Image
The bridge can be built inside a docker image buy executing the build script in the "docker" folder:

    $ ./docker/build_image.sh

## Network Setup

The ATAK interface we are using requires Multicast to be available on the network that connects the ATAK tablet to the system that runs this bridge.

## Run Bridge

To run the docker bridge, execute this command:

    $ docker run --rm -it --network=host --name=ros-cot-bridge aidtr-ros-cot-bridge roslaunch ros_cot_bridge bridge.launch

If the image was pulled from the registry, use this command:

    $ docker run --rm -it --network=host --name=ros-cot-bridge unicorn.rec.ri.cmu.edu:5000/aidtr-ros-cot-bridge:unstable roslaunch ros_cot_bridge bridge.launch



