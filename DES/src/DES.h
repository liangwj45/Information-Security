#ifndef _DES_H_
#define _DES_H_

#include <cstdio>
#include <cstring>
#define Bit bool

class DES {
 public:
  enum TYPE { ENCODER, DECODER };

 public:
  void EncodeWithFile(const char* input, const char key[7], const char* output);
  void DecodeWithFile(const char* input, const char key[7], const char* output);
  void GenerateSecretKeys(const char K[7], Bit K48[16][48]);

  void Encode(Bit text[64], Bit key[56], Bit code[64]);
  void Decode(Bit code[64], Bit key[56], Bit text[64]);
  void GenerateSecretKeys(Bit K56[56], Bit K48[16][48]);

  void EncodeWithKeys(Bit text[64], Bit keys[16][48], Bit code[64]);
  void DecodeWithKeys(Bit code[64], Bit keys[16][48], Bit text[64]);

  // void TestEncode(const char text[8], const char key[8], char code[8]);
  // void TestDecode(const char code[8], const char key[8], char text[8]);

 private:
  void IPPermutation(Bit M[64]);                    // 初始置换
  void Iteration(Bit L[32], Bit R[32], Bit K[48]);  // 16轮迭代
  void WPermutation(Bit M[64]);                     // 交换置换
  void IPInversePermutation(Bit M[64]);             // 逆置换
  void Feistel(Bit R[32], Bit K[48], Bit out[32]);  // 轮函数

 private:
  void Bits2Bytes(const Bit bits[64], char chars[8]);
  void Bits2Byte(const Bit bits[8], char& c);
  void Bytes2Bits(const char chars[8], Bit bits[64]);
  void Byte2Bits(char c, Bit bits[8]);
  void Read64Bit(FILE* in, Bit text[64], TYPE type, bool& end);
  void Write64Bit(FILE* out, Bit text[64], bool end);

 private:
  static int IP_table[64];
  static int E_table[48];
  static int P_table[32];
  static int IPI_table[64];
  static int PC1_table[56];
  static int PC2_table[48];
  static int Shift[16];
  static int S_box[8][4][16];
};

#endif