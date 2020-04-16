#include "metadata.h"
#include "common.h"

#include <string>
#include <iostream>
#include <sstream>

namespace reven {
namespace jsonresource {

void Metadata::serialize(pt::ptree& json, std::ostream& out) const
{
	write_metadata(json);
	try {
		pt::write_json(out, json);
	} catch (const std::exception& e) {
		throw WriteMetadataError((std::string("Can't write Json output: ") + e.what()).c_str());
	}
}

void Metadata::write_metadata(pt::ptree& json) const
{

	{
		const auto& jmetadata = json.get_child_optional("metadata");
		if (jmetadata) {
			throw MetadataError("Can't serialize metadata, since a metadata is already serialized.");
		}
	}

	pt::ptree jmetadata;
	try {
		jmetadata.put("metadata_version", metadata_version);
		jmetadata.put("type", type_);
		jmetadata.put("format_version", format_version_);
		jmetadata.put("tool_version", tool_version_);
		jmetadata.put("tool_name", tool_name_);
		jmetadata.put("tool_info", tool_info_);
		jmetadata.put("generation_date", generation_date_);

		if (not custom_metadata_.empty()) {
			pt::ptree jcustom_metadata;
			for (const auto& custom : custom_metadata_) {
				jcustom_metadata.put(custom.first, custom.second);
			}
			jmetadata.add_child("custom", jcustom_metadata);
		}

		json.add_child("metadata", jmetadata);
	} catch (const std::exception& e) {
		throw WriteMetadataError((std::string("Can't write Json output: ") + e.what()).c_str());
	}
}

Metadata Metadata::read_metadata(pt::ptree& json)
{
	Metadata md;

	try {
		json.get_child("metadata");
	} catch (const std::exception&) {
		throw MissingMetadata("Missing \"metadata\" field");
	}

	std::uint32_t metadata_version = 0;
	try {
		metadata_version = json.get<std::uint32_t>("metadata.metadata_version");
	} catch (const std::exception&) {
		throw MissingMetadataVersion("Missing \"metadata_version\" field");
	}

	if (metadata_version > ::reven::jsonresource::metadata_version) {
		std::stringstream msg;
		msg << "Can't read a metadata with metadata version bigger than the current"
		    << " (" << metadata_version <<  " > " << ::reven::jsonresource::metadata_version << ")";
		throw IncompatibleMetadataVersion(msg.str().c_str());
	}

	try {
		md.type_ = json.get<std::uint32_t>("metadata.type");
		md.format_version_ = json.get<std::string>("metadata.format_version");
		md.tool_version_ = json.get<std::string>("metadata.tool_version");
		md.tool_name_ = json.get<std::string>("metadata.tool_name");
		md.tool_info_ = json.get<std::string>("metadata.tool_info");
		md.generation_date_ = json.get<std::uint64_t>("metadata.generation_date");
	} catch (const pt::ptree_bad_path& e) {
		throw MissingMetadataField((std::string("Missing metadata field: ") + e.what()).c_str());
	} catch (const pt::ptree_bad_data& e) {
		throw BadMetadataField((std::string("Can't read a metadata field: ") + e.what()).c_str());
	}

	const auto custom_metadata = json.get_child_optional("metadata.custom");
	if (custom_metadata) {
		for (const boost::property_tree::ptree::value_type& custom : custom_metadata.value()) {
			md.custom_metadata_.emplace(custom.first, custom.second.data());
		}
	}

	return md;
}

Metadata Metadata::deserialize(std::istream& input)
{
	input.clear();
	input.seekg(0, std::ios::beg);
	pt::ptree json;
	try {
		pt::read_json(input, json);
	} catch (const std::exception& e) {
		throw ReadMetadataError((std::string("Can't read Json input: ") + e.what()).c_str());
	}

	return read_metadata(json);
}

}} // namespace reven::binresource
