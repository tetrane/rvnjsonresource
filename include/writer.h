#pragma once

#include <ostream>
#include <memory>

#include <boost/property_tree/json_parser.hpp>

#include "metadata.h"

namespace pt = boost::property_tree;

namespace reven {
namespace jsonresource {

///
/// Exception that occurs when there is an error in the writing
///
class WriterError : public std::runtime_error {
public:
	WriterError(const char* msg) : std::runtime_error(msg) {}
};

///
/// A Writer of Json resource
///
class Writer {
public:
	///
	/// \brief create Create a resource with the metadata and filename passed in parameter
	/// \param filename The filename of the resource to open
	/// \param md The metadata to write in the file
	/// \throws WriterError if an error occurs during the writing of the file
	static Writer create(const char* filename, const Metadata& md);

	///
	/// \brief create Create a resource with the metadata and stream passed in parameter
	/// \param json The ptree containing the JSON objects to write
	/// \param stream The output stream to write
	/// \param md The metadata to write in the file
	/// \throws WriterError if an error occurs during the writing of the stream
	static Writer create(pt::ptree& json, std::unique_ptr<std::ostream>&& stream, const Metadata& md);

	///
	/// \brief open Open an already versioned resource with the filename passed in parameter
	/// \param filename The filename of the resource to open and write
	/// \throws WriterError if an error occurs during the reading of the file
	/// \throws MetadataError if an error occurs during the reading the existing metadata
	static Writer open(const char* filename);

	///
	/// \brief open Open an already versioned resource with the stream passed in parameter
	/// \param stream The output stream to write
	/// \throws WriterError if an error occurs during the reading of the stream
	/// \throws MetadataError if an error occurs during the reading the existing metadata
	static Writer open(pt::ptree& json, std::unique_ptr<std::ostream>&& stream);

public:
	//! Return the ptree used
	pt::ptree& json() {
		return json_;
	}

	//! Return the stream used
	std::ostream& stream() {
		return *stream_;
	}

	//! Retrieve the stream in case someone want to access it after the end of the writing
	std::unique_ptr<std::ostream>&& finalize() && {
		return std::move(stream_);
	}

	///
	/// \brief set_metadata Update the metadata of an already existing resource
	/// \param md The metadata to write in the resource
	/// \throws WriterError if an error occurs during the writing of the resource
	void set_metadata(const Metadata& md);

private:
	Writer(pt::ptree& json, std::unique_ptr<std::ostream>&& stream)
		: stream_{std::move(stream)}, json_(json) {}

private:
	//! Stored in a pointer because ostream itself is not movable
	std::unique_ptr<std::ostream> stream_;

	pt::ptree json_;
};

}} // namespace reven::binresource
