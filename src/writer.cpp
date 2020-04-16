#include "writer.h"
#include "common.h"

#include <ostream>
#include <fstream>

namespace reven {
namespace jsonresource {

namespace {

bool is_empty(std::ifstream& stream)
{
	return stream.peek() == std::ifstream::traits_type::eof();
}

}

Writer Writer::create(const char* filename, const Metadata& md)
{
	pt::ptree json;
	std::ifstream input(filename);
	if (not is_empty(input)) {
		try {
			pt::read_json(input, json);
		} catch (const std::exception& e) {
			throw WriterError((std::string("Can't read Json input: ") + e.what()).c_str());
		}
	}
	input.close();
	return Writer::create(json, std::make_unique<std::ofstream>(filename, std::fstream::trunc), md);
}

Writer Writer::create(pt::ptree& json, std::unique_ptr<std::ostream>&& stream, const Metadata& md)
{
	if (!*stream) {
		throw WriterError("Bad stream");
	}
	Writer writer(json, std::move(stream));

	json = writer.json();

	const auto& jmetadata = json.get_child_optional("metadata");
	if (jmetadata) {
		throw WriterError("Can't create a Json resource file already containing metadata.");
	}

	writer.set_metadata(md);

	return writer;
}

Writer Writer::open(const char* filename)
{
	pt::ptree json;
	std::ifstream input(filename);
	if (not is_empty(input)) {
		try {
			pt::read_json(input, json);
		} catch (const std::exception& e) {
			throw WriterError((std::string("Can't read Json input: ") + e.what()).c_str());
		}
	}
	input.close();
	return Writer::open(json, std::make_unique<std::ofstream>(filename, std::fstream::trunc));
}

Writer Writer::open(pt::ptree& json, std::unique_ptr<std::ostream>&& stream)
{
	if (!*stream) {
		throw WriterError("Bad stream");
	}
	Writer writer(json, std::move(stream));

	json = writer.json();

	// Check metadata structure
	Metadata::read_metadata(json);

	return writer;
}

void Writer::set_metadata(const Metadata& md)
{
	json_.erase("metadata");
	md.serialize(json_, *stream_);
}

}} // namespace reven::jsonresource
