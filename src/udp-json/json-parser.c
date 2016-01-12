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
#include "json.h"

static char* file_data;
static size_t file_size;

static bool hash_json(json_value* value, uint32_t* hash) {
    switch (value->type) {
        case json_null:    *hash = hash_nil(*hash); return true;
        case json_boolean: *hash = hash_bool(*hash, value->u.boolean ? true : false); return true;
        case json_double:  *hash = hash_double(*hash, value->u.dbl); return true;
        case json_string:  *hash = hash_str(*hash, value->u.string.ptr, value->u.string.length); return true;

        case json_integer:
            // json-parser does not support JSON big integers at all.
            // Judging from the code, it looks like it just overflows and
            // gives garbage with no error if an integer is outside the
            // range of int64_t.
            *hash = hash_i64(*hash, value->u.integer);
            return true;

        case json_array: {
            for (unsigned int i = 0; i < value->u.array.length; ++i)
                if (!hash_json(value->u.array.values[i], hash))
                    return false;
            *hash = hash_u32(*hash, value->u.array.length);
            return true;
        }

        case json_object: {
            for (unsigned int i = 0; i < value->u.object.length; ++i) {
                json_object_entry* entry = &value->u.object.values[i];
                *hash = hash_str(*hash, entry->name, entry->name_length);
                if (!hash_json(entry->value, hash))
                    return false;
            }
            *hash = hash_u32(*hash, value->u.object.length);
            return true;
        }

        default:
            break;
    }

    return false;
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    json_value* root = json_parse(data, file_size);
    if (!root) {
        benchmark_in_situ_free(data);
        return false;
    }

    bool ok = hash_json(root, hash_out);
    json_value_free(root);
    benchmark_in_situ_free(data);
    return ok;
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
    return BENCHMARK_JSON_PARSER_VERSION;
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

