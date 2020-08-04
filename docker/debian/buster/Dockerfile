FROM debian:buster
MAINTAINER Dmytro Iakovliev "dmytro.iakovliev@zodiacsystems.com"
LABEL version="0.0.1"

RUN apt-get update && apt-get install -y --force-yes \
    pkg-config \
    git \
    gcc \
    g++ \
    cmake \
    python2-dev \
    python3-dev \
    libssl-dev \
    libcurl4-gnutls-dev \
    libboost-program-options-dev \
    libboost-python-dev \
    libboost-random-dev \
    libboost-test-dev
