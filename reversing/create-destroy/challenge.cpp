#include <string.h>
#include <cinttypes>
#include <cstdio>
#include <sys/ptrace.h>

uint8_t sbox[256] = {
    0,42, 232, 16, 75, 191, 48, 23, 167, 166, 80, 135, 150, 244, 49, 46, 241, 133, 125, 238, 231, 90, 33, 249, 174, 107, 221, 178, 210, 111, 230, 237, 171, 212, 29, 216, 83, 71, 203, 196, 234, 91, 79, 38, 219, 3, 47, 57, 165, 88, 185, 96, 32, 137, 147, 205, 250, 105, 127, 36, 45, 188, 184, 218, 72, 158, 21, 7, 73, 223, 159, 143, 39, 195, 209, 146, 193, 179, 202, 168, 28, 35, 119, 189, 40, 78, 152, 43, 2, 190, 229, 208, 93, 181, 144, 253, 110, 246, 239, 13, 187, 66, 153, 160, 177, 154, 100, 163, 51, 225, 82, 12, 20, 180, 15, 113, 176, 170, 214, 123, 108, 4, 139, 183, 233, 161, 251, 252, 74, 140, 243, 222, 124, 226, 63, 60, 19, 254, 114, 141, 175, 26, 89, 54, 142, 97, 81, 25, 84, 148, 9, 103, 169, 134, 50, 138, 59, 186, 52, 69, 1, 132, 204, 95, 61, 116, 131, 156, 227, 128, 164, 211, 18, 151, 121, 112, 217, 245, 220, 235, 130, 6, 68, 77, 213, 118, 94, 242, 55, 37, 70, 182, 106, 67, 31, 145, 14, 255, 102, 173, 115, 192, 126, 53, 199, 17, 228, 10, 172, 11, 92, 224, 56, 194, 129, 157, 64, 8, 101, 27, 122, 236, 22, 65, 162, 117, 109, 44, 206, 24, 41, 248, 200, 76, 34, 207, 5, 62, 30, 98, 247, 99, 215, 120, 104, 86, 136, 240, 197, 155, 149, 201, 87, 198, 85, 58
};
uint32_t diffusion[32] = {
    1272574999, 2630805305, 161373853, 645521172,
    284212197, 1637011332, 1895086301, 2703316953,
    2861043278, 3568928319, 1419534181, 1465751309,
    255268089, 4051162465, 3920189407, 1407341585,
    3202000953, 2791428421, 2681579712, 3746954981,
    14109939, 2313381186, 3971427433, 617368740,
    2154636012, 3045426417, 2263960451, 1398601743,
    2812587939, 2844197230, 3433811774, 3421363567
};


#pragma GCC push_options
#pragma GCC optimize ("O0")
void destroy(uint8_t in[33], uint8_t out[32], uint8_t sub[32], uint32_t p[32])
{
    for (uint32_t i = 0; i < 7; i++)
    {
        for (uint8_t j = 0; j < 32; j++)
        {
            out[j] = sub[in[j]];
            in[j] = 0;
        }



        for (uint8_t j = 0; j < 32; j++)
            for (uint8_t k = 0; k < 32; k++)
                in[j] ^= out[k] * ((p[j] >> k) & 1);
    }

    for (uint8_t i = 0; i < 32; i++) {
        out[i] = sbox[in[i]];
    }

}
#pragma GCC pop_options
int main(int argc, char *argv[])
{

    // if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
    //     printf("Debugging is not allowed!\n");
    //     return 1;
    // }

    uint8_t flag[33];
    // create(flag, starting, key);

    printf("Insert flag to be shredded: ");
    scanf("%32s", flag);

    for (int i = 0; i < 32; i++) {
        if (flag[i] == 0) {
            printf("Unsupported length\n");
            return 1;
        }
    }
    flag[32] = 0;

    uint8_t output[33];

    destroy(flag, output, sbox, diffusion);
    output[32] = 0;
    printf("Shredded the flag!\nHere are the remains: ");

    for (uint8_t i = 0; i < 32; i++) {
        printf("%#02x ", output[i]);
    }
    printf("\n");

    return 0;
}