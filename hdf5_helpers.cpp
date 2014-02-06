#include <config.h>

#ifdef HAVE_HDF5

#include "hdf5_helpers.h"

namespace hdf5 {

herr_t
visit_group(hid_t location_id, const char* /*name*/, const H5L_info_t* /*info*/, void* data) {

	std::vector<H5::Group>* groups = (std::vector<H5::Group>*)data;

	groups->push_back(H5::Group(location_id));

	return 0;
}

std::vector<H5::Group>
groups(H5::CommonFG& fg) {

	std::vector<H5::Group> groups;

	H5Literate(fg.getLocId(), H5_INDEX_NAME, H5_ITER_INC, 0, &visit_group, &groups);

	return groups;
}

std::vector<size_t>
dimensions(H5::CommonFG& fg, std::string name) {

	// open the dataset
	H5::DataSet dataset = fg.openDataSet(name);

	// get the dimensions of this dataset
	H5::DataSpace dataspace = dataset.getSpace();

	// get the number of dimensions
	int numDims = dataspace.getSimpleExtentNdims();

	// get the dimensions
	std::vector<hsize_t> hdims(numDims);
	dataspace.getSimpleExtentDims(&hdims[0]);

	// convert to size_t
	std::vector<size_t> dims(numDims);
	for (int d = 0; d < numDims; d++)
		dims[d] = hdims[d];

	return dims;
}

} // namespace hdf5

#endif // HAVE_HDF5
