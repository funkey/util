#include <config.h>

#ifdef HAVE_HDF5

#include "hdf5_traits.h"

namespace hdf5 {

namespace detail {

H5::PredType hdf5_traits<int>::pred_type           = H5::PredType::NATIVE_INT;
H5::PredType hdf5_traits<unsigned int>::pred_type  = H5::PredType::NATIVE_UINT;
H5::PredType hdf5_traits<double>::pred_type        = H5::PredType::NATIVE_DOUBLE;
H5::PredType hdf5_traits<char>::pred_type          = H5::PredType::NATIVE_CHAR;
H5::PredType hdf5_traits<unsigned char>::pred_type = H5::PredType::NATIVE_UCHAR;

} // namespace detail

} // namespace hdf5

#endif // HAVE_HDF5
