#!/usr/bin/python3

from flag import FLAG

from Crypto.PublicKey import ECC
from Crypto.Hash import SHA256
from Crypto.Math._IntegerGMP import IntegerGMP
import binascii

da = int.from_bytes(FLAG.encode(), byteorder = "big")

gx = 48439561293906451759052585252797914202762949526041747995844080717082404635286
gy = 36134250956749795798585127919587881956611106672985015071877198253568414405109
n = 115792089210356248762697446949407573529996955224135760342422259061068512044369

g = ECC.EccPoint(gx,gy)

s = input("Message to be signed: ")

h = SHA256.new(data=s.encode())

ed = h.digest()

e = int.from_bytes(ed, byteorder = "big")

ln = n.bit_length()

diff = 256 - ln

z = e >> diff

k = binascii.crc32(FLAG.encode('utf8'))

p1 = g * k

(x1, y1) = p1.xy

r = x1 % n

if r == 0:
	print("Could not sign this message")
	exit()

kinv = IntegerGMP(pow(k, -1, n))


s = kinv * (IntegerGMP(z) + r * da)

if s == 0:
	print("Could not sign this message")
	exit()
	
print("The signature for your message is: \n ({}, {})".format(r, s))
