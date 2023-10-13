from PIL import Image
import random
import string

random_junk = "".join(random.choices(string.ascii_letters, k=512))

flag = "TUDCTF{m4m4-1m-a-cr1min4l}"
i = 0
junk_bin = ''.join(format(ord(x), '08b') for x in random_junk)
flag_bin = ''.join(format(ord(x), '08b') for x in flag)

data = junk_bin + flag_bin
with Image.open("base.bmp") as img:
    width, height = img.size
    for y in range(0, height):
        for x in range(0, width):
            pixel = list(img.getpixel((x, y)))
            for n in range(0,3):
                if i < len(data):
                    pixel[n] = pixel[n] & ~1 | int(data[i])
                    i+=1
            img.putpixel((x,y), tuple(pixel))
    img.save("caterpillar.bmp", "BMP")
