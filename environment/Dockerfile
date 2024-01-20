ARG copy_dependencies_during_install=on

FROM ubuntu:22.04 AS build_env
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake-curses-gui \
    gdb \
    vim \
    wget \
    libboost-test-dev \
    libopenblas-dev \
    mpich

FROM ubuntu:22.04 AS test_env_copy_on
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    mpich

FROM ubuntu:22.04 AS test_env_copy_off
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    mpich \
    libopenblas-dev

FROM build_env AS cpptemplate_build
COPY . /opt/cpptemplate
ARG copy_dependencies_during_install
RUN cmake \
        -B /opt/cpptemplate/build \
        -S /opt/cpptemplate \
        -D copy_dependencies_during_install=$copy_dependencies_during_install \
        -D CMAKE_BUILD_TYPE=Release \
        -D CMAKE_INSTALL_PREFIX=/opt/cpptemplate-install && \
    cmake --build /opt/cpptemplate/build && \
    /opt/cpptemplate/build/cpptemplate_test && \
    cd /opt/cpptemplate/build && make install

FROM test_env_copy_${copy_dependencies_during_install} AS cpptemplate_test
COPY --from=cpptemplate_build /opt/cpptemplate-install /opt/cpptemplate
COPY example /opt/example
RUN cmake \
        -B /opt/example/build \
        -S /opt/example \
        -D CMAKE_PREFIX_PATH=/opt && \
    cmake --build /opt/example/build && \
    mpiexec -np 2 /opt/example/build/greeter_user
