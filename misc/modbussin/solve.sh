#!/bin/sh
mbpoll 158.180.233.240 -c 100 -1 | grep -Po ":\s+\K\d+$" | xargs -I $ awk 'BEGIN{printf "%c", $}'

