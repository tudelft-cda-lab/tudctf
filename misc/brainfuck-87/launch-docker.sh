#!/bin/sh
docker build -t bfjit87 --build-arg FLAG=$FLAG .
socat TCP-LISTEN:1387,reuseaddr,fork EXEC:"docker run --rm -i --memory=256m --cpus=0.5 bfjit87"
