#define BOOST_TEST_MODULE RVN_JSONRESOURCE_METADATA
#include <boost/test/unit_test.hpp>

#include <sstream>
#include <fstream>

#include "dummy.h"
#include "common.h"
#include "metadata.h"

pt::ptree json_from(const std::string& jsontext)
{
	std::stringstream istream(jsontext);
	pt::ptree json;
	pt::read_json(istream, json);
	return json;
}

BOOST_AUTO_TEST_CASE(serialize_deserialize)
{
	auto json = json_from(valid_json);
	std::stringstream stream;
	const auto md = TestMDWriter::dummy_md();

	md.serialize(json, stream);

	const auto md2 = MD::deserialize(stream);

	BOOST_CHECK_EQUAL(md.type(), md2.type());
	BOOST_CHECK_EQUAL(md.format_version(), md2.format_version());
	BOOST_CHECK_EQUAL(md.tool_name(), md2.tool_name());
	BOOST_CHECK_EQUAL(md.tool_version(), md2.tool_version());
	BOOST_CHECK_EQUAL(md.tool_info(), md2.tool_info());
	BOOST_CHECK_EQUAL(md.generation_date(), md2.generation_date());
}

BOOST_AUTO_TEST_CASE(multiple_serialize)
{
	auto json = json_from(valid_json);
	const auto md = TestMDWriter::dummy_md();

	md.write_metadata(json);
	BOOST_CHECK_THROW(md.write_metadata(json), reven::jsonresource::MetadataError);
}

BOOST_AUTO_TEST_CASE(deserialize_empty)
{
	auto json = json_from(valid_json);

	BOOST_CHECK_THROW(MD::read_metadata(json),
	                  reven::jsonresource::ReadMetadataError);
}


void test_metadata_version(std::uint32_t metadata_version)
{
	auto json = json_from(valid_json);

	///////////////////////////
	// METADATA Field
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadata);
	pt::ptree metadata;
	json.add_child("metadata", metadata);

	///////////////////////////
	// METADATA VERSION
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadataVersion);
	json.put("metadata.metadata_version", metadata_version);

	if (metadata_version > ::reven::jsonresource::metadata_version) {
		BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::IncompatibleMetadataVersion);
		return;
	}
}

BOOST_AUTO_TEST_CASE(deserialize_incompatible_metadata)
{
	test_metadata_version(::reven::jsonresource::metadata_version + 1);
}

BOOST_AUTO_TEST_CASE(deserialize_compatible_metadata)
{
	test_metadata_version(::reven::jsonresource::metadata_version);
	test_metadata_version(::reven::jsonresource::metadata_version - 1);
}

void test_incomplete_metadata()
{
	auto json = json_from(valid_json);

	///////////////////////////
	// METADATA Field
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadata);
	pt::ptree metadata;
	json.add_child("metadata", metadata);

	///////////////////////////
	// METADATA VERSION
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadataVersion);
	json.put("metadata.metadata_version", ::reven::jsonresource::metadata_version);

	///////////////////////////
	// TYPE
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadataField);
	json.put("metadata.type", dummy_type);

	///////////////////////////
	// FORMAT VERSION
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadataField);
	json.put("metadata.format_version", dummy_format_version);

	///////////////////////////
	// TOOL NAME
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadataField);
	json.put("metadata.tool_name", dummy_tool_name);

	///////////////////////////
	// TOOL VERSION
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadataField);
	json.put("metadata.tool_version", dummy_tool_version);

	///////////////////////////
	// TOOL INFO
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadataField);
	json.put("metadata.tool_info", dummy_tool_info);

	///////////////////////////
	// GENERATION DATE
	///////////////////////////

	BOOST_CHECK_THROW(MD::read_metadata(json), reven::jsonresource::MissingMetadataField);
	json.put("metadata.generation_date", dummy_generation_date);

	auto md = MD::read_metadata(json);

	BOOST_CHECK_EQUAL(md.type(), dummy_type);
	BOOST_CHECK_EQUAL(md.format_version(), dummy_format_version);
	BOOST_CHECK_EQUAL(md.tool_name(), dummy_tool_name);
	BOOST_CHECK_EQUAL(md.tool_version(), dummy_tool_version);
	BOOST_CHECK_EQUAL(md.tool_info(), dummy_tool_info);
	BOOST_CHECK_EQUAL(md.generation_date(), dummy_generation_date);
}

BOOST_AUTO_TEST_CASE(deserialize_incomplete_metadata)
{
	test_incomplete_metadata();
}
