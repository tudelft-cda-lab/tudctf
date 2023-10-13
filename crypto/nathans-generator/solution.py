#!/usr/bin/python3

from flag import FLAG

from ecpy.curves import Curve, Point
import binascii
from os import urandom

with open('output.txt', 'r') as f:
    ciphertext = f.read()
ciphertext = list(map(int, ciphertext.split(' ')))

known_plaintext = "The encrypted flag is: TUDCTF{".encode('utf-8') # 30 bytes: last 2 need to be bruteforced as each block is 32 bytes


# P is a multiple of Q
curve = Curve.get_curve('secp256r1')
P = Point(25126791430396947778309676245794192442104313592348125031060671325914583516853,
          75401441307929008727340002516952638952542672268873165933107957759668331067183,
          curve)
Q = Point(84409546476859015463528105749783723241651323619574584631155365312318232362937,
          57134560974230746446108604306074507204506763026268974516387545326464736650264,
          curve)

# find e such that Q*e=P
e = 1
while True:
    if e * Q == P:
        break
    e += 1
print(f"Found e={e}: Q*{e}=P")

# bruteforce last 2 plaintext bytes
for l1 in range(256):
    if not chr(l1).isprintable(): # only printable bytes
        continue
    print(l1)
    for l2 in range(256):
        if not chr(l2).isprintable():
            continue

        first_plaintext = known_plaintext + \
                          l1.to_bytes(1, 'little') + \
                          l2.to_bytes(1, 'little')

        # recover x of point = state*Q
        num1_bytes = bytes(c^m for c, m in zip(ciphertext[:32], first_plaintext))
        num1 = int.from_bytes(num1_bytes, byteorder='little')

        # recover y of point = state*Q
        num1_y = curve.y_recover(num1)
        # if point is on curve
        if num1_y:
            point_num1 = Point(num1, num1_y, curve)

            # calculate state' = state*P = state*(e*Q) = e * (state*Q) = e * point
            internal_state_after_num1 = (point_num1 * e).x

            # generate next number = state'*Q
            num2 = (internal_state_after_num1 * Q).x.to_bytes(32, 'little')
            
            # decrypt rest of ciphertext with that number as key
            second_plaintext = bytes(k ^ c for k, c in zip(num2, ciphertext[32:]))

            if all(chr(i).isprintable() for i in second_plaintext): # ignore non-printable candidates
                print(first_plaintext + second_plaintext)

