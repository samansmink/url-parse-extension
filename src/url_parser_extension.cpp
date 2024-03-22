#define DUCKDB_EXTENSION_MAIN

#include "url_parser_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

// OpenSSL linked through vcpkg
#include <openssl/opensslv.h>

namespace duckdb {

inline void UrlParserScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "UrlParser "+name.GetString()+" 🐥");;
        });
}

inline void UrlParserOpenSSLVersionScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "UrlParser " + name.GetString() +
                                                     ", my linked OpenSSL version is " +
                                                     OPENSSL_VERSION_TEXT );;
        });
}

static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    auto url_parser_scalar_function = ScalarFunction("url_parser", {LogicalType::VARCHAR}, LogicalType::VARCHAR, UrlParserScalarFun);
    ExtensionUtil::RegisterFunction(instance, url_parser_scalar_function);

    // Register another scalar function
    auto url_parser_openssl_version_scalar_function = ScalarFunction("url_parser_openssl_version", {LogicalType::VARCHAR},
                                                LogicalType::VARCHAR, UrlParserOpenSSLVersionScalarFun);
    ExtensionUtil::RegisterFunction(instance, url_parser_openssl_version_scalar_function);
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
