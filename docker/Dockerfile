FROM ros:melodic

# Install dependencies
RUN apt-get -yq update && apt-get -y install libxerces-c-dev

# Setup user
RUN useradd -m aidtr
USER aidtr
WORKDIR /home/aidtr

# Copy code
COPY --chown=aidtr:aidtr . /home/aidtr/ros-cot/

# Generate custom message source
WORKDIR ros-cot/workspace
RUN /bin/bash -c 'source /opt/ros/melodic/setup.bash && catkin_make clean && catkin_make'

# Build bridge
WORKDIR /home/aidtr/ros-cot/Bridge
RUN /bin/bash -c 'source /opt/ros/melodic/setup.bash && ./gccbuild.sh'

# Prepare environment
WORKDIR /home/aidtr
