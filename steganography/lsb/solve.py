from PIL import Image

extracted_bin = []
with Image.open("caterpillar.bmp") as img:
    width, height = img.size
    byte = []
    for x in range(0, width):
        for y in range(0, height):
            pixel = list(img.getpixel((x, y)))
            for n in range(0,3):
                extracted_bin.append(pixel[n]&1)

data = "".join([str(x) for x in extracted_bin])
data = "".join(chr(int(data[i*8:i*8+8], 2)) for i in range(len(data)//8))

result = ""
save = False
for c in data:
	if True:
		result += c
	elif save and 33 <= ord(c) <= 127:
		result += c
	elif c == 'T':
		save = True
		result += c

print(result)
