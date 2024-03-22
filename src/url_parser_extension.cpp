#define DUCKDB_EXTENSION_MAIN

#include "url_parser_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

#include "boost/url/url_view.hpp"
#include "boost/url/parse.hpp"

namespace duckdb {

inline void UrlParserScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
        name_vector, result, args.size(),
        [&](string_t url) {
            string url_string = url.GetString();
            boost::system::result<boost::urls::url_view> parse_result = boost::urls::parse_uri( url_string );
            if (parse_result.has_error() || !parse_result.value().has_scheme()) {
            return string_t();
            }
            string scheme = parse_result.value().scheme();
            return StringVector::AddString(result, scheme);
        });
}

static void LoadInternal(DatabaseInstance &instance) {
    auto url_parser_scalar_function = ScalarFunction("url_scheme", {LogicalType::VARCHAR}, LogicalType::VARCHAR, UrlParserScalarFun);
    ExtensionUtil::RegisterFunction(instance, url_parser_scalar_function);
}

void UrlParserExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string UrlParserExtension::Name() {
	return "url_parser";
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void url_parser_init(duckdb::DatabaseInstance &db) {
  duckdb::DuckDB db_wrapper(db);
  db_wrapper.LoadExtension<duckdb::UrlParserExtension>();
}

DUCKDB_EXTENSION_API const char *url_parser_version() {
  return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
