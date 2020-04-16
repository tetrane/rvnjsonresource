#include "reader.h"
#include "common.h"

#include <cassert>
#include <fstream>

namespace reven {
namespace jsonresource {

Reader Reader::open(const char* filename) {
	std::ifstream stream(filename);
	return Reader::open(stream);
}

Reader Reader::open(std::istream& stream) {
	Reader reader(stream);
	reader.md_ = reader.read_metadata();
	return reader;
}

Metadata Reader::read_metadata()
{
	return Metadata::read_metadata(json_);
}

}} // namespace reven::jsonresource
