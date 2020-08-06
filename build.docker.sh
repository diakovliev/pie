#! /bin/sh

DOCKER_DIR=$1
IMAGE_NAME=$(cat ${DOCKER_DIR}/.image_name)

docker build -t ${IMAGE_NAME} ${DOCKER_DIR}
docker run --rm -ti -v $(pwd):$(pwd) --workdir $(pwd) -u $(id -u):$(id -g) ${IMAGE_NAME}:latest make build_tree=${PWD}/_build.docker/${DOCKER_DIR} clean
docker run --rm -ti -v $(pwd):$(pwd) --workdir $(pwd) -u $(id -u):$(id -g) ${IMAGE_NAME}:latest make build_tree=${PWD}/_build.docker/${DOCKER_DIR}
# docker run --rm -ti -v $(pwd):$(pwd) --workdir $(pwd) -u $(id -u):$(id -g) ${IMAGE_NAME}:latest /bin/bash
