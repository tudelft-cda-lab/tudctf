FROM nimlang/nim

RUN apt update
RUN apt upgrade -y
RUN apt install -y --no-install-recommends gcc-multilib
RUN apt install -y --no-install-recommends socat

COPY brainfuck_jit.nim brainfuck_jit.nim

RUN nim c -d:release --cpu:i386 -t:-m32 -l:-m32 brainfuck_jit.nim

ARG FLAG

RUN echo $FLAG > $(mktemp "flag-XXXXXXXXXXXX.txt")

CMD timeout 5m ./brainfuck_jit
