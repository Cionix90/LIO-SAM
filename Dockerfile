FROM osrf/ros:jazzy-desktop-full-noble

RUN apt-get update \
    && apt-get install -y curl \
    && curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | apt-key add - \
    && apt-get update \
    && apt install -y python3-colcon-common-extensions \
    && apt-get install -y ros-jazzy-navigation2 \
    && apt-get install -y ros-jazzy-robot-localization \
    && apt-get install -y ros-jazzy-robot-state-publisher 
RUN apt-get update \
    &&  apt-get install -y libpcl-dev \
    && apt-get install -y ros-jazzy-perception-pcl \
  	&& apt-get install -y ros-jazzy-pcl-msgs \
    && apt-get install -y ros-jazzy-pcl-ros \
    && apt-get install -y ros-jazzy-pcl-conversions \
  	&& apt-get install -y ros-jazzy-vision-opencv \
  	&& apt-get install -y ros-jazzy-xacro \
      && apt-get install -y ros-jazzy-gtsam \
    && rm -rf /var/lib/apt/lists/*
RUN apt-get update \
    && apt-get install -y ros-jazzy-rosbag2-storage-mcap \
    && apt-get install -y ros-jazzy-plotjuggler-ros \
    && apt-get install -y ros-jazzy-rmw-cyclonedds-cpp \
    && apt-get clean
RUN apt-get upgrade -y 
# RUN apt-get update \
#     && apt install -y software-properties-common \
#     && add-apt-repository -y ppa:borglab/gtsam-release-4.1 \
#     && apt-get update \
#     && apt install -y libgtsam-dev libgtsam-unstable-dev \
#     && rm -rf /var/lib/apt/lists/*


# Create a new user
ARG USERNAME=ros
ARG USER_UID=1000
ARG USER_GID=${USER_UID}
RUN if id -u ${USER_UID} ; then userdel `id -un ${USER_UID}` ; fi
RUN groupadd --gid ${USER_GID} ${USERNAME} 
RUN useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME} \
    && apt-get update \
    && apt-get install -y sudo \
    && echo ${USERNAME} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME}

#Change HOME environment variable
ENV HOME=/home/${USERNAME}
# Choose to run as user
ENV USER=${USERNAME}

USER ${USERNAME}

ARG WORKSPACE=docker_navigation
WORKDIR /home/ros/${WORKSPACE}

RUN echo "source /opt/ros/jazzy/setup.bash" >> ~/.bashrc

RUN sudo ln -s /usr/include/eigen3/Eigen /usr/include/Eigen

ENTRYPOINT ["/ros_entrypoint.sh"]