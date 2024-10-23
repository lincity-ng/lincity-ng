#!/usr/bin/env bash
docker build -f contrib/build/Dockerfile . -t lincity-ng-build-env --network=host && \
docker run -v .:/home/lincity-ng -u $(id -u):$(id -g) lincity-ng-build-env
