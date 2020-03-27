#!/bin/bash
set -e

setup_script="${HOME}/ros-cot/workspace/devel/setup.bash"
if [[ ! -f "${setup_script}" ]]; then
    setup_script="/opt/ros/${ROS_DISTRO}/setup.bash"
fi

source "${setup_script}"
exec "$@"
