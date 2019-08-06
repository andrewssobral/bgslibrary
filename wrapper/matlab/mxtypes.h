/** MxTypes and other type traits for template.
 *
 * Copyright 2014 Kota Yamaguchi.
 */

#ifndef INCLUDE_MEXPLUS_MXTYPES_H_
#define INCLUDE_MEXPLUS_MXTYPES_H_

#include <mex.h>
#include <complex>
#include <type_traits>

namespace mexplus {

/************************************************************/
/* Traits for fundamental datatypes.
   Don't use with function templates due to type promotion!
   (Ignore it and get PITA!)                                */
/************************************************************/

/** Traits for mxLogical-convertibles.
 */
template <typename T, typename U = T>
struct MxLogicalTy : std::false_type {};

template <typename T>
struct MxLogicalTy<T, typename std::enable_if<
    std::is_same<typename std::remove_cv<T>::type, bool>::value ||
    std::is_same<typename std::remove_cv<T>::type, mxLogical>::value,
    T>::type> : std::true_type {};

/** Traits for mxChar-convertibles.
 *
 * Treat them as an integer types when they are specified signed or unsigned,
 * because uint8_t is exactly unsigned char and there is no way to tell them
 * apart.
 */
template <typename T, typename U = T>
struct MxCharTy : std::false_type {};

template <typename T>
struct MxCharTy<T, typename std::enable_if<
    std::is_same<typename std::remove_cv<T>::type, char>::value ||
    // Visual Studio cannot distinguish these from uint.
    // std::is_same<typename std::remove_cv<T>::type, char16_t>::value ||
    // std::is_same<typename std::remove_cv<T>::type, char32_t>::value ||
    std::is_same<typename std::remove_cv<T>::type, mxChar>::value ||
    std::is_same<typename std::remove_cv<T>::type, wchar_t>::value,
    T>::type> : std::true_type {};

/** Traits for integer numerics.
 */
template <typename T, typename U = T>
struct MxIntTy : std::false_type {};
template <typename T>
struct MxIntTy<T, typename std::enable_if<
    std::is_integral<T>::value &&
    !MxLogicalTy<T>::value &&
    !MxCharTy<T>::value,
    T>::type> : std::true_type {};
/** Traits for arithmetic types.
 */
template <typename T, typename U = T>
struct MxArithmeticTy : std::false_type {};
template <typename T>
struct MxArithmeticTy<T, typename std::enable_if<
    (std::is_floating_point<T>::value) || (MxIntTy<T>::value),
    T>::type> : std::true_type {};

/**********************************************/
/* Introducing traits for MATLAB array types. */
/**********************************************/

typedef struct mxNumeric_tag {} mxNumeric;
typedef struct mxCell_tag    {} mxCell;
typedef struct mxComplex_tag {} mxComplex;
// mxLogical already defined in MATLAB (matrix.h).

/** Traits for mxArray.
 */
template <typename T, typename U = T>
struct MxTypes {
  typedef T type;
  typedef mxCell array_type;
  static const mxClassID class_id = mxUNKNOWN_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<MxCharTy<T>::value, T>::type> {
  typedef T type;
  typedef mxChar array_type;
  static const mxClassID class_id = mxCHAR_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<MxLogicalTy<T>::value, T>::type> {
  typedef T type;
  typedef mxLogical array_type;
  static const mxClassID class_id = mxLOGICAL_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_signed<T>::value &&
                                          MxIntTy<T>::value &&
                                          sizeof(T) == 1, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxINT8_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_unsigned<T>::value &&
                                          MxIntTy<T>::value &&
                                          sizeof(T) == 1, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxUINT8_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_signed<T>::value &&
                                          MxIntTy<T>::value &&
                                          sizeof(T) == 2, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxINT16_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_unsigned<T>::value &&
                                          MxIntTy<T>::value &&
                                          sizeof(T) == 2, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxUINT16_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_signed<T>::value &&
                                          MxIntTy<T>::value &&
                                          sizeof(T) == 4, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxINT32_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_unsigned<T>::value &&
                                          MxIntTy<T>::value &&
                                          sizeof(T) == 4, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxUINT32_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_signed<T>::value &&
                                          MxIntTy<T>::value &&
                                          sizeof(T) == 8, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxINT64_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_unsigned<T>::value &&
                                          MxIntTy<T>::value &&
                                          sizeof(T) == 8, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxUINT64_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_floating_point<T>::value &&
                                          sizeof(T) == 4, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxSINGLE_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<std::is_floating_point<T>::value &&
                                          sizeof(T) == 8, T>::type> {
  typedef T type;
  typedef mxNumeric array_type;
  static const mxClassID class_id = mxDOUBLE_CLASS;
  static const mxComplexity complexity = mxREAL;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<
    std::is_same<typename std::remove_cv<T>::type,
                 std::complex<float>>::value,
                 T>::type> {
  typedef T type;
  typedef mxComplex array_type;
  static const mxClassID class_id = mxSINGLE_CLASS;
  static const mxComplexity complexity = mxCOMPLEX;
};

template <typename T>
struct MxTypes<T, typename std::enable_if<
    std::is_same<typename std::remove_cv<T>::type,
                 std::complex<double>>::value,
                 T>::type> {
  typedef T type;
  typedef mxComplex array_type;
  static const mxClassID class_id = mxDOUBLE_CLASS;
  static const mxComplexity complexity = mxCOMPLEX;
};

/********************************************/
/* Type traits for function template usage. */
/********************************************/

/* Traits for logical types.
 */
template <typename T, typename U = T>
struct MxLogicalType : std::false_type {};
template<typename T>
struct MxLogicalType<T, typename std::enable_if<
    std::is_same<typename MxTypes<T>::array_type, mxLogical>::value,
    T>::type> : std::true_type {};

/* Traits for char types.
 */
template <typename T, typename U = T>
struct MxCharType : std::false_type {};
template<typename T>
struct MxCharType<T, typename std::enable_if<
    std::is_same<typename MxTypes<T>::array_type, mxChar>::value,
    T>::type> : std::true_type {};

/* Traits for arithmetic types.
 */
template <typename T, typename U = T>
struct MxArithmeticType : std::false_type {};
template<typename T>
struct MxArithmeticType<T, typename std::enable_if<
    std::is_same<typename MxTypes<T>::array_type, mxNumeric>::value,
    T>::type> : std::true_type {};

/* Traits for complex types.
 */
template <typename T, typename U = T>
struct MxComplexType : std::false_type {};
template<typename T>
struct MxComplexType<T, typename std::enable_if<
    std::is_same<typename MxTypes<T>::array_type, mxComplex>::value,
    T>::type> : std::true_type {};

/* Traits for complex or arithmetic types.
 */
template <typename T, typename U = T>
struct MxComplexOrArithmeticType : std::false_type {};
template <typename T>
struct MxComplexOrArithmeticType<T, typename std::enable_if<
    MxComplexType<T>::value,
    T>::type> : std::true_type {};
template <typename T>
struct MxComplexOrArithmeticType<T, typename std::enable_if<
    MxArithmeticTy<T>::value,
    T>::type> : std::true_type {};

/* Traits for cell types.
 */
template <typename T, typename U = T>
struct MxCellType : std::false_type {};
template <typename T>
struct MxCellType<T, typename std::enable_if<
    std::is_same<typename MxTypes<T>::array_type, mxCell>::value,
    T>::type> : std::true_type {};

/* Traits for logical type compounds.
 */
template <typename T, typename U = T>
struct MxLogicalCompound : std::false_type {};
template <typename T>
struct MxLogicalCompound<T, typename std::enable_if<
    MxLogicalType<typename T::value_type>::value,
    T>::type> : std::true_type {};

/* Traits for char type compounds.
 */
template <typename T, typename U = T>
struct MxCharCompound : std::false_type {};
template <typename T>
struct MxCharCompound<T, typename std::enable_if<
    MxCharType<typename T::value_type>::value,
    T>::type> : std::true_type {};

/* Traits for arithmetic type compounds.
 */
template <typename T, typename U = T>
struct MxArithmeticCompound : std::false_type {};
template <typename T>
struct MxArithmeticCompound<T, typename std::enable_if<
    (MxArithmeticType<typename T::value_type>::value) &&
    !(MxComplexType<T>::value),
    T>::type> : std::true_type {};

/* Traits for complex type compounds.
 */
template <typename T, typename U = T>
struct MxComplexCompound : std::false_type {};
template <typename T>
struct MxComplexCompound<T, typename std::enable_if<
    MxComplexType<typename T::value_type>::value,
    T>::type> : std::true_type {};

/* Traits for complex or arithmetic type compounds.
 */
template <typename T, typename U = T>
struct MxComplexOrArithmeticCompound : std::false_type {};
template <typename T>
struct MxComplexOrArithmeticCompound<T, typename std::enable_if<
    MxComplexCompound<T>::value ||
    MxArithmeticCompound<T>::value,
    T>::type> : std::true_type {};

/* Traits for cell compounds.
 */
template <typename T, typename U = T>
struct MxCellCompound : std::false_type {};
template <typename T>
struct MxCellCompound<T, typename std::enable_if<
    MxCellType<typename T::value_type>::value,
    T>::type> : std::true_type {};

}  // namespace mexplus

#endif  // INCLUDE_MEXPLUS_MXTYPES_H_
