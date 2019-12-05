#include "x509.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
using std::vector;

// 反馈错误信息
#define TYPE(unit, t, msg)                                             \
  do {                                                                 \
    if (unit.type != t) {                                              \
      printf("expect: %x\nreceive: %x\nmsg: %s\n", unit.type, t, msg); \
      exit(-1);                                                        \
    }                                                                  \
  } while (0);

// 读文件并进行初始化
void parser_init(vector<byte_t> &bytes, const char *path) {
  FILE *file = fopen(path, "rb");
  byte_t buf;
  size_t rlen = 0;
  while ((rlen = fread(&buf, sizeof(byte_t), 1, file))) {
    bytes.push_back(buf);
  }
  fclose(file);
}

// 拆分TLV
void parse_units(vector<unit_t> &units, vector<byte_t> &bytes) {
  for (int index = 0; index < bytes.size();) {
    unit_t unit;
    auto &value = unit.value;
    unit.type = bytes[index++];
    byte_t length = bytes[index++];
    int len = length;      // 短格式
    if (length == 0x80) {  // 不定长格式
      byte_t b1, b2;
      while (true) {
        b1 = bytes[index++];
        b2 = bytes[index++];
        if (b1 == 0 && b2 == 0) {  // 判断结束符
          break;
        }
        value.push_back(b1);
        value.push_back(b2);
      }
      unit.length = value.size();
      continue;
    } else if (length > 0x80) {  // 长格式
      len = 0;
      for (int i = 0; i < (length & 0x7f); i++) {
        len = len * 256 + bytes[index++];
      }
    }
    unit.length = len;
    for (int i = 0; i < len; i++) {
      value.push_back(bytes[index++]);
    }
    units.push_back(std::move(unit));
  }
}

// 解析
void parse(unit_t unit) {
  // printf(">>>>>>>>>>>\n%x\n", unit.type);
  switch (unit.type) {
    case 0x1:
      parse_boolean(unit);
      break;
    case 0x2:
      parse_integer(unit);
      break;
    case 0x3:
      parse_bit_string(unit);
      break;
    case 0x4:
      parse_octet_string(unit);
      break;
    case 0x5:
      parse_null(unit);
      break;
    case 0x6:
      parse_object_identifier(unit, "%s\n\t");
      break;
    case 0x13:
    case 0x16:
      parse_printable_string(unit);
      break;
    case 0x17:
    case 0x18:
      parse_time(unit);
      break;
    case 0x30:
      parse_sequence(unit);
      break;
    case 0x31:
      parse_set(unit);
      break;
    default:
      if (unit.type >= 0xa0) {
        vector<unit_t> units;
        parse_units(units, unit.value);
        parse(units[0]);
      } else if (unit.type >= 0x80) {
        for (int i = 0; i < unit.value.size(); i++) {
          printf("%c", unit.value[i]);
        }
        puts("");
      } else {
        printf("type error");
        exit(-1);
      }
  }
}

// 0x01 解析布尔型
void parse_boolean(unit_t unit) {
  TYPE(unit, 0x1, "boolean error");
  puts(unit.value[0] ? "TRUE" : "FALSE");
}

// 0x02 解析整数
void parse_integer(unit_t unit) {
  TYPE(unit, 0x2, "integer error");
  for (int i = 0; i < unit.value.size(); i++) {
    printf("%x", unit.value[i]);
  }
  puts("");
}

// 0x03 解析bit字符串
void parse_bit_string(unit_t unit) {
  byte_t pad = unit.value[0];
  unit.value.erase(unit.value.begin());
  if (unit.type == 0x30) {
    parse_sequence(unit);
  } else {
    for (int i = 0; i < unit.length; i++) {
      int n = unit.value[i];
      string s;
      while (n) {
        s.insert(s.begin(), n % 2 + 48);
        n /= 2;
      }
      printf("%08d", atoi(s.c_str()));
    }
    puts("");
  }
}

// 0x04 解析字节字符串
void parse_octet_string(unit_t unit) {
  TYPE(unit, 0x4, "octet_string error");
  if (unit.value[0] != 0x4 && unit.value[0] != 0x30 && unit.value[0] != 0x31) {
    for (int i = 0; i < unit.value.size(); i++) {
      printf("%x", unit.value[i]);
    }
    puts("");
  } else {
    vector<unit_t> units;
    parse_units(units, unit.value);
    for (int i = 0; i < units.size(); i++) {
      parse(units[i]);
    }
  }
}

// 0x05 解析NULL
void parse_null(unit_t unit) {
  TYPE(unit, 0x5, "octet_string error");
  puts("NULL");
}

// 0x06 解析OID
void parse_object_identifier(unit_t unit, const char *format) {
  TYPE(unit, 0x6, "object_identifier error");
  auto &value = unit.value;
  int v2 = value[0] % 40;
  int v1 = (value[0] - v2) / 40;
  char identifier[30];
  memset(identifier, 0, sizeof(char) * 20);
  sprintf(identifier, "%d.%d", v1, v2);
  for (int i = 1; i < value.size();) {
    int n = 0;
    while (value[i] >> 7) {
      n = n * 128 + (value[i++] & 0x7f);
    }
    n = n * 128 + value[i++];
    char buf[20];
    memset(buf, 0, sizeof(char) * 10);
    sprintf(buf, ".%d", n);
    strcat(identifier, buf);
  }
  printf(format, oid_mapping_table[identifier].c_str());
}

// 0x13 解析字符串
void parse_printable_string(unit_t unit) {
  if (unit.type != 0x13 && unit.type != 0xc && unit.type != 0x16) {
    printf("expect: 0x13||0x0c||0x16\nreceive: %xmsg: printable_string\n",
           unit.type, unit.type);
    exit(-1);
  }
  for (int i = 0; i < unit.length; i++) {
    printf("%c", unit.value[i]);
  }
  puts("");
}

// 0x17 解析日期
void parse_time(unit_t unit) {
  auto &value = unit.value;
  vector<int> nums;
  switch (unit.type) {
    case 0x17:
      for (int i = 0; i < 6; i++) {
        nums.push_back((value[i * 2] - '0') * 10 + value[i * 2 + 1] - '0');
      }
      printf("20%02d-%02d-%02d %02d:%02d:%02d UTC\n", nums[0], nums[1], nums[2],
             nums[3], nums[4], nums[5]);
      break;
    case 0x18:
      // TODO
      break;
    default:
      printf("expect: 0x17||0x18\nreceive: %x\nmsg: time error\n", unit.type);
      exit(-1);
  }
}

// 0x30 解析序列
void parse_sequence(unit_t unit) {
  TYPE(unit, 0x30, "sequence error");
  vector<unit_t> units;
  parse_units(units, unit.value);
  for (int i = 0; i < units.size(); i++) {
    printf("\t");
    parse(units[i]);
  }
  // puts("");
}

// 0x31 解析集合
void parse_set(unit_t unit) {
  TYPE(unit, 0x31, "set error");
  vector<unit_t> units;
  parse_units(units, unit.value);
  for (int i = 0; i < units.size(); i++) {
    parse(units[i]);
  }
  puts("");
}

// 解析程序
void parse(const char *in) {
  vector<byte_t> bytes;
  vector<unit_t> units;
  parser_init(bytes, in);
  parse_units(units, bytes);
  parse_certificate(units[0]);
}

// 解析证书
void parse_certificate(unit_t unit) {
  vector<unit_t> units;
  parse_units(units, unit.value);
  parse_tbs_certificate(units[0]);
  printf("\n%-30s", "Signature:");
  parse_signature(units[1]);
  printf("%-30s", "Value");
  parse_bit_string(units[2]);
}

// 解析证书基本部分
void parse_tbs_certificate(unit_t unit) {
  int i = 0;
  vector<unit_t> units;
  parse_units(units, unit.value);
  printf("%-30s", "Version:");
  if (parse_version(units[i++]) == -1) {  // 使用缺省值
    printf("\n%-30s", "Serial Number:");
    parse_serial_number(units[i]);
  } else {
    printf("\n%-30s", "Serial Number:");
    parse_serial_number(units[i++]);
  }
  parse_signature(units[i++]);
  printf("\n%-30s\n", "Issuer Name:");
  parse_entity(units[i++]);
  printf("\n%-30s\n", "Validity:");
  parse_validity(units[i++]);
  printf("\n%-30s\n", "Subject Name:");
  parse_entity(units[i++]);
  printf("\n%-30s", "Subject Public Key:");
  parse_public_key(units[i++]);
  printf("\n%-30s\n", "Options:");
  while (i < units.size()) {
    parse(units[i++]);
  }
}

// 解析版本号
int parse_version(unit_t unit) {
  if (unit.type != 0xA0) {
    puts("1");
    return -1;  // 使用缺省值
  }
  vector<unit_t> units;
  parse_units(units, unit.value);
  byte_t version = units[0].value[0];
  printf("%d\n", version);
}

// 解析序列数
void parse_serial_number(unit_t unit) {
  if (unit.type != 2) throw 1;
  for (int i = 0; i < unit.value.size(); i++) {
    printf("%02x", unit.value[i]);
  }
  puts("");
}

// 解析签名
void parse_signature(unit_t unit) {
  vector<unit_t> units;
  parse_units(units, unit.value);
  printf("\n%-30s", "Signature Algorithm:");
  parse_signature_algorithm(units[0]);
  printf("%-30s", "Parameters:");
  parse_signature_parameters(units[1]);
}

// 解析签名算法
void parse_signature_algorithm(unit_t unit) {
  parse_object_identifier(unit, "%s\n");
}

// 解析签名算法参数
void parse_signature_parameters(unit_t unit) {
  if (unit.type == 5) {
    puts("NULL");
  } else {
    // TODO
  }
}

// 解析实体
void parse_entity(unit_t unit) {
  vector<unit_t> units;
  parse_units(units, unit.value);
  for (int i = 0; i < units.size(); i++) {
    parse_entity_attribute(units[i]);
  }
}

// 解析实体属性
void parse_entity_attribute(unit_t unit) {
  vector<unit_t> units1, units2;
  parse_units(units1, unit.value);
  parse_units(units2, units1[0].value);
  parse_object_identifier(units2[0], "%-30s");
  parse_printable_string(units2[1]);
}

// 解析有效期
void parse_validity(unit_t unit) {
  vector<unit_t> units;
  parse_units(units, unit.value);
  printf("%-30s", "Not Before");
  parse_time(units[0]);
  printf("%-30s", "Not After");
  parse_time(units[1]);
}

// 解析公钥
void parse_public_key(unit_t unit) {
  vector<unit_t> units;
  parse_units(units, unit.value);
  parse_signature(units[0]);
  printf("%-30s", "Value");
  parse_bit_string(units[1]);
}

int main(int argc, const char **argv) {
  if (argc < 2) {
    printf("usage: %s file\n", argv[0]);
    exit(-1);
  }
  parse(argv[1]);
}