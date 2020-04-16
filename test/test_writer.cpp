#define BOOST_TEST_MODULE RVN_JSONRESOURCE_WRITER
#include <boost/test/unit_test.hpp>

#include <sstream>

#include "common.h"
#include "metadata.h"
#include "writer.h"
#include "reader.h"
#include "dummy.h"

using MD = reven::jsonresource::Metadata;
using Reader = reven::jsonresource::Reader;
using Writer = reven::jsonresource::Writer;

BOOST_AUTO_TEST_CASE(open_valid_metadata)
{
	transient_directory tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, metadata_json);

	BOOST_CHECK_NO_THROW(Writer::open(tmp_file.c_str()));
}

BOOST_AUTO_TEST_CASE(open_no_metadata)
{
	transient_directory  tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, no_metadata_json);

	BOOST_CHECK_THROW(Writer::open(tmp_file.c_str()), reven::jsonresource::MissingMetadata);
}

BOOST_AUTO_TEST_CASE(open_no_metadata_version)
{
	transient_directory  tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, no_metadata_version_json);

	BOOST_CHECK_THROW(Writer::open(tmp_file.c_str()), reven::jsonresource::MissingMetadataVersion);
}

BOOST_AUTO_TEST_CASE(open_incompatible_metadata_version)
{
	transient_directory  tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, incompatible_metadata_version_json);

	BOOST_CHECK_THROW(Writer::open(tmp_file.c_str()), reven::jsonresource::IncompatibleMetadataVersion);
}

BOOST_AUTO_TEST_CASE(open_incomplete_metadata)
{
	transient_directory  tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, incomplete_metadata_json);

	BOOST_CHECK_THROW(Writer::open(tmp_file.c_str()), reven::jsonresource::MissingMetadataField);
}

BOOST_AUTO_TEST_CASE(open_and_set_metadata)
{
	transient_directory tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, metadata_json);

	auto md = TestMDWriter::dummy_md2();
	{
		auto writer = Writer::open(tmp_file.c_str());
		BOOST_CHECK_NO_THROW(writer.set_metadata(md));
	}
	{
		auto reader = Reader::open(tmp_file.c_str());
		BOOST_CHECK_EQUAL(md, reader.metadata());
	}
}

BOOST_AUTO_TEST_CASE(create_metadata)
{
	transient_directory tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, no_metadata_json);

	auto md = TestMDWriter::dummy_md();

	{
		BOOST_CHECK_THROW(Reader::open(tmp_file.c_str()), reven::jsonresource::MissingMetadata);
	}
	{
		BOOST_CHECK_NO_THROW(Writer::create(tmp_file.c_str(), md));
	}
	{
		auto reader = Reader::open(tmp_file.c_str());
		BOOST_CHECK_EQUAL(md, reader.metadata());
	}
}

BOOST_AUTO_TEST_CASE(impossible_create_metadata)
{
	transient_directory tmp_dir{};
	const auto tmp_file = (tmp_dir.path / "foo.json").generic_string();
	init_json_file(tmp_file, metadata_json);

	auto md = TestMDWriter::dummy_md();

	BOOST_CHECK_THROW(Writer::create(tmp_file.c_str(), md), reven::jsonresource::WriterError);
}
