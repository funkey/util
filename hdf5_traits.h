#ifndef UTIL_HDF5_TRAITS_H__
#define UTIL_HDF5_TRAITS_H__

#include <config.h>

#ifdef HAVE_HDF5

#include <H5Cpp.h>

namespace hdf5 {

namespace detail {

template <typename T>
struct hdf5_traits {

	// no default
};

template <>
struct hdf5_traits<int> {

	typedef H5::IntType data_type;

	static H5::PredType pred_type;
};

template <>
struct hdf5_traits<unsigned int> {

	typedef H5::IntType data_type;

	static H5::PredType pred_type;
};

template <>
struct hdf5_traits<double> {

	typedef H5::FloatType data_type;

	static H5::PredType pred_type;
};

template <>
struct hdf5_traits<char> {

	typedef H5::IntType data_type;

	static H5::PredType pred_type;
};

template <>
struct hdf5_traits<unsigned char> {

	typedef H5::IntType data_type;

	static H5::PredType pred_type;
};

} // namespace detail

} // namespace hdf5

#endif // HAVE_HDF5

#endif // UTIL_HDF5_TRAITS_H__

