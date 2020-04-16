#define BOOST_TEST_MODULE RVN_JSONRESOURCE_READER
#include <boost/test/unit_test.hpp>

#include <sstream>
#include <iostream>

#include "common.h"
#include "metadata.h"
#include "reader.h"
#include "dummy.h"

using MD = reven::jsonresource::Metadata;
using Reader = reven::jsonresource::Reader;

BOOST_AUTO_TEST_CASE(valid_metadata)
{
	transient_directory tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, metadata_json);

	const auto& reader = Reader::open(tmp_file.c_str());
	BOOST_CHECK_EQUAL(reader.metadata(), TestMDWriter::dummy_md());
}

BOOST_AUTO_TEST_CASE(no_metadata)
{
	transient_directory  tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, no_metadata_json);

	BOOST_CHECK_THROW(Reader::open(tmp_file.c_str()), reven::jsonresource::MissingMetadata);
}

BOOST_AUTO_TEST_CASE(no_metadata_version)
{
	transient_directory  tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, no_metadata_version_json);

	BOOST_CHECK_THROW(Reader::open(tmp_file.c_str()), reven::jsonresource::MissingMetadataVersion);
}

BOOST_AUTO_TEST_CASE(incompatible_metadata_version)
{
	transient_directory  tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, incompatible_metadata_version_json);

	BOOST_CHECK_THROW(Reader::open(tmp_file.c_str()), reven::jsonresource::IncompatibleMetadataVersion);
}

BOOST_AUTO_TEST_CASE(incomplete_metadata)
{
	transient_directory  tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, incomplete_metadata_json);

	BOOST_CHECK_THROW(Reader::open(tmp_file.c_str()), reven::jsonresource::MissingMetadataField);
}

BOOST_AUTO_TEST_CASE(bad_metadata_field)
{
	transient_directory  tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, bad_metadata_field_json);

	BOOST_CHECK_THROW(Reader::open(tmp_file.c_str()), reven::jsonresource::BadMetadataField);
}
