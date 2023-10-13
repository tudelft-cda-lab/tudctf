from pwn import *

from Crypto.Cipher import AES
from Crypto.Hash import SHA256
from Crypto.Util.Padding import pad, unpad

import time

from tqdm import tqdm

ip = '164.92.218.155'
port = 6001

conn = remote(ip, port)

first_line = conn.recvline()
encrypted_flag_hex = first_line.split(b": ")[1].strip()
print("encrypted_flag_hex", encrypted_flag_hex)

second_line = conn.recvline()
encrypted_secret_hex = second_line.split(b": ")[1].strip()
print("encrypted_secret_hex", encrypted_secret_hex)

partial_solution = ""
for round_iter in range(8):
    longest_time = -1
    for i in range(10):
        guess = partial_solution + (str(i) * (8 - round_iter))
        send_time = time.time()

        conn.sendline(guess.encode())
        conn.recvline()
        response = conn.recvline()

        time_taken = time.time() - send_time

        if response.startswith(b"Omg you guessed it!"):
            best_guess = i
            print("Secret verified!")
            break

        if time_taken > longest_time:
            longest_time = time_taken
            best_guess = i

    partial_solution += str(best_guess)

    print("found partial solution:", partial_solution)

secret = partial_solution.encode()

print("Complete secret:", secret)

conn.close()

# Now brute force using MITM attack
message = pad(secret + b", only a real genius could guess my secret.", 16)
ciphertext = bytes.fromhex(encrypted_secret_hex.decode())

iv = b"0123456789abcdef"

dictionary = {}
for key_1_int in tqdm(range(2**24)):
    key_1 = key_1_int.to_bytes(length=3, byteorder="little")

    h = SHA256.new(key_1)
    hash_1 = h.digest()

    cipher = AES.new(hash_1, AES.MODE_CBC, iv=iv)
    found_ciphertext = cipher.encrypt(message)

    # We only take the first half just to reduce the space requirement
    dictionary[found_ciphertext[:32]] = key_1

for key_2_int in tqdm(range(2**24)):
    key_2 = key_2_int.to_bytes(length=3, byteorder="little")

    h = SHA256.new(key_2)
    hash_2 = h.digest()

    cipher = AES.new(hash_2, AES.MODE_CBC, iv=iv)
    found_ciphertext = cipher.decrypt(ciphertext)

    if found_ciphertext[:32] in dictionary:
        print("Found keys!")
        first_key = dictionary[found_ciphertext[:32]]
        second_key = key_2
        break
else:
    raise RuntimeError("Search unsuccessful")

reconstructed_key = bytes((first_key[1], first_key[2], second_key[0], first_key[0], second_key[2], second_key[1]))

print("reconstructed_key:", reconstructed_key)

def encrypt(message, key):
    message = pad(message, 16)

    h = SHA256.new(bytes((key[3], key[0], key[1])))
    hash_1 = h.digest()

    cipher = AES.new(hash_1, AES.MODE_CBC, iv=iv)
    ciphertext = cipher.encrypt(message)

    h = SHA256.new(bytes((key[2], key[5], key[4])))
    hash_2 = h.digest()

    cipher = AES.new(hash_2, AES.MODE_CBC, iv=iv)
    ciphertext = cipher.encrypt(ciphertext)

    return ciphertext

def decrypt(ciphertext, key):
    h = SHA256.new(bytes((key[2], key[5], key[4])))
    hash_2 = h.digest()

    cipher = AES.new(hash_2, AES.MODE_CBC, iv=iv)
    message = cipher.decrypt(ciphertext)

    h = SHA256.new(bytes((key[3], key[0], key[1])))
    hash_1 = h.digest()

    cipher = AES.new(hash_1, AES.MODE_CBC, iv=iv)
    message = cipher.decrypt(message)

    message = unpad(message, 16)

    return message

enrypted_flag = bytes.fromhex(encrypted_flag_hex.decode())
flag = decrypt(enrypted_flag, reconstructed_key)

print("flag:", flag)
