
#include "lc_string_util.h"
#include <string.h>
#include <limits.h>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <iostream>

#if (defined __GNUC__ && __GNUC__ >= 4)
#include <uuid/uuid.h> ///¨Cluuid     yum install libuuid-devel
#elif (defined _WIN32 || defined WINCE || defined __CYGWIN__)	 
#include <objbase.h>
#endif

namespace lc_string_util {

  unsigned char ToHex(unsigned char x) {
    return  x > 9 ? x + 55 : x + 48;
  }

  unsigned char FromHex(unsigned char x) {
    unsigned char y = 0;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    //else assert(0);
    return y;
  }

  // in must be at least len bytes
  // len must be 1, 2, or 3
  // buf must be a buffer of at least 4 bytes
  static void _base64_encode(const uint8_t *in, uint32_t len, uint8_t *buf) {
    static const uint8_t *kEncodeTable = (const uint8_t *)
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
      "ghijklmnopqrstuvwxyz0123456789+/";
    buf[0] = kEncodeTable[(in[0] >> 2) & 0x3F];
    if (len == 3) {
      buf[1] = kEncodeTable[((in[0] << 4) + (in[1] >> 4)) & 0x3f];
      buf[2] = kEncodeTable[((in[1] << 2) + (in[2] >> 6)) & 0x3f];
      buf[3] = kEncodeTable[in[2] & 0x3f];
    }
    else if (len == 2) {
      buf[1] = kEncodeTable[((in[0] << 4) + (in[1] >> 4)) & 0x3f];
      buf[2] = kEncodeTable[(in[1] << 2) & 0x3f];
    }
    else { // len == 1
      buf[1] = kEncodeTable[(in[0] << 4) & 0x3f];
    }
  }

  // buf must be a buffer of at least 4 bytes
  // buf will be changed to contain output bytes
  // len is number of bytes to consume from input (must be 2, 3, or 4)
  static void _base64_decode(uint8_t *buf, uint32_t len) {
    static const uint8_t kDecodeTable[256] = {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
      0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
      0x3c, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
      0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
      0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
      0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
      0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
      0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    };
    buf[0] = (kDecodeTable[buf[0]] << 2)
      | (kDecodeTable[buf[1]] >> 4);
    if (len > 2)
    {
      buf[1] = ((kDecodeTable[buf[1]] << 4) & 0xf0)
        | (kDecodeTable[buf[2]] >> 2);
      if (len > 3) buf[2] = ((kDecodeTable[buf[2]] << 6) & 0xc0)
        | (kDecodeTable[buf[3]]);
    }
  }

  static int go_base64_encode(const uint8_t *src, int src_len, char *dst, int dst_len, int wraps) {
    uint8_t buf[512];
    char *mark = dst;
    int cnt = 0;
    if (wraps > (int)sizeof(buf) - 4) {
      wraps = sizeof(buf) - 4; // left 4 bytes
    }
    while (src_len >= 3) {
      _base64_encode(src, 3, &buf[cnt]);
      src += 3;
      src_len -= 3;
      cnt += 4;
      if (cnt >= wraps) {
        if (wraps > 0) buf[cnt++] = '\n';
        if (dst_len < cnt) return -1;
        memcpy(dst, buf, cnt);
        dst += cnt;
        dst_len -= cnt;
        cnt = 0;
      }
    }

    if (src_len > 0) {
      int j = src_len + 1;
      _base64_encode(src, src_len, &buf[cnt]);
      cnt += j;
      while (j++ < 4) buf[cnt++] = '=';
    }
    if (cnt > 0) {
      if (dst_len < cnt) return -1;
      memcpy(dst, buf, cnt);
      dst += cnt;
      dst_len -= cnt;
    }
    if (dst_len > 0) *dst = '\0';
    return (dst - mark);
  }

  static int go_base64_decode(const char *src, int src_len, uint8_t *dst, int dst_len) {
    uint8_t buf[4], *mark = dst;
    int cnt = 0, stop = 0;
    for (; stop == 0 && src_len > 0; src++, --src_len) {
      switch (*src) { // skip all white spaces.
      case '\n': case '\r':
      case '\t': case ' ':
      case '\f': case '\b':
      case 0177: break;
      case '=': { stop = 1; break; }
      default: { buf[cnt++] = *src; }
      }
      if (cnt == 4 || (stop && cnt > 1)) {
        _base64_decode(buf, cnt--);
        if (dst_len < cnt) return -1;
        memcpy(dst, buf, cnt);
        dst += cnt;
        dst_len -= cnt;
        cnt = 0;
      }
    }
    return (dst - mark);
  }

  static inline char* getArray(std::string &str) {
    std::string::iterator it = str.begin();
    return str.empty() ? NULL : &(*it);
  }

  static inline char* getArray(std::vector<char> &str) {
    auto it = str.begin();
    return str.empty() ? NULL : &(*it);
  }

  void SafeBase64ToBase64(std::string& url64) {
    std::replace(url64.begin(), url64.end(), '-', '+');
    std::replace(url64.begin(), url64.end(), '_', '/');
    int len = url64.length();
    int fill_len = (4 - len % 4) % 4;
    for (int i = 0; i < fill_len; ++i) {
      url64 += '=';
    }
  }

  std::string Base64Decode(const char *str, size_t len) {
    size_t siz = (len > 0 ? len : strlen(str));
    size_t dst_len = siz + 1;
    std::string result;
    result.resize(dst_len);

    uint8_t *dst = (uint8_t *)getArray(result);
    int j = go_base64_decode(str, siz, dst, dst_len);
    result.resize(j);
    return result;
  }

  void Base64Decode(const char *str, size_t len, std::vector<char> &result) {
    size_t siz = (len > 0 ? len : strlen(str));
    size_t dst_len = siz + 1;
    result.resize(dst_len);

    uint8_t *dst = (uint8_t *)getArray(result);
    int j = go_base64_decode(str, siz, dst, dst_len);
    result.resize(j);
  }

  void Base64Encode(const void *src, size_t len, int wraps, std::string &result) {
    size_t dst_len = (4 * len + 2) / 3 + 3 + (wraps > 0 ? len / (wraps > 64 ? 64 : wraps) : 0);
    result.resize(dst_len);
    char *dst = getArray(result);
    int j = go_base64_encode((const uint8_t *)src, len, dst, dst_len, wraps);
    result.resize(j);
  }

  std::string Base64Encode(const void *src, size_t len, int wraps) {
    size_t dst_len = (4 * len + 2) / 3 + 3 + (wraps > 0 ? len / (wraps > 64 ? 64 : wraps) : 0);
    std::string result;
    result.resize(dst_len);
    char *dst = getArray(result);
    int j = go_base64_encode((const uint8_t *)src, len, dst, dst_len, wraps);
    result.resize(j);
    return result;
  }

  void BinaryTo16Hex(const std::string& src, std::string& dest) {
    static const char HEX_STR[] = "0123456789abcdef";
    dest.reserve(src.size() * 2 + 1);
    dest.clear();
    for (size_t idx = 0; idx < src.size(); ++idx) {
      dest += HEX_STR[(src[idx] >> 4) & 0xF];
      dest += HEX_STR[src[idx] & 0xF];
    }
  }

  //static std::string hex_digest(const md5_byte_t digest[], size_t size){
  //  static const char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7',  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
  //  std::string result;
  //  result.resize(size + size);
  //  char *str = getArray(result);
  //  for (size_t i = 0, j = 0; i < size; ++i)   {
  //    int x = (int)digest[i];
  //    str[j++] = hex[x >> 4];
  //    str[j++] = hex[x & 15];
  //  }
  //  return result;
  //}

  //std::string GetMd5(const std::string &src, bool hex) {
  //  md5_state_t ms;
  //  md5_byte_t digest[16];
  //  md5_init(&ms);
  //  md5_append(&ms, (const md5_byte_t *)src.c_str(), src.length());
  //  md5_finish(&ms, digest);
  //  if (hex) return hex_digest(digest, sizeof(digest));
  //  return std::string((char *)digest, sizeof(digest));
  //}

  int UrlSafeBase64Encode(const uint8_t *src, int src_len, char *dst) {
    static const char _t64[64] = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'
    };
    int i, loop = src_len / 3;
    char *mark = dst;
    for (i = 0; i < loop; ++i, src += 3)
    {
      *dst++ = _t64[((src[0] >> 2))];
      *dst++ = _t64[((src[0] & 0x03) << 4) | (src[1] >> 4)];
      *dst++ = _t64[((src[1] & 0x0f) << 2) | (src[2] >> 6)];
      *dst++ = _t64[((src[2] & 0x3f))];
    }
    switch (src_len % 3)
    {
    case 2:
      *dst++ = _t64[((src[0] >> 2))];
      *dst++ = _t64[((src[0] & 0x03) << 4) | (src[1] >> 4)];
      *dst++ = _t64[((src[1] & 0x0f) << 2)];
      break;
    case 1:
      *dst++ = _t64[((src[0] >> 2))];
      *dst++ = _t64[((src[0] & 0x03) << 4)];
      break;
    }
    return dst - mark;
  }

  int UrlSafeBase64Decode(const char *src, int src_len, uint8_t *dst) {
    static const char _t256[256] = {
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1,
      52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
      -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
      15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,
      -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
      41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    };
    int i, loop = src_len / 4;
    uint8_t *mark = dst;
    char p0, p1, p2, p3;
    const uint8_t *s = (const uint8_t *)src;
    for (i = 0; i < loop; ++i) {
      if ((p0 = _t256[(int)*s++]) < 0) return -1;
      if ((p1 = _t256[(int)*s++]) < 0) return -1;
      if ((p2 = _t256[(int)*s++]) < 0) return -1;
      if ((p3 = _t256[(int)*s++]) < 0) return -1;
      *dst++ = (p0 << 2) | (p1 >> 4);
      *dst++ = ((p1 << 4) & 0xf0) | (p2 >> 2);
      *dst++ = ((p2 << 6) & 0xc0) | (p3);
    }
    switch (src_len % 4)
    {
    case 0:
      break;
    case 3:
      if ((p0 = _t256[(int)*s++]) < 0) return -1;
      if ((p1 = _t256[(int)*s++]) < 0) return -1;
      if ((p2 = _t256[(int)*s++]) < 0) return -1;
      *dst++ = (p0 << 2) | (p1 >> 4);
      *dst++ = ((p1 << 4) & 0xf0) | (p2 >> 2);
      if ((p2 & 0x3) != 0) return -1;
      break;
    case 2:
      if ((p0 = _t256[(int)*s++]) < 0) return -1;
      if ((p1 = _t256[(int)*s++]) < 0) return -1;
      *dst++ = (p0 << 2) | (p1 >> 4);
      if ((p1 & 0xf) != 0) return -1;
      break;
    default:
      return -2;
    }
    return dst - mark;
  }

  void CreateUuid(std::string &uuid) {
    //  char buffer[36] = { 0 };
    //
    //#if (defined __GNUC__ && __GNUC__ >= 4)
    //  uuid_t request_uuid = { 0 };
    //  uuid_generate(request_uuid);
    //
    //  uuid = hex_digest(request_uuid, sizeof(request_uuid));
    //  /*
    //  uuid_unparse(request_uuid, buffer);
    //  for (int i = 0; i < 36; ++i) {
    //    if (buffer[i] != '-') {
    //      std::string tmp(1, buffer[i]);
    //      uuid.append(tmp);
    //    }
    //  } 
    //  */
    //#elif (defined _WIN32 || defined WINCE || defined __CYGWIN__)	 
    //  GUID guid;
    //  CoCreateGuid(&guid);
    //  _snprintf_s(
    //    buffer,
    //    sizeof(buffer),
    //    "%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x",  //X-upper    x-lower
    //    guid.Data1, guid.Data2, guid.Data3,
    //    guid.Data4[0], guid.Data4[1],
    //    guid.Data4[2], guid.Data4[3],
    //    guid.Data4[4], guid.Data4[5],
    //    guid.Data4[6], guid.Data4[7]);
    //	uuid.assign(buffer);
    //#endif  
  }

  static  std::string header_chars = "string,stringactivationD=Bid^irec$%tionnavgpoolalR(N#N=&exMapToSma-xpoolequebatchno-rmncesHtgh)ucost_layermo-ljp7co@nv2d_fixed_padding+cboG-Vhc_2m95psxd14rqweblma*li4fnvethgudkf12Kc3m5F0{ZT0wL}jAwMQ=4crop5=TE=keN|SORFLO=W=ERvPgsB4Ax][Efhks1==1qzvct6y7u8i9o0p-=as1x3d.4c5f:6v7b8n9m0";
  static  std::string tail_chars = "=Bid^ireSequencesHtgh)uc==onvmxed_padding+cboG-Vhc_2m95oljphog7co@nv2c$%tionnalR(N#hgudkf12Kc3m5F0{ZT0wlma*li4fnvetLO=W=ERvPL}jAwMQ=45=TE=keN|SORFN=&exMapTod_fipsxd14rqs1x3d.4c5f:6v7b8n9mwebgsB4Ax][Efhks1==1qzvct6y7u8i9o0p-=a0";

  void ansiitext2base64(const std::string &src_file, const std::string &dst_file) {
    std::ifstream ifs(src_file.c_str());
    std::ofstream ofs(dst_file.c_str());
    std::string line;
    while (std::getline(ifs, line)) {
      if (line.empty()) {
        ofs << "\n";
        continue;
      }
      std::string base64_str;
      Base64Encode(line.data(), line.size(), 0, base64_str);
      srand(unsigned(time(NULL)));
      std::random_shuffle(header_chars.begin(), header_chars.end());
      std::random_shuffle(tail_chars.begin(), tail_chars.end());
      ofs << header_chars << base64_str << tail_chars << "\n";
    }
    ifs.close();
    ofs.close();
  }

  void base642ansciitext(const std::string &src_file, const std::string &dst_file) {
    std::ifstream ifs(src_file.c_str());
    std::ofstream ofs(dst_file.c_str());
    std::string line;
    const auto ssize = header_chars.size() + tail_chars.size();
    //std::cout << "=====h:" << header_chars.size() << ",t:" << tail_chars << std::endl;
    //int idx = 0;
    while (std::getline(ifs, line)) {
      //idx++;
      if (line.empty()) {
        ofs << "\n";
        continue;
      }

      if (line.size() < ssize) {
        //std::cout <<"lineno:" << idx << ",linesize:" << line.size()  << ",fixsize:"<< ssize << std::endl;
        ofs << "\n";
        continue;
      }

      line = line.substr(header_chars.size(), line.size() - ssize);
      std::string de_str = Base64Decode(line.data(), line.size());
      ofs << de_str << "\n";
      line.clear();
    }
    ifs.close();
    ofs.close();
  }

  void valid(const std::string &modelConfiguration) {
    auto b64 = modelConfiguration + "b64";
    lc_string_util::ansiitext2base64(modelConfiguration, b64);
    auto confpb = modelConfiguration + ".tmp";
    lc_string_util::base642ansciitext(b64, confpb);
  }

}