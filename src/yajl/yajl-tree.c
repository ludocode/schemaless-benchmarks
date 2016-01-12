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
#include "yajl/yajl_tree.h"
#include "yajl/yajl_version.h"

static char* file_data;
static size_t file_size;

static bool hash_node(yajl_val node, uint32_t* hash) {
    switch (node->type) {
        case yajl_t_null: *hash = hash_nil(*hash); return true;
        case yajl_t_true: *hash = hash_bool(*hash, true); return true;
        case yajl_t_false: *hash = hash_bool(*hash, false); return true;

        case yajl_t_number:
            if (YAJL_IS_INTEGER(node)) {
                *hash = hash_i64(*hash, YAJL_GET_INTEGER(node));
                return true;
            }
            if (YAJL_IS_DOUBLE(node)) {
                *hash = hash_double(*hash, YAJL_GET_DOUBLE(node));
                return true;
            }
            // We don't handle big integers. (The benchmark data won't contain
            // any, but we have to safely handle it anyway.)
            return false;

        case yajl_t_string: {
            // It seems YAJL tree strings are only null-terminated; the length is 
            // not provided anywhere in the node, so we need to strlen it :(
            const char* str = YAJL_GET_STRING(node);
            *hash = hash_str(*hash, str, strlen(str));
            return true;
        }

        case yajl_t_array: {
            size_t count = YAJL_GET_ARRAY(node)->len;
            for (size_t i = 0; i < count; ++i)
                if (!hash_node(YAJL_GET_ARRAY(node)->values[i], hash))
                    return false;
            *hash = hash_u32(*hash, count);
            return true;
        }

        case yajl_t_object: {
            size_t count = YAJL_GET_OBJECT(node)->len;
            for (uint32_t i = 0; i < count; ++i) {

                // we expect keys to be short strings (also null-terminated)
                const char* key = YAJL_GET_OBJECT(node)->keys[i];
                *hash = hash_str(*hash, key, strlen(key));

                if (!hash_node(YAJL_GET_OBJECT(node)->values[i], hash))
                    return false;
            }
            *hash = hash_u32(*hash, count);
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

    char errbuf[1024];
    yajl_val node = yajl_tree_parse(data, errbuf, sizeof(errbuf));
    if (node == NULL) {
        benchmark_in_situ_free(data);
        return false;
    }

    bool ok = hash_node(node, hash_out);
    yajl_tree_free(node);
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

