flag = "TUDCTF{g4u551an-3l1min47i0n-x0r}"

import random



sbox = list(range(1, 256))

random.shuffle(sbox)

good = False

while not good:
    good = True
    for i, v in enumerate(sbox):
        if v == i + 1:
            good = False
    if not good:
        random.shuffle(sbox)



print(sbox)

# diffusion = [1272574999, 2630805305, 161373853, 645521172,
#     284212197, 1637011332, 1895086301, 2703316953,
#     2861043278, 3568928319, 1419534181, 1465751309,
#     255268089, 4051162465, 3920189407, 1407341585,
#     3202000953, 2791428421, 2681579712, 3746954981,
#     14109939, 2313381186, 3971427433, 617368740,
#     2154636012, 3045426417, 2263960451, 1398601743,
#     2812587939, 2844197230, 3433811774, 3421363560]

# for j in range(32):
#     string = f"in[{j}] = "
#     for k in range(32):
#         if (diffusion[j] >> k) & 1 > 0:
#             string += f"out[{k}] ^ "
    
#     print(string)
    