FROM ros:melodic

# Install dependencies
RUN apt-get -yq update && apt-get -y install libxerces-c-dev libeigen3-dev

# Setup user
RUN useradd -m aidtr
USER aidtr
WORKDIR /home/aidtr

# Copy code
COPY --chown=aidtr:aidtr . /home/aidtr/ros-cot/

# Build bridge
WORKDIR ros-cot/workspace
RUN rm -rf build/ devel/
RUN /bin/bash -c 'source /opt/ros/melodic/setup.bash && catkin_make clean'
RUN /bin/bash -c 'source /opt/ros/melodic/setup.bash && catkin_make'

# Prepare environment
WORKDIR /home/aidtr

COPY --chown=1000:1000 docker/aidtr_entrypoint.sh .
ENTRYPOINT ["/home/aidtr/aidtr_entrypoint.sh"]

CMD bash
