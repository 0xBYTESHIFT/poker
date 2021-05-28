FROM ubuntu:20.04 as builder

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow
ENV CONAN_USER_HOME /home

RUN apt update && \
    apt upgrade -y && \
    apt install -y -qq \
        ninja-build pkg-config git \
        clang-11 clang-11 python3-dev python3-pip python3 \
        python3-pip && \
    apt clean && \
    rm -rf /var/lib/apt/lists/* && \
    pip3 install --no-cache-dir conan  cmake && \
    alias python=python3

WORKDIR /app

RUN conan profile new default --detect && \
    conan profile update settings.compiler=clang default && \
    conan profile update settings.compiler.libcxx=libstdc++11 default && \
    conan profile update settings.compiler.version=11 default && \
    conan profile update env.CXX=clang++-11 default && \
    conan profile update env.CC=clang-11 default && \
    conan remote add cyberduckninja http://conan.cyberduckninja.com:9300 && \
    conan config set general.parallel_download=$(nproc) &&\
    conan config set general.cpu_count=$(nproc)

RUN git clone https://github.com/Stiffstream/restinio-conan
RUN cd restinio-conan && conan export . restinio/0.6.13@stiffstream/stable && \
        conan create . restinio/0.6.13@stiffstream/stable --build=fmt

COPY ./conanfile.txt ./

WORKDIR /app/build
RUN CONAN_LOGGING_LEVEL=debug conan install .. --build=missing -s build_type=Release
 
WORKDIR /app

RUN git clone https://github.com/wolfpld/tracy ./tracy
COPY ./api ./api
COPY ./client ./client
COPY ./tests ./tests
COPY ./service ./service
COPY ./components ./components
COPY ./CMakeLists.txt .

#make services
WORKDIR /app/build
RUN cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang-11 -DCMAKE_CXX_COMPILER=clang++-11
RUN cmake --build . --parallel
