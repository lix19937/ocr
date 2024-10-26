
#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <set>
#include <list>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <unordered_set>

namespace lc_string_util {

void valid(const std::string &modelConfiguration);

void ansiitext2base64(const std::string &src_file, const std::string &dst_file);
void base642ansciitext(const std::string &src_file, const std::string &dst_file);

void SafeBase64ToBase64(std::string& url64);
void Base64Decode(const char *str, size_t len, std::vector<char> &result);
std::string Base64Decode(const char *str, size_t len = 0);
std::string Base64Encode(const void *src, size_t len, int wraps = 0);
void Base64Encode(const void *src, size_t len, int wraps, std::string &result);
void BinaryTo16Hex(const std::string& src, std::string& dest);

void CreateUuid(std::string &uuid);

#define UrlSafe64GetEncodedSize(size) ((4 * (size) + 2) / 3)
int UrlSafeBase64Encode(const uint8_t *src, int src_len, char *dst);
int UrlSafeBase64Decode(const char *src, int src_len, uint8_t *dst);

template<typename T>
T exchange(std::string& src){
  return atoll(src.c_str());
}

template<>
inline double exchange(std::string& src){
  return atof(src.c_str());
}

template<>
inline float exchange(std::string& src){
  return (float)atof(src.c_str());
}

template<>
inline std::string exchange(std::string& src){
  return src;
}

template<typename T>
void Split(std::vector<T>& ret, const std::string &str, char delim, bool ignoreEmpty = true){
  if (str.empty()){
    return;
  }
  ret.clear();
  size_t n = str.size();
  size_t s = 0;

  while (s <= n) {
    size_t i = str.find_first_of(delim, s);
    size_t len = 0;
    //T tmp;

    if (i == std::string::npos){
      len = n - s;
    }
    else{
      len = i - s;
    }

    if (!ignoreEmpty || 0 != len){
      std::string tmp = str.substr(s, len);
      ret.push_back(std::move(exchange<T>(tmp)));
    }

    s += len + 1;
  }
}

template<typename T>
void Split(std::set<T>& ret, const std::string &str, char delim, bool ignoreEmpty = true){
  if (str.empty()){
    return;
  }
  ret.clear();

  size_t n = str.size();
  size_t s = 0;
  while (s <= n) {
    size_t i = str.find_first_of(delim, s);
    size_t len = 0;
    //T tmp;

    if (i == std::string::npos){
      len = n - s;
    }
    else{
      len = i - s;
    }

    if (false == ignoreEmpty || 0 != len){
      std::string tmp = str.substr(s, len);
      ret.insert(std::move(exchange<T>(tmp)));
    }

    s += len + 1;
  }
}

template<typename T>
void Split(std::unordered_set<T>& ret, const std::string &str, char delim, bool ignoreEmpty = true){
  if (str.empty()){
    return;
  }
  ret.clear();

  size_t n = str.size();
  size_t s = 0;

  while (s <= n)
  {
    size_t i = str.find_first_of(delim, s);
    size_t len = 0;
    //T tmp;

    if (i == std::string::npos){
      len = n - s;
    }
    else{
      len = i - s;
    }

    if (false == ignoreEmpty || 0 != len){
      std::string tmp = str.substr(s, len);
      ret.insert(std::move(exchange<T>(tmp)));
    }

    s += len + 1;
  }
}
	
}//namespace string_util

namespace string_convert{

#ifdef WIN32
#	define __strtol		strtol
#	define __strtoul	strtoul
#	define __strtoll   _strtoi64
#	define __strtoull	_strtoui64
#	define __strtof		strtod
#	define __strtod		strtod
#	define __strtold	strtod
#else
#	define __strtol		strtol
#	define __strtoul	strtoul
#	define __strtoll	strtoll
#	define __strtoull	strtoull
#	define __strtof		strtof
#	define __strtod		strtod
#	define __strtold	strtold
#endif

//////////////////////////////////////////////////////////////////////
// _StringToValueImpl

template <typename T>
struct _StringToValueImpl{};

#define _BUILD_STRING_TO_VALUE_IMPL_(type)          \
  template<>                        \
  struct _StringToValueImpl<type>             \
  {                             \
    static type toValue(\
    const char * str, \
    type default_value)               \
  {                           \
    return (type)(_StringToValueImpl<long>      \
    ::toValue(str, default_value));   \
  }                           \
  };                            \
    \
    template<>                        \
  struct _StringToValueImpl<unsigned type>        \
  {                             \
    static unsigned type toValue(\
    const char * str, \
    unsigned type default_value)          \
  {                           \
    return (unsigned type)(\
    _StringToValueImpl<unsigned long>       \
    ::toValue(str, default_value));       \
  }                           \
  };                            \

template<>
struct _StringToValueImpl<long> {
  static long toValue(const char * str, long default_value)  {
    char * end = NULL;
    long n = __strtol(str, &end, 0);
    return end == NULL ? default_value : n;
  }
};

template<>
struct _StringToValueImpl<unsigned long> {
  static unsigned long toValue(const char * str, unsigned long default_value) {
    char * end = NULL;
    long n = ::__strtoul(str, &end, 0);
    return end == NULL ? default_value : n;
  }
};

template<>
struct _StringToValueImpl<long long> {
  static long long toValue(const char * str, long long default_value) {
    char * end = NULL;
    long long n = __strtoll(str, &end, 0);
    return end == NULL ? default_value : n;
  }
};

template<>
struct _StringToValueImpl<unsigned long long> {
  static unsigned long long toValue(const char * str, unsigned long long default_value) {
    char * end = NULL;
    long long n = __strtoull(str, &end, 0);
    return end == NULL ? default_value : n;
  }
};

template<>
struct _StringToValueImpl<float> {
  static float toValue(const char * str, float default_value) {
    char * end = NULL;
    float n = __strtof(str, &end);
    return end == NULL ? default_value : n;
  }
};

template<>
struct _StringToValueImpl<double> {
  static double toValue(const char * str, double default_value) {
    char * end = NULL;
    double n = __strtod(str, &end);
    return end == NULL ? default_value : n;
  }
};

template<>
struct _StringToValueImpl<long double> {
  static long double toValue(const char * str, long double default_value)  {
    char * end = NULL;
    long double n = __strtold(str, &end);
    return end == NULL ? default_value : n;
  }
};

_BUILD_STRING_TO_VALUE_IMPL_(char);
_BUILD_STRING_TO_VALUE_IMPL_(short);
_BUILD_STRING_TO_VALUE_IMPL_(int);

#undef __strtol
#undef __strtoul
#undef __strtoll
#undef __strtoull
#undef __strtof
#undef __strtod
#undef __strtodl

template<typename T>
static T ToValue(const char * str, T default_value = 0){
  return _StringToValueImpl<T>::toValue(str, default_value);
}

template<typename T>
static T ToValue(const std::string& str, T default_value = 0){
  return _StringToValueImpl<T>::toValue(str.c_str(), default_value);
}

template<typename T>
static std::string ToString(T value){
  return std::to_string(value);
}

}//namespace string_convert

#endif
