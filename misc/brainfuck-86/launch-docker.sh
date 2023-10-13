#!/bin/sh
docker build -t bfjit86 --build-arg FLAG=$FLAG .
socat TCP-LISTEN:1386,reuseaddr,fork EXEC:"docker run --rm -i --memory=256m --cpus=0.5 bfjit86"
