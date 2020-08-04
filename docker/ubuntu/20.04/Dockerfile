FROM ubuntu:20.04
MAINTAINER Dmytro Iakovliev "dmytro.iakovliev@zodiacsystems.com"
LABEL version="0.0.1"

# Needed for latest ubuntu
ENV TZ=Europe/Kiev
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y --force-yes \
    pkg-config \
    git \
    gcc-8 \
    g++-8 \
    cmake \
    python3-dev \
    libssl-dev \
    libcurl4-gnutls-dev \
    libboost-program-options-dev \
    libboost-python-dev \
    libboost-random-dev \
    libboost-test-dev
