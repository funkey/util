#ifndef UTIL_HDF5_H__
#define UTIL_HDF5_H__

#include <config.h>

#ifdef HAVE_HDF5

#include <vector>

#include <hdf5.h>
#include <H5Cpp.h>
#include "hdf5_traits.h"

namespace hdf5 {

/**
 * Discover all groups in the given H5File or Group and return them in a vector.
 *
 * @param fg An H5File or Group object.
 * @return A vector of Groups in the given object.
 */
std::vector<H5::Group>
groups(H5::CommonFG& fg);

/**
 * Write a vector of data to a given group.
 *
 * @param fg An H5File or Group object.
 * @param name The name of the data set to create.
 * @param data The data to write.
 * @param dims The dimensions of the data (optional, defaults to 1D array).
 * @param datatype An HDF5 DataType object describing the data (optional, for
 *                 compound data types).
 */
template <typename T, typename SizeType>
void
write(
		H5::CommonFG& fg,
		std::string name,
		const std::vector<T>& data,
		const std::vector<SizeType>& dims,
		H5::DataType datatype = typename detail::hdf5_traits<T>::data_type(detail::hdf5_traits<T>::pred_type)) {

	// create an hdf5 dim vector
	std::vector<hsize_t> hdims;
	for (unsigned int i = 0; i < dims.size(); i++)
		hdims.push_back(dims[i]);

	// create the data space
	H5::DataSpace dataspace(dims.size(), &hdims[0]);

	// create a data set from data space and data type
	H5::DataSet dataset = fg.createDataSet(name, datatype, dataspace);

	// write the data into the data set
	dataset.write(&data[0], datatype);
}

/**
 * Write a vector of variable length data to a given group.
 *
 * @param fg An H5File or Group object.
 * @param name The name of the data set to create.
 * @param data The data to write.
 * @param dims The dimensions of the data.
 */
template <typename T>
void
write(H5::CommonFG& fg, std::string name, const std::vector<std::vector<T> >& data, const std::vector<size_t>& dims) {

	// get the HDF5 data and pred type for T
	typedef typename detail::hdf5_traits<T>::data_type hdf5_data_type;
	const H5::PredType hdf5_pred_type = detail::hdf5_traits<T>::pred_type;

	// create the data type
	hdf5_data_type datatype(hdf5_pred_type);

	// create the varlen data type
	H5::VarLenType varlentype(&datatype);

	// create an hdf5 dim vector
	std::vector<hsize_t> hdims;
	for (int i = 0; i < dims.size(); i++)
		hdims.push_back(dims[i]);

	// create the data space
	H5::DataSpace dataspace(dims.size(), &hdims[0]);

	// create a data set from data space and data type
	H5::DataSet dataset = fg.createDataSet(name, varlentype, dataspace);

	// create the varlen data vector
	std::vector<hvl_t> varlendata(data.size());

	// fill it with pointers to the actual data
	for (int i = 0; i < data.size(); i++) {

		varlendata[i].p   = (void*)(&data[i][0]);
		varlendata[i].len = data[i].size();
	}

	// write the varlen data
	dataset.write(&varlendata[0], varlentype);
}

/**
 * Get the dimensions of a dataset.
 *
 * @param fg An H5File or Group object.
 * @param name The name to the data set to read.
 * @return A vector with the size of each dimension.
 */
std::vector<size_t>
dimensions(H5::CommonFG& fg, std::string name);

/**
 * Read a vector of data from a group or file.
 *
 * @param fg An H5File or Group object.
 * @param name The name to the data set to read.
 * @return A vector containing the data.
 */
template <typename T>
std::vector<T>
read(H5::CommonFG& fg, std::string name) {

	// open the dataset
	H5::DataSet dataset = fg.openDataSet(name);

	// get the dimensions
	std::vector<size_t> dims = dimensions(fg, name);

	// compute the size
	unsigned int size = 1;
	for (unsigned int d = 0; d < dims.size(); d++)
		size *= dims[d];

	// create a data buffer
	std::vector<T> data(size);

	// read the data
	dataset.read(&data[0], detail::hdf5_traits<T>::pred_type);

	return data;
}

} // namespace hdf5

#endif // HAVE_HDF5

#endif // UTIL_HDF5_H__

