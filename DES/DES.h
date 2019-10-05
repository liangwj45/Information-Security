#ifndef _DES_H_
#define _DES_H_

#define Bit bool

#include <cstdio>
#include <cstring>

class DES {
 public:
  enum TYPE { ENCODER, DECODER };

 public:
  void Encode(Bit text[64], Bit key[64], Bit code[64]);
  void Decode(Bit code[64], Bit key[64], Bit text[64]);

 public:
  void TestEncode(const char text[8], const char key[8], char code[8]);
  void TestDecode(const char code[8], const char key[8], char text[8]);
  void EncodeWithFile(const char* input, const char* key, const char* output);
  void DecodeWithFile(const char* input, const char* key, const char* output);
  void GenerateSecretKeys(const char K[8], Bit K48[16][48]);

 private:
  void IPPermutation(Bit M[64]);
  void Iteration(Bit L[32], Bit R[32], Bit K[48]);
  void WPermutation(Bit M[64]);
  void IPInversePermutation(Bit M[64]);

 private:
  void GenerateSecretKeys(Bit K64[64], Bit K48[16][48]);
  void Feistel(Bit R[32], Bit K[48], Bit out[32]);

 private:
  void EncodeWithKeys(Bit text[64], Bit keys[16][48], Bit code[64]);
  void DecodeWithKeys(Bit code[64], Bit keys[16][48], Bit text[64]);

 public:
  void Bit2Char(const Bit bits[64], char chars[8]);
  void Bit2Char(const Bit bits[8], char& c);
  void Char2Bit(const char chars[8], Bit bits[64]);
  void Char2Bit(char c, Bit bits[8]);
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