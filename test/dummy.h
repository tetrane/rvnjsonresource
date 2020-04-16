#pragma once

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include "common.h"
#include "metadata.h"

using MD = reven::jsonresource::Metadata;

constexpr const char* no_metadata_json = "{}";
constexpr const char* metadata_json = // contains TestMDWriter::dummy_md() metadata
	"{\n"
	"    \"metadata\": {\n"
	"        \"metadata_version\": \"1\",\n"
	"        \"type\": \"42\",\n"
	"        \"format_version\": \"1.0.0-dummy\",\n"
	"        \"tool_version\": \"1.0.0\",\n"
	"        \"tool_name\": \"TestMetaDataWriter\",\n"
	"        \"tool_info\": \"Tests version 1.0.0\",\n"
	"        \"generation_date\": \"42424242\"\n"
	"    }\n"
	"}";
const char* no_metadata_version_json =
	"{\n"
	"    \"metadata\": {\n"
	"    }\n"
	"}";
constexpr const char* incompatible_metadata_version_json =
	"{\n"
	"    \"metadata\": {\n"
	"        \"metadata_version\": \"3\",\n"
	"        \"type\": \"42\",\n"
	"        \"format_version\": \"1.0.0-dummy\",\n"
	"        \"tool_version\": \"1.0.0\",\n"
	"        \"tool_name\": \"TestMetaDataWriter\",\n"
	"        \"tool_info\": \"Tests version 1.0.0\",\n"
	"        \"generation_date\": \"42424242\"\n"
	"    }\n"
	"}";
constexpr const char* incomplete_metadata_json =
	"{\n"
	"    \"metadata\": {\n"
	"        \"metadata_version\": \"1\",\n"
	"        \"type\": \"42\",\n"
	"        \"format_version\": \"1.0.0-dummy\"\n"
	"    }\n"
	"}";
constexpr const char* bad_metadata_field_json = // contains TestMDWriter::dummy_md() metadata
	"{\n"
	"    \"metadata\": {\n"
	"        \"metadata_version\": \"1\",\n"
	"        \"type\": \"0x42\",\n"
	"        \"format_version\": \"1.0.0-dummy\",\n"
	"        \"tool_version\": \"1.0.0\",\n"
	"        \"tool_name\": \"TestMetaDataWriter\",\n"
	"        \"tool_info\": \"Tests version 1.0.0\",\n"
	"        \"generation_date\": \"42424242\"\n"
	"    }\n"
	"}";
constexpr const char* valid_json = "{\"toto\": \"0\"}";
constexpr const int dummy_type = 42;
constexpr const char* dummy_format_version = "1.0.0-dummy";
constexpr const char* dummy_tool_name = "TestMetaDataWriter";
constexpr const char* dummy_tool_version = "1.0.0";
constexpr const char* dummy_tool_info = "Tests version 1.0.0";
constexpr const int dummy_generation_date = 42424242;

// Allows to write MD
class TestMDWriter : reven::jsonresource::MetadataWriter {
public:
	static MD dummy_md() {
		return write(dummy_type,
		             dummy_format_version,
		             dummy_tool_name,
		             dummy_tool_version,
		             dummy_tool_info,
		             dummy_generation_date);
	}
	static MD dummy_md2() {
		return write(dummy_type * 2,
		             dummy_format_version,
		             dummy_tool_name,
		             dummy_tool_version,
		             dummy_tool_info,
		             dummy_generation_date);
	}
};

struct transient_directory {
	//! Path of created directory.
	boost::filesystem::path path;

	//! Create a uniquely named temporary directory in base_dir.
	//! A suffix is generated and appended to the given prefix to ensure the directory name is unique.
	//! Throw if directory cannot be created.
	transient_directory(const boost::filesystem::path& base_dir = boost::filesystem::temp_directory_path(),
	                    std::string prefix = {}) {
		boost::filesystem::path tmp_path = boost::filesystem::unique_path(prefix + "%%%%-%%%%-%%%%-%%%%");
		tmp_path = base_dir / tmp_path;

		if (!boost::filesystem::create_directories(tmp_path)) {
			throw std::runtime_error(("Can't create the directory " + tmp_path.native()).c_str());
		}

		this->path = tmp_path;
	}

	//! Delete created directory.
	~transient_directory() {
		boost::filesystem::remove_all(this->path);
	}
};

void init_json_file(const std::string& filename, const std::string& text)
{
	std::ofstream stream(filename);
	stream << text;
	stream.close();
}
