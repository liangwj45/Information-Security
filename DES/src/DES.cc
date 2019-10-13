#include "DES.h"

#define print(i, n, a)                 \
  for (int(i) = 0; (i) < (n); (i)++) { \
    printf("%d", (a)[(i)]);            \
  }                                    \
  printf("\n");

#define printc(c) printf("%c\n", (c))

int DES::Shift[16] = {  // 偏移
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
int DES::IP_table[64] = {  // IP 置换矩阵
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};
int DES::E_table[48] = {  // E 扩展矩阵
    32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,  8,  9,  10, 11,
    12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21,
    22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};
int DES::P_table[32] = {  // P 置换
    16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
    2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25};
int DES::IPI_table[64] = {  // IP 逆置换矩阵
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9,  49, 17, 57, 25};
int DES::PC1_table[56] = {  // 密钥第一次置换矩阵
    57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18, 10, 2,  59, 51, 43,
    35, 27, 19, 11, 3,  60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7,  62, 54,
    46, 38, 30, 22, 14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4};
int DES::PC2_table[48] = {  // 密钥第二次置换矩阵
    14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10, 23, 19, 12, 4,
    26, 8,  16, 7,  27, 20, 13, 2,  41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};
int DES::S_box[8][4][16] = {  // S 盒
    // S1
    14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 0, 15, 7, 4, 14, 2,
    13, 1, 10, 6, 12, 11, 9, 5, 3, 8, 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7,
    3, 10, 5, 0, 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,
    // S2
    15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10, 3, 13, 4, 7, 15, 2, 8,
    14, 12, 0, 1, 10, 6, 9, 11, 5, 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9,
    3, 2, 15, 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,
    // S3
    10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8, 13, 7, 0, 9, 3, 4, 6,
    10, 2, 8, 5, 14, 12, 11, 15, 1, 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5,
    10, 14, 7, 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,
    // S4
    7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15, 13, 8, 11, 5, 6, 15,
    0, 3, 4, 7, 2, 12, 1, 10, 14, 9, 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14,
    5, 2, 8, 4, 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,
    // S5
    2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9, 14, 11, 2, 12, 4, 7,
    13, 1, 5, 0, 15, 10, 3, 9, 8, 6, 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6,
    3, 0, 14, 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,
    // S6
    12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11, 10, 15, 4, 2, 7, 12,
    9, 5, 6, 1, 13, 14, 0, 11, 3, 8, 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1,
    13, 11, 6, 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,
    // S7
    4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1, 13, 0, 11, 7, 4, 9, 1,
    10, 14, 3, 5, 12, 2, 15, 8, 6, 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0,
    5, 9, 2, 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,
    // S8
    13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7, 1, 15, 13, 8, 10, 3,
    7, 4, 12, 5, 6, 11, 0, 14, 9, 2, 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13,
    15, 3, 5, 8, 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11};

/**
 * @brief 测试用的接口——编码
 *
 * @param input 输入文件（明文）路径
 * @param key 56位（7个字节）密钥
 * @param output 输出文件（密文）路径
 */
void DES::EncodeWithFile(const char* input, const char* key,
                         const char* output) {
  FILE* in = fopen(input, "r");
  FILE* out = fopen(output, "w");

  Bit keys[16][48];
  GenerateSecretKeys(key, keys);

  bool end = false, FALSE = false;
  Bit text[64], code[64];
  while (!end) {
    Read64Bit(in, text, TYPE::ENCODER, end);
    EncodeWithKeys(text, keys, code);
    Write64Bit(out, code, FALSE);
  }

  fclose(in);
  fclose(out);
}

/**
 * @brief 测试用的接口——译码
 *
 * @param input 输入文件（密文）路径
 * @param key 56位（7个字节）密钥
 * @param output 输出文件（明文）路径
 */
void DES::DecodeWithFile(const char* input, const char* key,
                         const char* output) {
  FILE* in = fopen(input, "r");
  FILE* out = fopen(output, "w");

  Bit keys[16][48];
  GenerateSecretKeys(key, keys);

  bool end = false;
  Bit text[64], code[64];
  while (!end) {
    Read64Bit(in, code, TYPE::DECODER, end);
    DecodeWithKeys(code, keys, text);
    Write64Bit(out, text, end);
  }

  fclose(in);
  fclose(out);
}

/**
 * @brief 编码
 *
 * @param text 64位明文输入
 * @param key 56位密钥
 * @param code 64位密文输出
 */
void DES::Encode(Bit text[64], Bit key[56], Bit code[64]) {
  Bit keys[16][48];
  GenerateSecretKeys(key, keys);
  EncodeWithKeys(text, keys, code);
}

/**
 * @brief 译码
 *
 * @param text 64位密文输出
 * @param key 56位密钥
 * @param code 64位明文输入
 */
void DES::Decode(Bit code[64], Bit key[56], Bit text[64]) {
  Bit keys[16][48];
  GenerateSecretKeys(key, keys);
  DecodeWithKeys(code, keys, text);
}

/**
 * @brief 编码
 *
 * @param text 64位明文输入
 * @param key 16组48位子密钥
 * @param code 64位密文输出
 */
void DES::EncodeWithKeys(Bit text[64], Bit keys[16][48], Bit code[64]) {
  memcpy(code, text, sizeof(Bit) * 64);
  IPPermutation(code);
  for (int i = 0; i < 16; i++) {
    Iteration(code, code + 32, keys[i]);
  }
  WPermutation(code);
  IPInversePermutation(code);
}

/**
 * @brief 译码
 *
 * @param text 64位密文输出
 * @param key 16组48位子密钥
 * @param code 64位明文输入
 */
void DES::DecodeWithKeys(Bit code[64], Bit keys[16][48], Bit text[64]) {
  memcpy(text, code, sizeof(Bit) * 64);
  IPPermutation(text);
  for (int i = 15; i >= 0; i--) {
    Iteration(text, text + 32, keys[i]);
  }
  WPermutation(text);
  IPInversePermutation(text);
}

/**
 * @brief 读文件，一次读取64位
 *
 * @param in 输入文件路径
 * @param text 读取结果输出位置
 * @param type 执行类型（编码/译码过程）
 * @param end 读取结束标志
 */
void DES::Read64Bit(FILE* in, Bit text[64], TYPE type, bool& end) {
  end = false;

  char ctext[8], c;
  // 读取并转成位数组
  for (int i = 0; i < 8; i++) {
    if (fscanf(in, "%c", &c) <= 0) {
      // 字节填补
      for (int j = i; j < 8; j++) {
        ctext[j] = 8 - i + '0';
        end = true;
      }
      break;
    }
    ctext[i] = c;
  }

  // 判断是否已读完
  if (type == TYPE::DECODER) {
    if (fscanf(in, "%c", &c) <= 0) {
      end = true;
    } else {
      // 文件指针回退
      fseek(in, -1, SEEK_CUR);
    }
  }

  Bytes2Bits(ctext, text);
}

/**
 * @brief 写文件，一次写64位
 *
 * @param out 输出文件路径
 * @param text 需要写的内容
 * @param end 用于判断是否是最后一次要写，译码最后一次要去掉填补内容
 */
void DES::Write64Bit(FILE* out, Bit text[64], bool end) {
  char ctext[8];
  Bits2Bytes(text, ctext);
  // 去除文件末尾填补的字节
  int n = end ? 8 - ctext[7] + '0' : 8;
  for (int i = 0; i < n; i++) {
    fprintf(out, "%c", ctext[i]);
  }
}

// IP 初始置换
void DES::IPPermutation(Bit M[64]) {
  Bit tmp[64];
  for (int i = 0; i < 64; i++) {
    tmp[i] = M[IP_table[i] - 1];
  }
  memcpy(M, tmp, sizeof(Bit) * 64);
}

// 迭代
void DES::Iteration(Bit L[32], Bit R[32], Bit K[48]) {
  Bit feistel[32];
  Feistel(R, K, feistel);
  for (int i = 0; i < 32; i++) {
    Bit tmp = R[i];
    R[i] = L[i] ^ feistel[i];
    L[i] = tmp;
  }
}

// Feistel
void DES::Feistel(Bit R[32], Bit K[48], Bit out[32]) {
  Bit x[48], tmp[32];

  // E 扩展
  for (int i = 0; i < 48; i++) {
    x[i] = R[E_table[i] - 1] ^ K[i];
  }

  // S-Box 置换
  for (int i = 0; i < 8; i++) {
    int st = i * 6;
    int n = (x[st] << 1) + x[st + 5];
    int m = (x[st + 1] << 3) + (x[st + 2] << 2) + (x[st + 3] << 1) + x[st + 4];
    int num = S_box[i][n][m];
    int st2 = i * 4;
    for (int j = 0; j < 4; j++) {
      tmp[st2 + 3 - j] = (num >> j) & 1;
    }
  }

  // P 置换
  for (int i = 0; i < 32; i++) {
    out[i] = tmp[P_table[i] - 1];
  }
}

// W 交换置换
void DES::WPermutation(Bit M[64]) {
  Bit tmp[32];
  memcpy(tmp, M, sizeof(Bit) * 32);
  memcpy(M, M + 32, sizeof(Bit) * 32);
  memcpy(M + 32, tmp, sizeof(Bit) * 32);
}

// IP 逆置换
void DES::IPInversePermutation(Bit M[64]) {
  Bit tmp[64];
  for (int i = 0; i < 64; i++) {
    tmp[i] = M[IPI_table[i] - 1];
  }
  memcpy(M, tmp, sizeof(Bit) * 64);
}

// 56位密钥生成16个48位的子密钥
void DES::GenerateSecretKeys(Bit K56[56], Bit K48[16][48]) {
  Bit L[56], R[56];
  for (int i = 0; i < 28; i++) {
    L[i + 28] = L[i] = K56[PC1_table[i] - 1];
    R[i + 28] = R[i] = K56[PC1_table[i + 28] - 1];
  }

  // 以记录偏移量的方式代替偏移操作
  int st = 0;
  for (int i = 0; i < 16; i++) {
    st += Shift[i];
    for (int j = 0; j < 48; j++) {
      if (PC2_table[j] < 28) {
        K48[i][j] = L[PC2_table[j] + st - 1];
      } else {
        K48[i][j] = R[PC2_table[j] - 29 + st];
      }
    }
  }
}

// 7个字节的字符串密钥生成16个48位的子密钥
void DES::GenerateSecretKeys(const char K[7], Bit K48[16][48]) {
  Bit K56[56];
  for (int i = 0; i < 7; i++) {
    Byte2Bits(K[i], K56 + i * 8);
  }
  GenerateSecretKeys(K56, K48);
}

// 位数组转字节
void DES::Bits2Bytes(const Bit bits[64], char chars[8]) {
  for (int i = 0; i < 8; i++) {
    Bits2Byte(bits + i * 8, chars[i]);
  }
}

// 位数组转字节
void DES::Bits2Byte(const Bit bits[8], char& c) {
  c = '\0';
  for (int i = 0; i < 8; i++) {
    c |= (bits[i] << i);
  }
}

// 字节转位数组
void DES::Bytes2Bits(const char chars[8], Bit bits[64]) {
  for (int i = 0; i < 8; i++) {
    Byte2Bits(chars[i], bits + 8 * i);
  }
}

// 字节转位数组
void DES::Byte2Bits(char c, Bit bits[8]) {
  for (int j = 0; j < 8; j++) {
    bits[j] = (c >> j & 1);
  }
}