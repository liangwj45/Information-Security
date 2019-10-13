# DES 算法实现

> 数据科学与计算机学院 2017 级 软件工程 16303050 梁文杰

## 算法原理概述

DES 是基于 56 位密钥的对称算法，它以 64 位为分组长度，64 位一组的明文作为算法的输入，通过一系列复杂的操作，输出同样 64 位长度的密文。解密为加密的逆过程，需要使用同一密钥进行。其中加密、解密过程都涉及到初始置换、16 轮迭代、交换置换、逆置换等相同过程，唯一区别为 16 轮迭代的次序不同。

加密过程：

$$
C = E_k(M) = IP^{-1}\cdot W\cdot T_{16}\cdot T_{15}\cdot \dots \cdot T_1\cdot IP(M)
$$

解密过程：

$$
M = D_k(C) = IP^{-1}\cdot W\cdot T_{1}\cdot T_{2}\cdot \dots \cdot T_{16}\cdot IP(C)
$$

## 总体结构

![1](E:\大三\信息安全技术原理\hw1\DES\img\1.png)

## 模块分解

根据算法具体步骤，程序可分成：

- 输入模块：每次从文件中读取 64 位字符。
  - `void Read64Bit(FILE* in, Bit text[64], TYPE type, bool& end)`
- 输出模块：每次将 64 位字符写进文件。
  - `void Write64Bit(FILE* out, Bit text[64], bool end)`
- 初始置换模块：DES 算法中的初始置换实现。
  - `void IPPermutation(Bit M[64])`
- 迭代模块：DES 算法中的一次迭代实现。
  - `void Iteration(Bit L[32], Bit R[32], Bit K[48])`
- 交换置换模块：DES 算法中的交换置换实现。
  - `void WPermutation(Bit M[64])`
- 逆置换模块：DES 算法中的逆置换实现。
  - `void IPInversePermutation(Bit M[64])`
- 轮函数模块：DES 算法中的轮函数实现。
  - `void Feistel(Bit R[32], Bit K[48], Bit out[32])`
- 密钥生成模块：根据不同数据结构的密钥生成 16 组 48 位子密钥。
  - `void GenerateSecretKeys(const char K[7], Bit K48[16][48])`
  - `void GenerateSecretKeys(Bit K56[56], Bit K48[16][48])`
- 位数组与字节的转换模块：提供转换接口，便于代码复用。
  - `void Bits2Bytes(const Bit bits[64], char chars[8])`
  - `void Bits2Byte(const Bit bits[8], char& c)`
  - `void Bytes2Bits(const char chars[8], Bit bits[64])`
  - `void Byte2Bits(char c, Bit bits[8])`
- 加密、解密模块：提供不同的加密、解密接口，便于代码复用。
  - `void EncodeWithFile(const char* input, const char key[7], const char* output)`
  - `void DecodeWithFile(const char* input, const char key[7], const char* output)`
  - `void Encode(Bit text[64], Bit key[56], Bit code[64])`
  - `void Decode(Bit code[64], Bit key[56], Bit text[64])`
  - `void EncodeWithKeys(Bit text[64], Bit keys[16][48], Bit code[64])`
  - `void DecodeWithKeys(Bit code[64], Bit keys[16][48], Bit text[64])`

## 数据结构

- 使用整型数组 `int []` 存储各类置换表

```c++
static int IP_table[64];
static int E_table[48];
static int P_table[32];
static int IPI_table[64];
static int PC1_table[56];
static int PC2_table[48];
static int Shift[16];
static int S_box[8][4][16];
```

- 使用 `bool` 类型存储 `bit` 的数据

```c++
#define Bit bool
```

- 使用 `bool []` 存储密钥、明文和密文

```c++
Bit keys[16][48];
Bit text[64];
Bit code[64];
```

## 程序源代码

### 输入模块

每次从文件中读取 64 位字符。

```c++

```

### 输出模块

每次将 64 位字符写进文件。

```c++

```

### 初始置换模块

DES 算法中的初始置换实现。

```c++

```

### 迭代模块

DES 算法中的一次迭代实现。

```c++

```

### 交换置换模块

DES 算法中的交换置换实现。

```c++

```

### 逆置换模块

DES 算法中的逆置换实现。

```c++

```

### 轮函数模块

DES 算法中的轮函数实现。

```c++

```

### 密钥生成模块

根据不同数据结构的密钥生成 16 组 48 位子密钥。

```c++

```

### 位数组与字节的转换模块

提供转换接口，便于代码复用。

```c++

```

### 加密、解密模块

提供不同的加密、解密接口，便于代码复用。

```c++
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
```

### 主函数

```c++
int main() {
  DES des;
  des.EncodeWithFile("plaintext.txt", "1234567", "out_encode.bin");
  des.DecodeWithFile("out_encode.bin", "1234567", "out_decode.txt");
}
```

## 编译运行结果

`plaintext.txt` 文件内容：

```
test decode

des testing
```

一个简单的 `makefile` 文件：

```makefile
all:
	g++ main.cc DES.cc -o main
	./main
```

在终端执行 `make` 指令即可编译并执行程序，输出结果为：

`out_encode.bin` 文件为二进制文件。

`out_decode.txt` 文件内容：

```
test decode

des testing
```
