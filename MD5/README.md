# MD5 算法

## 算法原理概述

 MD5 使用 little-endian(小端模式)，输入任意不定长度信息，以 512-bit 进行分组，生成四个32-bit 数据，最后联合输出固定 128-bit 的信息摘要。

MD5 算法的基本过程为：填充、分块、缓冲区初始化、循环压缩、得出结果。 

填充：输入长度为 K bits 的原始消息数据，填充 P bits 的标识和 64 bits 的消息长度消息。

分块：将填充后的消息分割成 L 个 512 bits 的分组。

缓冲区初始化：初始化一个 128 bits 的 MD 缓冲区，表示成 4 个 32 bits 的寄存器（A，B，C，D）。

循环压缩：将循环压缩函数依次作用于缓冲区数据和消息的每一个分组，最后得出结果。

## 总体结构

以 512 bits 消息分组为单位，每一分组经过 4 个循环的压缩算法，表示为：
$$
\begin{align}
CV_0 &= IV\\
CV_i &= H_{MD5}(CV_{i-1}, Y_i)
\end{align}
$$
输出结果：
$$
MD = CV_L
$$


![](./img/1.png)

## 模块分解

### 数据填充

最后作用于压缩函数的消息是以 512 bit 为分组的，原始消息 (K bits) 加上填充字段 (P bits) 和尾部填充的数据长度消息 (64 bits) 的总长度最终是 512 的倍数，即：
$$
K+P+64 \equiv 0 \ (mod\ \  512) \\
K+P \equiv 448 \ (mod\ \  512)
$$
填充的字段是 1000, 0000, ...，二进制首位为 1，其余为 0。最后 64 位加上原始消息的长度信息。

```c
void MD5(const uint8_t* src, size_t len, uint8_t* out) {
  // 复制和填充
  int i, plen = (512 + 448 - len * 8) % 512 / 8;      // 填充的字节数
  uint8_t* msg = (uint8_t*)malloc(len + plen + 8);    // 填充后的消息
  memcpy(msg, src, len);                              // 拷贝消息
  msg[len] = 0x80;                                    // 填充 1000,0000
  for (i = len + 1; i < len + plen; i++) msg[i] = 0;  // 填充 00...00

  // 添加消息长度
  to_bytes(len * 8, msg + len + plen);        // 低32位
  to_bytes(len >> 29, msg + len + plen + 4);  // 高32位
  len = len + plen + 8;                       // 更新长度
    
  // ...
}

void to_bytes(uint32_t val, uint8_t* bytes) {
  bytes[0] = (uint8_t)val;
  bytes[1] = (uint8_t)(val >> 8);
  bytes[2] = (uint8_t)(val >> 16);
  bytes[3] = (uint8_t)(val >> 24);
}
```

### 分块

把填充后的消息结果分割为 L 个 512 bits 的分组，也可表示成 N 个 32 bits 字。

```c
void HMD5(uint32_t CV[4], uint8_t Y[64]) {
  uint32_t a, b, c, d, g, x, i, tmp, y[16];

  // 分组
  for (i = 0; i < 16; i++) {
    y[i] = to_int32(Y + i * 4);
  }
    
  // ...
}

uint32_t to_int32(const uint8_t* bytes) {
  return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
         ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}
```

### 初始化

初始化一个用于迭代操作，长度为 128 bits 的缓存区（IV）。

```c
uint32_t CV[] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
```

### 压缩函数

从 CV 输入128 位，从消息分组输入 512 位，完成 4 轮循环后，输出 128 位，用于下 一轮输入的 CV 值。 

每轮循环分别固定不同的生成函数 F, G, H, I，结合指定的 T 表元素 T[] 和消息分组的不同部分 X[] 做 16 次迭代运算，生成下一轮循环的输入。4 轮循环共有 64 次迭代运算。

每轮循环中的一次迭代运算逻辑：

- 对 A 迭代：a ← b + ((a + g(b, c, d) + X[k] + T[i]) <<< s)
- 缓冲区 (A, B, C, D) 作循环轮换：(B, C, D, A) ← (A, B, C, D)

|   4 轮循环逻辑   |    轮函数逻辑    | 一次迭代运算逻辑 |
| :--------------: | :--------------: | :--------------: |
| ![](./img/2.png) | ![](./img/3.png) | ![](./img/4.png) |

```c
typedef uint32_t (*g_t)(uint32_t b, uint32_t c, uint32_t d);
g_t gs[] = {F, G, H, I};  // 轮函数

void MD5(const uint8_t* src, size_t len, uint8_t* out) {
  // ...

  // MD5压缩函数
  uint32_t CV[] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
  for (i = 0; i < len; i += 64) {
    HMD5(CV, msg + i);
  }
    
  // ...
}

void HMD5(uint32_t CV[4], uint8_t Y[64]) {
  uint32_t a, b, c, d, g, x, i, tmp, y[16];

  // 分组
  for (i = 0; i < 16; i++) {
    y[i] = to_int32(Y + i * 4);
  }

  // 寄存器
  a = CV[0];
  b = CV[1];
  c = CV[2];
  d = CV[3];

  // 迭代
  for (i = 0; i < 64; i++) {
    g = gs[i / 16](b, c, d);  // 轮函数
    x = y[xk[i]];             // 消息分组第k个32位字
    tmp = b + left_rotate(a + g + x + T[i], s[i]);  // 对A迭代
    a = d, d = c, c = b, b = tmp;                   // 循环轮换
  }

  // 累加
  CV[0] += a;
  CV[1] += b;
  CV[2] += c;
  CV[3] += d;
}

uint32_t F(uint32_t b, uint32_t c, uint32_t d) { return (b & c) | ((~b) & d); }
uint32_t G(uint32_t b, uint32_t c, uint32_t d) { return (d & b) | ((~d) & c); }
uint32_t H(uint32_t b, uint32_t c, uint32_t d) { return b ^ c ^ d; }
uint32_t I(uint32_t b, uint32_t c, uint32_t d) { return c ^ (b | (~d)); }
uint32_t left_rotate(uint32_t a, uint8_t s) { return a << s | a >> (32 - s); }
```

## 数据结构

主要使用 uint8_t 和 uint32_t 两种类型的变量，用于存储消息和缓冲区中的数据。

- 原始消息以及填充后的消息

```c
uint8_t *msg;
```

- 缓冲区以及四个寄存器

```c
uint32_t CV[4], a, b, c, d;
```

- 各次迭代运算 (1..64) 采用的 T 值

```c
const uint32_t T[64];
```

- 各次迭代运算 (1..64) 采用的左循环移位的 s 值

```c
const uint32_t s[64];
```

- 各次迭代运算 (1..64) 采用的 X[k] 的值

```c
const uint8_t xk[64];
```





