#!/usr/bin/python3

from ecpy.curves import Curve, Point
from os import urandom

# NIST P-256
curve = Curve.get_curve('secp256r1')
n = int.from_bytes(urandom(32), byteorder='little') % curve.order

Q = curve.generator * n

P = Q * 42

print(f'Q = ({Q.x}, {Q.y})')
print(f'P = ({P.x}, {P.y})')
