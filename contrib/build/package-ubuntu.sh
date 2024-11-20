#!/usr/bin/env bash
docker build -f contrib/build/Dockerfile . -t lincity-ng-build-env --network=host && \
docker run -v .:/home/ubuntu/lincity-ng -u $(id -u):$(id -g) --network=host lincity-ng-build-env
