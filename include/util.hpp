//
// Created by Niemand on 4/11/16.
//

#ifndef FACTORIOVIEWER_UTIL_HPP
#define FACTORIOVIEWER_UTIL_HPP


#include <cstdarg>
#include <cstdio>
#include <string>
#include <sstream>

/**
 * Conversion function which uses a format specifier for the string conversion.
 *
 * @param format        The format specifier like printf
 * @param list          The variable argument list for the format string
 *
 * @return  The output with the formated values
 */
inline std::string vconvertType(const char* format, va_list list) {
  const int bufferSize = 200;
  char buffer[bufferSize];

  // copy the list if we need to iterate through the variables again
  va_list listCpy;
  va_copy(listCpy, list);


  int outSize = vsnprintf(buffer, bufferSize, format, list);

  std::string result;
  if(outSize + 1 > bufferSize) {
    char* newBuffer = new char[outSize + 1];

    outSize = vsnprintf(newBuffer, outSize + 1, format, listCpy);

    result = newBuffer;

    delete [] newBuffer;
  } else {
    result = buffer;
  }

  // cleanup the copied list
  va_end (listCpy);

  return result;
}

/**
 * Conversion function which uses a format specifier for the string conversion.
 *
 * @param format        The format specifier like printf
 * @param ...           The values for the format string
 *
 * @return  The output with the formated values
 */
inline std::string convertType(const char* format, ...) {
  va_list list;
  va_start(list, format);
  std::string result = vconvertType(format, list);
  va_end(list);

  return result;
}


/**
 * Conversion function which uses a format specifier for the string conversion.
 *
 * @param format        The format specifier like printf
 * @param ...           The values for the format string
 *
 * @return  The output with the formated values
 */
inline std::string format(const char* format, ...) {
  va_list list;
  va_start(list, format);
  std::string output = vconvertType(format, list);
  va_end(list);

  return output;
}

/**
 * Conversion function which converts the string into the given type.
 *
 * @param string        The string representation
 *
 * @return  The value from the string representation
 */
template<typename TYPE>
TYPE parseType(const std::string& string) {
    TYPE value;

    std::stringstream ss(string);
    ss >> value;

    return value;
}

/**
 * Conversion function which converts the string into the given type.
 *
 * @param string        The string representation
 *
 * @return  The value from the string representation
 */
template<>
inline std::string parseType<std::string>(const std::string& string) {
    return string;
}


#endif //FACTORIOVIEWER_UTIL_HPP
