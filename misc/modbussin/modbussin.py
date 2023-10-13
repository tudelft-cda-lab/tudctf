#!/usr/bin/python3

# Original Modbus server code by ipal0
# https://github.com/ipal0/modbus/blob/master/server.py

# Modified for TUDCTF

import socket
import _thread
from array import array
from math import ceil
from struct import unpack


def TCP(conn, addr):
	buffer = array('B', [0] * 300)
	cnt = 0
	hexOf = lambda BUFFER: ','.join([hex(i) for i in BUFFER])
	while True:
		try:
			conn.recv_into(buffer)
			TID0 = buffer[0]   #Transaction ID	to sync
			TID1 = buffer[1]   #Transaction ID 
			ID = buffer[6]	   #Unit ID
			FC = buffer[7]	   #Function Code
			mADR = buffer[8]   #Address MSB
			lADR = buffer[9]   #Address LSB
			ADR = mADR * 256 + lADR 
			LEN = 1
			if FC not in [5,6]: 
				LEN = buffer[10] * 256 + buffer[11]
			BYT = LEN * 2
			print("Received: ", hexOf(buffer[:6+buffer[5]]))
			if (FC in [1, 2, 3, 4]):  # Read Inputs or Registers
				DAT = array('B')
				if FC < 3:
					BYT = ceil(LEN / 8)  # Round off the no. of bytes
					v = 85	# send 85,86.. for bytes.
					for i in range(BYT):
						DAT.append(v)
						v = (lambda x: x + 1 if (x < 255) else 85)(v)
				else:
					DAT = array('B', bytes(LEN*2))
					print("LEN:", LEN)
					print("ADR:", ADR)

					flag = 'TUDCTF{y0u_h4ck3d_my_m0dbussin_s0lar_p4nel_4nd_st0pp3d_my_p0wer}'

					for i in range(min(LEN, len(flag)-ADR)):
						DAT[2*i+1] = ord(flag[i+ADR])
				conn.send(array('B', [TID0, TID1, 0, 0, 0, BYT + 3, ID, FC, BYT]) + DAT)
			else:
				print(f"Funtion Code {FC} Not Supported")
				conn.send(array('B', [TID0, TID1, 0, 0, 0, BYT + 3, ID, FC, BYT]))
		except Exception as e:
			print(e, "\nConnection with Client terminated")
			exit()

if __name__ == '__main__':
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.bind(('', 502))
	s.listen(1)
	print("Ready for modbussin'")
	while True:
		conn, addr = s.accept()
		print("Connected by", addr[0])
		_thread.start_new_thread(TCP, (conn, addr))

