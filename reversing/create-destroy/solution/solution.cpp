#include <string.h>
#include <cinttypes>
#include <cstdio>
#include <vector>
#include <string>

uint8_t sbox[256] = {
    0,42, 232, 16, 75, 191, 48, 23, 167, 166, 80, 135, 150, 244, 49, 46, 241, 133, 125, 238, 231, 90, 33, 249, 174, 107, 221, 178, 210, 111, 230, 237, 171, 212, 29, 216, 83, 71, 203, 196, 234, 91, 79, 38, 219, 3, 47, 57, 165, 88, 185, 96, 32, 137, 147, 205, 250, 105, 127, 36, 45, 188, 184, 218, 72, 158, 21, 7, 73, 223, 159, 143, 39, 195, 209, 146, 193, 179, 202, 168, 28, 35, 119, 189, 40, 78, 152, 43, 2, 190, 229, 208, 93, 181, 144, 253, 110, 246, 239, 13, 187, 66, 153, 160, 177, 154, 100, 163, 51, 225, 82, 12, 20, 180, 15, 113, 176, 170, 214, 123, 108, 4, 139, 183, 233, 161, 251, 252, 74, 140, 243, 222, 124, 226, 63, 60, 19, 254, 114, 141, 175, 26, 89, 54, 142, 97, 81, 25, 84, 148, 9, 103, 169, 134, 50, 138, 59, 186, 52, 69, 1, 132, 204, 95, 61, 116, 131, 156, 227, 128, 164, 211, 18, 151, 121, 112, 217, 245, 220, 235, 130, 6, 68, 77, 213, 118, 94, 242, 55, 37, 70, 182, 106, 67, 31, 145, 14, 255, 102, 173, 115, 192, 126, 53, 199, 17, 228, 10, 172, 11, 92, 224, 56, 194, 129, 157, 64, 8, 101, 27, 122, 236, 22, 65, 162, 117, 109, 44, 206, 24, 41, 248, 200, 76, 34, 207, 5, 62, 30, 98, 247, 99, 215, 120, 104, 86, 136, 240, 197, 155, 149, 201, 87, 198, 85, 58
};
uint8_t reverse_sbox[256];
uint32_t diffusion[32] = {
    1272574999, 2630805305, 161373853, 645521172,
    284212197, 1637011332, 1895086301, 2703316953,
    2861043278, 3568928319, 1419534181, 1465751309,
    255268089, 4051162465, 3920189407, 1407341585,
    3202000953, 2791428421, 2681579712, 3746954981,
    14109939, 2313381186, 3971427433, 617368740,
    2154636012, 3045426417, 2263960451, 1398601743,
    2812587939, 2844197230, 3433811774, 3421363560};

uint8_t remains[32] = {
    0xfd, 0xf2, 0x76, 0x6b, 0x57, 0x4f, 0x7a, 0x37, 0x79, 0x9e, 0x27, 0x8, 0xf5, 0x17, 0xf4, 0x8b, 0x62, 0x32, 0x9b, 0x96, 0x24, 0x4d, 0x72, 0xfd, 0x1a, 0xc8, 0x4b, 0x78, 0xa, 0x8e, 0x40, 0xca
    };

std::vector<std::vector<uint8_t>> matrix =
{
{1,1,1,0,1,0,0,0,0,0,1,0,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,0,0,1,0,},
{1,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,1,},
{1,0,1,1,1,0,0,1,0,1,1,1,1,0,1,0,0,1,1,1,1,0,0,1,1,0,0,1,0,0,0,0,},
{0,0,1,0,1,0,0,0,1,1,1,1,1,0,1,1,1,0,0,1,1,1,1,0,0,1,1,0,0,1,0,0,},
{1,0,1,0,0,1,1,1,1,1,0,1,1,1,0,1,0,0,0,0,1,1,1,1,0,0,0,0,1,0,0,0,},
{0,0,1,0,0,0,0,1,1,1,1,1,0,0,1,1,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,},
{1,0,1,1,1,0,1,1,0,0,0,1,1,1,0,1,0,0,1,0,1,1,1,1,0,0,0,0,1,1,1,0,},
{1,0,0,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,0,0,0,1,0,0,1,0,0,0,0,1,0,1,},
{0,1,1,1,0,0,1,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,},
{1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,0,1,0,0,1,0,1,0,1,1,},
{1,0,1,0,0,1,1,0,1,1,1,1,1,0,1,0,0,0,1,1,1,0,0,1,0,0,1,0,1,0,1,0,},
{1,0,1,1,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,},
{1,0,0,1,1,1,1,1,0,0,1,0,1,0,0,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,0,0,},
{1,0,0,0,0,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,0,0,1,1,1,1,},
{1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,0,1,0,1,0,1,1,0,0,1,0,1,1,1,},
{1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,1,0,0,0,1,1,1,1,1,0,0,1,0,1,0,},
{1,0,0,1,1,1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,},
{1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1,1,0,0,0,0,1,1,0,0,1,1,0,0,1,0,1,},
{0,0,0,0,0,0,1,1,0,0,0,1,0,1,0,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,0,1,},
{1,0,1,0,0,1,1,1,0,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,},
{1,1,0,0,1,1,1,1,0,0,1,1,0,0,1,0,1,1,1,0,1,0,1,1,0,0,0,0,0,0,0,0,},
{0,1,0,0,0,0,1,0,1,0,1,0,0,1,1,0,1,1,0,0,0,1,1,1,1,0,0,1,0,0,0,1,},
{1,0,0,1,0,1,1,0,0,0,1,1,0,1,0,0,1,1,1,0,1,1,0,1,0,0,1,1,0,1,1,1,},
{0,0,1,0,0,1,0,1,0,0,1,1,0,0,1,0,0,0,1,1,0,0,1,1,0,0,1,0,0,1,0,0,},
{0,0,1,1,0,1,1,1,0,1,0,0,0,1,0,0,1,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,},
{1,0,0,0,1,1,1,1,0,0,1,0,0,0,0,1,1,0,1,0,0,0,0,1,1,0,1,0,1,1,0,1,},
{1,1,0,0,0,0,0,1,1,1,0,1,0,0,1,0,1,0,0,0,1,1,1,1,0,1,1,0,0,0,0,1,},
{1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1,0,1,0,1,1,0,0,1,0,1,0,},
{1,1,0,0,0,1,0,1,1,1,1,1,0,1,0,1,0,0,1,0,0,1,0,1,1,1,1,0,0,1,0,1,},
{0,1,1,1,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,0,0,1,0,1,0,1,},
{0,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,1,1,},
{1,1,1,1,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,1,0,1,0,0,1,1,},
};

std::vector<uint8_t> row_xor(const std::vector<uint8_t> &a, const std::vector<uint8_t> &b) {
	std::vector<uint8_t> r;
	
	for(int i = 0; i < a.size(); i++)
		r.push_back(a[i] ^ b[i]);
	
	return r;
}

void gaussian(std::vector<std::vector<uint8_t>> &g) {
	int i, j;
	
	for(i = 0; i < g.size() - 1; i++) {
		for(j = i + 1; j < g.size() && g[i][i] == 0; j++) {
			if(g[j][i] == 1) {
				g[i] = row_xor(g[i], g[j]);
				printf("output[%u] ^= output[%u];\n", i, j);
			}
		}
		
		for(j = i + 1; j < g.size(); j++) {
			if(g[j][i] == 1) {
				g[j] = row_xor(g[i], g[j]);
				printf("output[%u] ^= output[%u];\n", j, i);
			}
		}
	}
}

void final_row(std::vector<std::vector<uint8_t>> &g) {
	
	for(int i = g.size() - 1; i >= 0; i--) {
		printf("output[%u] ^= ", i); 
		for(int j = i + 1; j < g.size(); j++) {
			if(g[i][j] == 1)
				printf("output[%u] ^ ", j);
		}
		printf(";\n");
	}
}


void reverse_xor_part(uint8_t chars[32], uint8_t (&output)[32]) {

    for (int i = 0; i< 32; i++) {
        output[i] = chars[i];
    }

    output[1] ^= output[0];
    output[2] ^= output[0];
    output[4] ^= output[0];
    output[6] ^= output[0];
    output[7] ^= output[0];
    output[9] ^= output[0];
    output[10] ^= output[0];
    output[11] ^= output[0];
    output[12] ^= output[0];
    output[13] ^= output[0];
    output[14] ^= output[0];
    output[15] ^= output[0];
    output[16] ^= output[0];
    output[17] ^= output[0];
    output[19] ^= output[0];
    output[20] ^= output[0];
    output[22] ^= output[0];
    output[25] ^= output[0];
    output[26] ^= output[0];
    output[27] ^= output[0];
    output[28] ^= output[0];
    output[31] ^= output[0];
    output[2] ^= output[1];
    output[4] ^= output[1];
    output[6] ^= output[1];
    output[7] ^= output[1];
    output[8] ^= output[1];
    output[10] ^= output[1];
    output[11] ^= output[1];
    output[12] ^= output[1];
    output[13] ^= output[1];
    output[15] ^= output[1];
    output[16] ^= output[1];
    output[17] ^= output[1];
    output[19] ^= output[1];
    output[21] ^= output[1];
    output[22] ^= output[1];
    output[25] ^= output[1];
    output[29] ^= output[1];
    output[30] ^= output[1];
    output[3] ^= output[2];
    output[4] ^= output[2];
    output[5] ^= output[2];
    output[6] ^= output[2];
    output[10] ^= output[2];
    output[11] ^= output[2];
    output[17] ^= output[2];
    output[19] ^= output[2];
    output[20] ^= output[2];
    output[21] ^= output[2];
    output[23] ^= output[2];
    output[24] ^= output[2];
    output[26] ^= output[2];
    output[28] ^= output[2];
    output[3] ^= output[4];
    output[4] ^= output[3];
    output[9] ^= output[3];
    output[10] ^= output[3];
    output[13] ^= output[3];
    output[14] ^= output[3];
    output[15] ^= output[3];
    output[17] ^= output[3];
    output[19] ^= output[3];
    output[21] ^= output[3];
    output[24] ^= output[3];
    output[25] ^= output[3];
    output[27] ^= output[3];
    output[31] ^= output[3];
    output[10] ^= output[4];
    output[11] ^= output[4];
    output[13] ^= output[4];
    output[17] ^= output[4];
    output[19] ^= output[4];
    output[22] ^= output[4];
    output[26] ^= output[4];
    output[27] ^= output[4];
    output[28] ^= output[4];
    output[30] ^= output[4];
    output[31] ^= output[4];
    output[7] ^= output[5];
    output[8] ^= output[5];
    output[9] ^= output[5];
    output[11] ^= output[5];
    output[13] ^= output[5];
    output[15] ^= output[5];
    output[17] ^= output[5];
    output[22] ^= output[5];
    output[27] ^= output[5];
    output[28] ^= output[5];
    output[30] ^= output[5];
    output[7] ^= output[6];
    output[8] ^= output[6];
    output[9] ^= output[6];
    output[12] ^= output[6];
    output[15] ^= output[6];
    output[18] ^= output[6];
    output[20] ^= output[6];
    output[22] ^= output[6];
    output[27] ^= output[6];
    output[29] ^= output[6];
    output[9] ^= output[7];
    output[10] ^= output[7];
    output[12] ^= output[7];
    output[15] ^= output[7];
    output[17] ^= output[7];
    output[18] ^= output[7];
    output[22] ^= output[7];
    output[24] ^= output[7];
    output[26] ^= output[7];
    output[28] ^= output[7];
    output[30] ^= output[7];
    output[9] ^= output[8];
    output[11] ^= output[8];
    output[12] ^= output[8];
    output[13] ^= output[8];
    output[16] ^= output[8];
    output[19] ^= output[8];
    output[20] ^= output[8];
    output[22] ^= output[8];
    output[23] ^= output[8];
    output[27] ^= output[8];
    output[9] ^= output[16];
    output[16] ^= output[9];
    output[18] ^= output[9];
    output[20] ^= output[9];
    output[28] ^= output[9];
    output[11] ^= output[10];
    output[13] ^= output[10];
    output[16] ^= output[10];
    output[17] ^= output[10];
    output[20] ^= output[10];
    output[21] ^= output[10];
    output[27] ^= output[10];
    output[28] ^= output[10];
    output[29] ^= output[10];
    output[30] ^= output[10];
    output[31] ^= output[10];
    output[11] ^= output[12];
    output[12] ^= output[11];
    output[13] ^= output[11];
    output[16] ^= output[11];
    output[17] ^= output[11];
    output[19] ^= output[11];
    output[21] ^= output[11];
    output[23] ^= output[11];
    output[24] ^= output[11];
    output[28] ^= output[11];
    output[30] ^= output[11];
    output[31] ^= output[11];
    output[17] ^= output[12];
    output[22] ^= output[12];
    output[23] ^= output[12];
    output[24] ^= output[12];
    output[27] ^= output[12];
    output[28] ^= output[12];
    output[29] ^= output[12];
    output[13] ^= output[14];
    output[14] ^= output[13];
    output[16] ^= output[13];
    output[18] ^= output[13];
    output[19] ^= output[13];
    output[20] ^= output[13];
    output[21] ^= output[13];
    output[25] ^= output[13];
    output[27] ^= output[13];
    output[28] ^= output[13];
    output[29] ^= output[13];
    output[31] ^= output[13];
    output[16] ^= output[14];
    output[17] ^= output[14];
    output[20] ^= output[14];
    output[21] ^= output[14];
    output[23] ^= output[14];
    output[26] ^= output[14];
    output[27] ^= output[14];
    output[28] ^= output[14];
    output[29] ^= output[14];
    output[31] ^= output[14];
    output[17] ^= output[15];
    output[18] ^= output[15];
    output[20] ^= output[15];
    output[21] ^= output[15];
    output[24] ^= output[15];
    output[26] ^= output[15];
    output[27] ^= output[15];
    output[28] ^= output[15];
    output[29] ^= output[15];
    output[16] ^= output[18];
    output[18] ^= output[16];
    output[20] ^= output[16];
    output[25] ^= output[16];
    output[31] ^= output[16];
    output[20] ^= output[17];
    output[23] ^= output[17];
    output[24] ^= output[17];
    output[28] ^= output[17];
    output[29] ^= output[17];
    output[19] ^= output[18];
    output[20] ^= output[18];
    output[21] ^= output[18];
    output[22] ^= output[18];
    output[24] ^= output[18];
    output[26] ^= output[18];
    output[30] ^= output[18];
    output[31] ^= output[18];
    output[20] ^= output[19];
    output[21] ^= output[19];
    output[22] ^= output[19];
    output[24] ^= output[19];
    output[25] ^= output[19];
    output[26] ^= output[19];
    output[27] ^= output[19];
    output[30] ^= output[19];
    output[31] ^= output[19];
    output[22] ^= output[20];
    output[23] ^= output[20];
    output[24] ^= output[20];
    output[29] ^= output[20];
    output[21] ^= output[24];
    output[24] ^= output[21];
    output[26] ^= output[21];
    output[28] ^= output[21];
    output[22] ^= output[27];
    output[27] ^= output[22];
    output[28] ^= output[22];
    output[29] ^= output[22];
    output[30] ^= output[22];
    output[31] ^= output[22];
    output[23] ^= output[26];
    output[26] ^= output[23];
    output[28] ^= output[23];
    output[29] ^= output[23];
    output[27] ^= output[24];
    output[31] ^= output[24];
    output[25] ^= output[26];
    output[26] ^= output[25];
    output[28] ^= output[26];
    output[27] ^= output[29];
    output[29] ^= output[27];
    output[30] ^= output[27];
    output[31] ^= output[27];
    output[28] ^= output[30];
    output[30] ^= output[28];
    output[30] ^= output[31];
    output[31] ^= output[30];
    output[31] ^= 0;
    output[30] ^= 0;
    output[29] ^= output[30];
    output[28] ^= output[30];
    output[27] ^= output[29] ^ output[31];
    output[26] ^= output[28];
    output[25] ^= output[27];
    output[24] ^= output[30] ^ output[31];
    output[23] ^= output[26] ^ output[28] ^ output[30] ^ output[31];
    output[22] ^= output[26] ^ output[29] ^ output[30];
    output[21] ^= output[22] ^ output[24] ^ output[27] ^ output[30];
    output[20] ^= output[24];
    output[19] ^= output[20] ^ output[23];
    output[18] ^= output[20] ^ output[21] ^ output[24] ^ output[25] ^ output[28] ^ output[29] ^ output[31];
    output[17] ^= output[18] ^ output[23] ^ output[24] ^ output[25] ^ output[26] ^ output[30] ^ output[31];
    output[16] ^= output[18] ^ output[19] ^ output[20] ^ output[21] ^ output[22] ^ output[26] ^ output[27] ^ output[29] ^ output[31];
    output[15] ^= output[18] ^ output[23] ^ output[27] ^ output[28] ^ output[30];
    output[14] ^= output[15] ^ output[17] ^ output[18] ^ output[19] ^ output[20] ^ output[22] ^ output[23] ^ output[24] ^ output[25] ^ output[28] ^ output[29];
    output[13] ^= output[14] ^ output[15] ^ output[16] ^ output[19] ^ output[21] ^ output[22] ^ output[23] ^ output[24] ^ output[25] ^ output[27] ^ output[28] ^ output[29];
    output[12] ^= output[15] ^ output[18] ^ output[20] ^ output[21] ^ output[24] ^ output[26] ^ output[27] ^ output[28];
    output[11] ^= output[13] ^ output[16] ^ output[17] ^ output[18] ^ output[20] ^ output[22] ^ output[23] ^ output[24] ^ output[25] ^ output[26] ^ output[28] ^ output[29] ^ output[31];
    output[10] ^= output[14] ^ output[15] ^ output[16] ^ output[17] ^ output[19] ^ output[21] ^ output[26] ^ output[28] ^ output[29] ^ output[30] ^ output[31];
    output[9] ^= output[10] ^ output[11] ^ output[18] ^ output[19] ^ output[20] ^ output[22] ^ output[23] ^ output[25] ^ output[27] ^ output[29];
    output[8] ^= output[10] ^ output[11] ^ output[15] ^ output[16] ^ output[17] ^ output[18] ^ output[20] ^ output[21] ^ output[28] ^ output[29] ^ output[30] ^ output[31];
    output[7] ^= output[13] ^ output[16] ^ output[17] ^ output[18] ^ output[22] ^ output[28] ^ output[29] ^ output[31];
    output[6] ^= output[9] ^ output[10] ^ output[13] ^ output[14] ^ output[15] ^ output[17] ^ output[19] ^ output[21] ^ output[22] ^ output[24] ^ output[27] ^ output[28] ^ output[29] ^ output[30];
    output[5] ^= output[9] ^ output[10] ^ output[12] ^ output[13] ^ output[14] ^ output[17] ^ output[22] ^ output[23] ^ output[26] ^ output[28] ^ output[29] ^ output[30] ^ output[31];
    output[4] ^= output[5] ^ output[7] ^ output[9] ^ output[10] ^ output[13] ^ output[14] ^ output[16] ^ output[19] ^ output[21] ^ output[24] ^ output[25] ^ output[28] ^ output[29] ^ output[31];
    output[3] ^= output[6] ^ output[7] ^ output[8] ^ output[9] ^ output[15] ^ output[16] ^ output[17] ^ output[18] ^ output[22] ^ output[25] ^ output[27] ^ output[29] ^ output[31];
    output[2] ^= output[5] ^ output[7] ^ output[8] ^ output[11] ^ output[12] ^ output[13] ^ output[15] ^ output[20] ^ output[22] ^ output[24] ^ output[26] ^ output[28] ^ output[31];
    output[1] ^= output[2] ^ output[3] ^ output[5] ^ output[8] ^ output[9] ^ output[12] ^ output[16] ^ output[17] ^ output[18] ^ output[20] ^ output[24] ^ output[25] ^ output[26] ^ output[28] ^ output[30] ^ output[31];
    output[0] ^= output[1] ^ output[2] ^ output[4] ^ output[10] ^ output[12] ^ output[13] ^ output[14] ^ output[15] ^ output[16] ^ output[19] ^ output[20] ^ output[22] ^ output[23] ^ output[24] ^ output[25] ^ output[27] ^ output[30];
}

void reverse_sub(uint8_t chars[32], uint8_t (&output)[32]) {

    for (int i = 0; i < 32; i++) {
        output[i] = reverse_sbox[chars[i]];
    }

}

void reverse_once(uint8_t chars[32], uint8_t (&output)[32]) {

    uint8_t tmp[32];
    reverse_xor_part(chars, tmp);

    for (int i = 0; i< 32; i++) {
        printf("%#2x ", tmp[i]);
    }
    printf("\n");

    reverse_sub(tmp, output);

}


int main(int argc, char *argv[])
{

    // reverse s-box

    for (int i = 0; i < 256; i++)
    {
        reverse_sbox[sbox[i]] = i;
    }


    // reverse last step

    std::vector<std::vector<uint8_t>> possible_chars;

    uint8_t possible_string[32];

    reverse_sub(remains, possible_string);


    // gaussian(matrix);

    // final_row(matrix);



    uint8_t final[32];
    for (int round = 7; round > 0; round--) {
        reverse_once(possible_string, final);
        for (int i = 0; i < 32; i++) {
            possible_string[i] = final[i];
        }
    }

    for (int i = 0; i < 32; i++) {
        printf("%c ", possible_string[i]);
    }

    printf("\n");


    


    return 0;
}

