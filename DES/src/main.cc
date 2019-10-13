#include "DES.h"

int main() {
  DES des;
  des.EncodeWithFile("plaintext.txt", "1234567", "out_encode.bin");
  des.DecodeWithFile("out_encode.bin", "1234567", "out_decode.txt");
}