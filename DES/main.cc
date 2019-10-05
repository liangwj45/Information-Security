#include "DES.h"

int main() {
  DES des;
  des.EncodeWithFile("plaintext.txt", "16303050", "out_encode.txt");
  des.DecodeWithFile("out_encode.txt", "16303050", "out_decode.txt");
}