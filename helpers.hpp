#ifndef UTIL_HELPERS_H__
#define UTIL_HELPERS_H__

/*
 *       Filename:  to_string.hpp
 *
 *    Description:  definition of a string convertion
 *
 *        Version:  1.0
 *        Created:  02/14/2008 06:55:12 PM
 *
 *         Author:  Jan Funke, TU Dresden
 */

#include <cassert>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace util {

template<class T>
inline std::string to_string(const T& value) {

  std::ostringstream streamOut;
  streamOut << value;

  return streamOut.str();
}

template<class T>
inline std::string to_string_with_leading_zeros(const T& value, unsigned num_zeros) {

  std::ostringstream streamOut;
  streamOut << std::setw(num_zeros) << std::setfill('0') << value;

  return streamOut.str();
}

template<class T>
inline T from_string(const std::string& string) {

  std::istringstream streamIn(string);
  T value;

  streamIn >> value;

  return value;
}

} // namespace util

template<class T, class U>
std::vector<T> operator+(std::vector<T> lhs, std::vector<U> rhs) {

  assert(lhs.size() == rhs.size());
  std::vector<T> result;
  for (int i = 0; i < lhs.size(); i++)
    result.push_back(lhs[i] + (T)rhs[i]);
  return result;
}
//
//template<class T, class U>
//std::vector<T> operator+(std::vector<T>& lhs, std::vector<U>& rhs) {
//
//  assert(lhs.size() == rhs.size());
//  std::vector<T> result;
//  for (int i = 0; i < lhs.size(); i++)
//    result.push_back(lhs[i] + rhs[i]);
//  return result;
//}

template<class T, class U>
std::vector<T>& operator+=(std::vector<T>& lhs, std::vector<U>& rhs) {

  assert(lhs.size() == rhs.size());
  for (int i = 0; i < lhs.size(); i++)
    lhs[i] += rhs[i];
  return lhs;
}

template<class T, class U>
std::vector<T> operator-(std::vector<T> lhs, std::vector<U> rhs) {

  assert(lhs.size() == rhs.size());
  std::vector<T> result;
  for (int i = 0; i < lhs.size(); i++)
    result.push_back(lhs[i] - rhs[i]);
  return result;
}

template<class T, class U>
std::vector<T>& operator-=(std::vector<T>& lhs, std::vector<U>& rhs) {

  assert(lhs.size() == rhs.size());
  for (int i = 0; i < lhs.size(); i++)
    lhs[i] -= rhs[i];
  return lhs;
}

template<class T>
T operator*(std::vector<T>& lhs, std::vector<T>& rhs) {

  assert(lhs.size() == rhs.size());
  T result = 0.0;
  for (int i = 0; i < lhs.size(); i++)
    result += lhs[i] * rhs[i];
  return result;
}

template<class T, class U>
std::vector<T> operator*(std::vector<T> lhs, U rhs) {

  for (int i = 0; i < lhs.size(); i++)
    lhs[i] *= rhs;
  return lhs;
}

template<class T, class U>
std::vector<T> operator*(std::vector<T>& lhs, U rhs) {

  for (int i = 0; i < lhs.size(); i++)
    lhs[i] *= rhs;
  return lhs;
}

template<class T, class U>
std::vector<T>& operator*=(std::vector<T>& lhs, U rhs) {

  for (int i = 0; i < lhs.size(); i++)
    lhs[i] *= rhs;
  return lhs;
}

template<class T, class U>
std::vector<T> operator/(std::vector<T>& lhs, U& rhs) {

  std::vector<T> result;
  for (int i = 0; i < lhs.size(); i++)
    result.push_back(lhs[i] / rhs);
  return result;
}

template<class T, class U>
std::vector<T> operator/(std::vector<T>& lhs, U rhs) {

  std::vector<T> result;
  for (int i = 0; i < lhs.size(); i++)
    result.push_back(lhs[i] / rhs);
  return result;
}

template<class T, class U>
std::vector<T>& operator/=(std::vector<T>& lhs, U& rhs) {

  for (int i = 0; i < lhs.size(); i++)
    lhs[i] /= rhs;
  return lhs;
}

template<class T, class U>
std::vector<T>& operator/=(std::vector<T>& lhs, U rhs) {

  for (int i = 0; i < lhs.size(); i++)
    lhs[i] /= rhs;
  return lhs;
}

namespace std {

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vector) {

  os << "[";

  for (size_t i = 0; i < vector.size(); i++) {
      os << vector[i];
      if (i != vector.size() - 1)
        os << ", ";
  }

  os << "]";

  return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& set) {

  os << "{";

  typename std::set<T>::const_iterator i;
  for (i = set.begin(); i != set.end(); i++) {
      if (i != set.begin())
        os << ", ";
      os << *i;
  }

  os << "}";

  return os;
}

} // namespace std

#endif // UTIL_HELPERS_H__

