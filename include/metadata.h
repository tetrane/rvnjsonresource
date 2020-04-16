#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <ostream>

#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

namespace reven {
namespace jsonresource {

///
/// Root metadata exception. Catch this exception to catch all exceptions related to metadata.
///
class MetadataError : public std::runtime_error {
public:
	MetadataError(const char* msg) : std::runtime_error(msg) {}
};

///
/// Exception that occurs when it is not possible to serialize the metadata.
///
class WriteMetadataError : public MetadataError {
public:
	WriteMetadataError(const char* msg) : MetadataError(msg) {}
};

///
/// Exception that occurs when it is not possible to deserialize the metadata.
///
class ReadMetadataError : public MetadataError {
public:
	ReadMetadataError(const char* msg) : MetadataError(msg) {}
};

///
/// Exception that occurs when metadata is missing
///
class MissingMetadata : public ReadMetadataError {
public:
	MissingMetadata(const char* msg) : ReadMetadataError(msg) {}
};

///
/// Exception that occurs when metadata version is missing
///
class MissingMetadataVersion : public ReadMetadataError {
public:
	MissingMetadataVersion(const char* msg) : ReadMetadataError(msg) {}
};

///
/// Exception that occurs when a metadata field is missing
///
class MissingMetadataField : public ReadMetadataError {
public:
	MissingMetadataField(const char* msg) : ReadMetadataError(msg) {}
};

///
/// Exception that occurs when a metadata field does not contains a good value
///
class BadMetadataField : public ReadMetadataError {
public:
	BadMetadataField(const char* msg) : ReadMetadataError(msg) {}
};

///
/// Exception that occurs when metadata version is incompatible
///
class IncompatibleMetadataVersion : public ReadMetadataError {
public:
	IncompatibleMetadataVersion(const char* msg) : ReadMetadataError(msg) {}
};

///
/// Raw Metadata class that contains the metadata in the format stored and retrieved by the reader.
/// It is not meant to be used directly by clients, instead use converters that give a semantics to the metadata
///
using CustomMetadata = std::unordered_map<std::string /* key */, std::string /* value */>;

class Metadata {
public:
	static Metadata deserialize(std::istream& input);
	static Metadata read_metadata(pt::ptree& json);

public:
	/// Magic representing the resource type
	std::uint32_t type() const { return type_; }
	/// A version for the resource file format (should be of format "x.y.z-suffix" with an optional suffix)
	const std::string& format_version() const { return format_version_; }
	/// Name of the tool that generated the resource
	const std::string& tool_name() const { return tool_name_; }
	/// A version for the tool that generated the resource (should be of format "x.y.z-suffix" with an optional suffix)
	const std::string& tool_version() const { return tool_version_; }
	/// The version of the tool and possibly the version of the writer library used
	const std::string& tool_info() const { return tool_info_; }
	/// The date of the generation
	std::uint64_t generation_date() const { return generation_date_; }

	const CustomMetadata& custom_metadata() const { return custom_metadata_; }

	void write_metadata(pt::ptree& json) const;
	void serialize(pt::ptree& json, std::ostream& out) const;

	bool operator==(const Metadata& md) const
	{
		return type_ == md.type_ and format_version_ == md.format_version_ and tool_name_ == md.tool_name_
			and tool_version_ == md.tool_version_ and tool_info_ == md.tool_info_ and
			generation_date_ == md.generation_date_ and custom_metadata_ == md.custom_metadata_;
	}

private:
	// General clients are not expected to be able to build Metadata
	Metadata() = default;

	std::uint32_t type_;
	std::string format_version_;
	std::string tool_name_;
	std::string tool_version_;
	std::string tool_info_;
	std::uint64_t generation_date_;
	CustomMetadata custom_metadata_;

	// Special class that is allowed to build Metadata
	friend class MetadataWriter;
	// Special permission for Reader to build Metadata
	friend class Reader;
};

inline std::ostream& operator<<(std::ostream& stream, const Metadata& md)
{
	pt::ptree json;
	md.serialize(json, stream);
	return stream;
}

///
/// Special purpose class that is meant to be subclassed to create classes able to build Metadata.
/// Subclasses can call the static method `write` to build a Metadata.
///
/// Example:
///
/// ```cpp
/// class TestMDWriter : MetadataWriter {
/// public:
///		static constexpr std::uint32_t type = 42;
///		static constexpr char format_version[] = "1.0.0-dummy";
///		static constexpr char tool_name[] = "TestMetaDataWriter";
///		static constexpr char tool_version[] = "1.0.0-dummy";
///		static constexpr char tool_info[] = "Tests version 1.0.0";
///		static constexpr std::uint64_t generation_date = 42424242;
///
///		static Metadata dummy_md() {
///			return write(type, format_version, tool_name, tool_info, generation_date);
///		}
/// };
///
/// // clients can then call `TestMDWriter::dummy_md()` to build metadata.
/// ```
class MetadataWriter {
protected:
	static Metadata write(std::uint32_t type, std::string format_version,
	                      std::string tool_name, std::string tool_version, std::string tool_info,
	                      std::uint64_t generation_date, const CustomMetadata& custom_metadata={}) {
		Metadata md;
		md.type_ = type;
		md.format_version_ = std::move(format_version);
		md.tool_name_ = std::move(tool_name);
		md.tool_version_ = std::move(tool_version);
		md.tool_info_ = std::move(tool_info);
		md.generation_date_ = generation_date;
		md.custom_metadata_ = custom_metadata;
		return md;
	}
};

}} // namespace reven::jsonresource
