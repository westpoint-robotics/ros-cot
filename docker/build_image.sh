#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"


docker build -t aidtr-ros-cot-bridge -f "${SCRIPT_DIR}/Dockerfile" ${SCRIPT_DIR}/..

