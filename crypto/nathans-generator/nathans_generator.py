#!/usr/bin/python3

from flag import FLAG

from Crypto.PublicKey import ECC
from Crypto.Hash import SHA256
import binascii
from os import urandom

class NathansGenerator:
    def __init__(self, initial_state):
        self.P = ECC.EccPoint(25126791430396947778309676245794192442104313592348125031060671325914583516853,
                              75401441307929008727340002516952638952542672268873165933107957759668331067183,
                              curve='p256')
        self.Q = ECC.EccPoint(84409546476859015463528105749783723241651323619574584631155365312318232362937,
                              57134560974230746446108604306074507204506763026268974516387545326464736650264,
                              curve='p256')
        self.state = initial_state

    def next(self):
        output = (self.state * self.Q).x
        self.state = (self.state * self.P).x
        return output

gen = NathansGenerator(int.from_bytes(urandom(32), byteorder='little'))

message = f"The encrypted flag is: {FLAG}".encode('utf-8')

c = []
for i in range(len(message) // 32):
    num = gen.next().to_bytes(32, 'little')
    part = message[i*32:(i+1)*32]
    c += [k^p for k, p in zip(num, part)]

print(' '.join(map(str, c)))
