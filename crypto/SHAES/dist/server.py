from Crypto.Cipher import AES
from Crypto.Hash import SHA256
from Crypto.Util.Padding import pad, unpad

from flag import FLAG

import os

iv = b"0123456789abcdef"
max_length = 12345678

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

def check_guess(guess, secret):
    if len(guess) != len(secret):
        return False
    
    for guessed_byte, secret_byte in zip(guess, secret):
        key = os.urandom(6)
        if encrypt(guessed_byte.to_bytes(length=max_length, byteorder="little"), key) != encrypt(secret_byte.to_bytes(length=max_length, byteorder="little"), key):
            return False
    
    return True


if __name__ == "__main__":
    # Generate 8 random digits
    secret = str(int.from_bytes(os.urandom(4), byteorder="little"))[:8].encode()

    key = os.urandom(6)

    encrypted_flag = encrypt(FLAG, key)
    print("Only nation states with 512 bit AES cracking capabilities can read this:", encrypted_flag.hex())

    message = secret + b", only a real genius could guess my secret."
    encrypted_secret = encrypt(message, key)
    print("I'll leave this message here so you know I won't change my secret, now guess it (but you only get 100 attempts lol):", encrypted_secret.hex())

    for _ in range(100):
        try:
            guess = input().encode()
            if check_guess(guess, secret):
                print("Omg you guessed it!")
            else:
                print("Nope.")
        except:
            print("Error.")
            break
    else:
        print("Bye bye!")
