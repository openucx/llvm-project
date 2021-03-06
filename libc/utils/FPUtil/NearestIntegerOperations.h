//===-- Nearest integer floating-point operations ---------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_UTILS_FPUTIL_NEAREST_INTEGER_OPERATIONS_H
#define LLVM_LIBC_UTILS_FPUTIL_NEAREST_INTEGER_OPERATIONS_H

#include "FPBits.h"

#include "utils/CPP/TypeTraits.h"

#include <math.h>
#if math_errhandling & MATH_ERREXCEPT
#include "FEnv.h"
#endif
#if math_errhandling & MATH_ERRNO
#include "src/errno/llvmlibc_errno.h"
#include <errno.h>
#endif

namespace __llvm_libc {
namespace fputil {

template <typename T,
          cpp::EnableIfType<cpp::IsFloatingPointType<T>::Value, int> = 0>
static inline T trunc(T x) {
  FPBits<T> bits(x);

  // If x is infinity or NaN, return it.
  // If it is zero also we should return it as is, but the logic
  // later in this function takes care of it. But not doing a zero
  // check, we improve the run time of non-zero values.
  if (bits.isInfOrNaN())
    return x;

  int exponent = bits.getExponent();

  // If the exponent is greater than the most negative mantissa
  // exponent, then x is already an integer.
  if (exponent >= static_cast<int>(MantissaWidth<T>::value))
    return x;

  // If the exponent is such that abs(x) is less than 1, then return 0.
  if (exponent <= -1) {
    if (bits.sign)
      return T(-0.0);
    else
      return T(0.0);
  }

  int trimSize = MantissaWidth<T>::value - exponent;
  bits.mantissa = (bits.mantissa >> trimSize) << trimSize;
  return bits;
}

template <typename T,
          cpp::EnableIfType<cpp::IsFloatingPointType<T>::Value, int> = 0>
static inline T ceil(T x) {
  FPBits<T> bits(x);

  // If x is infinity NaN or zero, return it.
  if (bits.isInfOrNaN() || bits.isZero())
    return x;

  bool isNeg = bits.sign;
  int exponent = bits.getExponent();

  // If the exponent is greater than the most negative mantissa
  // exponent, then x is already an integer.
  if (exponent >= static_cast<int>(MantissaWidth<T>::value))
    return x;

  if (exponent <= -1) {
    if (isNeg)
      return T(-0.0);
    else
      return T(1.0);
  }

  uint32_t trimSize = MantissaWidth<T>::value - exponent;
  bits.mantissa = (bits.mantissa >> trimSize) << trimSize;
  T truncValue = T(bits);

  // If x is already an integer, return it.
  if (truncValue == x)
    return x;

  // If x is negative, the ceil operation is equivalent to the trunc operation.
  if (isNeg)
    return truncValue;

  return truncValue + T(1.0);
}

template <typename T,
          cpp::EnableIfType<cpp::IsFloatingPointType<T>::Value, int> = 0>
static inline T floor(T x) {
  FPBits<T> bits(x);
  if (bits.sign) {
    return -ceil(-x);
  } else {
    return trunc(x);
  }
}

template <typename T,
          cpp::EnableIfType<cpp::IsFloatingPointType<T>::Value, int> = 0>
static inline T round(T x) {
  using UIntType = typename FPBits<T>::UIntType;
  FPBits<T> bits(x);

  // If x is infinity NaN or zero, return it.
  if (bits.isInfOrNaN() || bits.isZero())
    return x;

  bool isNeg = bits.sign;
  int exponent = bits.getExponent();

  // If the exponent is greater than the most negative mantissa
  // exponent, then x is already an integer.
  if (exponent >= static_cast<int>(MantissaWidth<T>::value))
    return x;

  if (exponent == -1) {
    // Absolute value of x is greater than equal to 0.5 but less than 1.
    if (isNeg)
      return T(-1.0);
    else
      return T(1.0);
  }

  if (exponent <= -2) {
    // Absolute value of x is less than 0.5.
    if (isNeg)
      return T(-0.0);
    else
      return T(0.0);
  }

  uint32_t trimSize = MantissaWidth<T>::value - exponent;
  bool halfBitSet = bits.mantissa & (UIntType(1) << (trimSize - 1));
  bits.mantissa = (bits.mantissa >> trimSize) << trimSize;
  T truncValue = T(bits);

  // If x is already an integer, return it.
  if (truncValue == x)
    return x;

  if (!halfBitSet) {
    // Franctional part is less than 0.5 so round value is the
    // same as the trunc value.
    return truncValue;
  } else {
    return isNeg ? truncValue - T(1.0) : truncValue + T(1.0);
  }
}

template <typename F, typename I,
          cpp::EnableIfType<cpp::IsFloatingPointType<F>::Value &&
                                cpp::IsIntegral<I>::Value,
                            int> = 0>
static inline I roundToSignedInteger(F x) {
  constexpr I IntegerMin = (I(1) << (sizeof(I) * 8 - 1));
  constexpr I IntegerMax = -(IntegerMin + 1);

  using FPBits = FPBits<F>;
  F roundedValue = round(x);
  FPBits bits(roundedValue);
  auto setDomainErrorAndRaiseInvalid = []() {
#if math_errhandling & MATH_ERRNO
    llvmlibc_errno = EDOM;
#endif
#if math_errhandling & MATH_ERREXCEPT
    raiseExcept(FE_INVALID);
#endif
  };

  if (bits.isInfOrNaN()) {
    // Result of round is infinity or NaN only if x is infinity
    // or NaN.
    setDomainErrorAndRaiseInvalid();
    return bits.sign ? IntegerMin : IntegerMax;
  }

  int exponent = bits.getExponent();
  constexpr int exponentLimit = sizeof(I) * 8 - 1;
  if (exponent > exponentLimit) {
    setDomainErrorAndRaiseInvalid();
    return bits.sign ? IntegerMin : IntegerMax;
  } else if (exponent == exponentLimit) {
    if (bits.sign == 0 || bits.mantissa != 0) {
      setDomainErrorAndRaiseInvalid();
      return bits.sign ? IntegerMin : IntegerMax;
    }
    // If the control reaches here, then it means that the rounded
    // value is the most negative number for the signed integer type I.
  }

  // For all other cases, if |roundedValue| can fit in the integer type |I|,
  // we just return the roundedValue. Implicit conversion will convert the
  // floating point value to the exact integer value.
  return roundedValue;
}

} // namespace fputil
} // namespace __llvm_libc

#endif // LLVM_LIBC_UTILS_FPUTIL_NEAREST_INTEGER_OPERATIONS_H
