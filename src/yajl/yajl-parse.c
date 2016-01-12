/*
 * Copyright (c) 2016 Nicholas Fraser
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "benchmark.h"
#include "yajl/yajl_parse.h"
#include "yajl/yajl_version.h"

static char* file_data;
static size_t file_size;

typedef struct parser_t {
    uint32_t hash;
    int32_t depth;
    uint32_t children[32];
} parser_t;

static void parser_init(parser_t* parser, uint32_t initial_value) {
    parser->hash = initial_value;
    parser->depth = -1;
}

static int parse_null(void* ctx) {
    parser_t* parser = (parser_t*)ctx;
    ++parser->children[parser->depth];
    parser->hash = hash_nil(parser->hash);
    return 1;
}

static int parse_boolean(void* ctx, int boolean) {
    parser_t* parser = (parser_t*)ctx;
    ++parser->children[parser->depth];
    parser->hash = hash_bool(parser->hash, boolean);
    return 1;
}

static int parse_integer(void* ctx, long long val) {
    parser_t* parser = (parser_t*)ctx;
    ++parser->children[parser->depth];
    parser->hash = hash_i64(parser->hash, val);
    return 1;
}

static int parse_double(void* ctx, double val) {
    parser_t* parser = (parser_t*)ctx;
    ++parser->children[parser->depth];
    parser->hash = hash_double(parser->hash, val);
    return 1;
}

static int parse_string(void* ctx, const unsigned char* str, size_t len) {
    parser_t* parser = (parser_t*)ctx;
    ++parser->children[parser->depth];
    parser->hash = hash_str(parser->hash, (const char*)str, len);
    return 1;
}

static int parse_map_key(void* ctx, const unsigned char* str, size_t len) {
    parser_t* parser = (parser_t*)ctx;
    parser->hash = hash_str(parser->hash, (const char*)str, len);
    return 1;
}

static int parse_start_compound(void* ctx) {
    parser_t* parser = (parser_t*)ctx;
    ++parser->children[parser->depth];
    if (parser->depth + 1 >= sizeof(parser->children) / sizeof(*parser->children))
        return 0;
    ++parser->depth;
    parser->children[parser->depth] = 0;
    return 1;
}

static int parse_end_compound(void* ctx) {
    parser_t* parser = (parser_t*)ctx;
    parser->hash = hash_u32(parser->hash, parser->children[parser->depth]);
    --parser->depth;
    return 1;
}

static yajl_callbacks callbacks = {
    parse_null,
    parse_boolean,
    parse_integer,
    parse_double,
    NULL,
    parse_string,
    parse_start_compound,
    parse_map_key,
    parse_end_compound,
    parse_start_compound,
    parse_end_compound
};

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    parser_t parser;
    parser_init(&parser, *hash_out);

    yajl_handle handle = yajl_alloc(&callbacks, NULL, &parser);
    yajl_status status = yajl_parse(handle, (const unsigned char*)file_data, file_size);
    if (status == yajl_status_ok)
        status = yajl_complete_parse(handle);
    yajl_free(handle);

    *hash_out = parser.hash;
    benchmark_in_situ_free(data);
    return status == yajl_status_ok;
}

bool setup_test(size_t object_size) {
    file_data = load_data_file(BENCHMARK_FORMAT_JSON, object_size, &file_size);
    if (!file_data)
        return false;
	return true;
}

void teardown_test(void) {
    free(file_data);
}

bool is_benchmark(void) {
    return true;
}

const char* test_version(void) {
    static char buf[16];
    snprintf(buf, sizeof(buf), "%u.%u.%u", YAJL_MAJOR, YAJL_MINOR, YAJL_MICRO);
    return buf;
}

const char* test_language(void) {
    return BENCHMARK_LANGUAGE_C;
}

const char* test_format(void) {
    return "JSON";
}

const char* test_filename(void) {
    return __FILE__;
}

