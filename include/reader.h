#pragma once

#include <istream>
#include <memory>

#include <boost/property_tree/json_parser.hpp>

#include "metadata.h"

namespace pt = boost::property_tree;

namespace reven {
namespace jsonresource {

///
/// Exception that occurs when there is an error in the reading
///
class ReaderError : public std::runtime_error {
public:
	ReaderError(const char* msg) : std::runtime_error(msg) {}
};

///
/// A Reader of Json resource
///
class Reader {
public:
	///
	/// \brief open Open a resource from the filename passed in parameter
	/// \param filename The filename of the resource to open
	/// \throws ReaderError if an error occurs during the reading of the file
	/// \throws MetadataError if an error occurs during the reading the metadata
	static Reader open(const char* filename);

	///
	/// \brief open Open a resource from a stream passed in parameter
	/// \param stream The stream to read
	/// \throws ReaderError if an error occurs during the reading of the stream
	/// \throws MetadataError if an error occurs during the reading the metadata
	static Reader open(std::istream& stream);

public:
	//! Return the ptree used
	pt::ptree& json() {
		return json_;
	}
	const pt::ptree& json() const {
		return json_;
	}

	//! Returns the metadata read at the opening
	const Metadata& metadata() const { return md_; }

private:
	Reader(std::istream& stream) {
		stream.clear();
		stream.seekg(0, std::ios::beg);
		try {
			pt::read_json(stream, json_);
		} catch (const std::exception& e) {
			throw ReaderError((std::string("Json input malformed: ") + e.what()).c_str());
		}
	}

	Metadata read_metadata();

private:
	pt::ptree json_;

	Metadata md_;
};

}} // namespace reven::jsonresource
