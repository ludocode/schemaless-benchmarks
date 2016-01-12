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
#include "bson.h"

static char* file_data;
static size_t file_size;

bool hash_bson(bson_iter_t* iter, bool is_array, uint32_t* hash) {
    uint32_t count = 0;
    while (bson_iter_next(iter)) {
        ++count;
        
        if (!is_array) {
            const char* key = bson_iter_key(iter);
            *hash = hash_str(*hash, key, strlen(key));
        }

        switch (bson_iter_type(iter)) {
            case BSON_TYPE_NULL: *hash = hash_nil(*hash); break;
            case BSON_TYPE_BOOL: *hash = hash_bool(*hash, bson_iter_bool(iter)); break;
            case BSON_TYPE_INT32: *hash = hash_i64(*hash, bson_iter_int32(iter)); break;
            case BSON_TYPE_INT64: *hash = hash_i64(*hash, bson_iter_int64(iter)); break;
            case BSON_TYPE_DOUBLE: *hash = hash_double(*hash, bson_iter_double(iter)); break;

            case BSON_TYPE_UTF8: {
                uint32_t length;
                const char* str = bson_iter_utf8(iter, &length);
                *hash = hash_str(*hash, str, length);
                break;
            }

            case BSON_TYPE_DOCUMENT: {
                bson_iter_t child;
                bool ret = bson_iter_recurse(iter, &child);
                if (ret)
                    hash_bson(&child, false, hash);
                else
                    return false;
                break;
            }

            case BSON_TYPE_ARRAY: {
                bson_iter_t child;
                bool ret = bson_iter_recurse(iter, &child);
                if (ret)
                    hash_bson(&child, true, hash);
                else
                    return false;
                break;
            }

            default:
                return false;
        }
    }

    *hash = hash_u32(*hash, count);
    return true;
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    bson_t bson;
    bool ret = bson_init_static(&bson, (const uint8_t*)data, file_size);
    if (ret) {
        bson_iter_t iter;

        // The BSON top-level document doesn't contain its own type, so
        // it doesn't say whether it's an array. This seems like a
        // serious flaw in BSON since it's supposed to be compatible
        // with JSON, but JSON supports both arrays and objects as the
        // document root. I feel like I'm misunderstanding how this should
        // work so feel free to correct me here. In the meantime, as a
        // workaround we just look at the first element and check if it's
        // a string zero.
        bool is_array = false;
        ret &= bson_iter_init(&iter, &bson);
        if (ret)
            is_array = (bson_iter_next(&iter) && strcmp(bson_iter_key(&iter), "0") == 0);

        ret &= bson_iter_init(&iter, &bson);
        if (ret) {
            *hash_out = HASH_INITIAL_VALUE;
            ret &= hash_bson(&iter, is_array, hash_out);
        }
    }

    // The documentation says that bson_destroy() should be called
    // regardless of whether the bson_t was initialized via bson_init(),
    // bson_new() or BSON_INITIALIZER. This is because it stores a flag
    // to say whether it should be freed when destroyed.
    // This causes a warning under -flto about freeing a stack object
    // even though the bson_t is set for static.
    bson_destroy(&bson);

    benchmark_in_situ_free(data);
    return ret;
}

bool setup_test(size_t object_size) {
    file_data = load_data_file(BENCHMARK_FORMAT_BSON, object_size, &file_size);
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
    return BSON_VERSION_S;
}

const char* test_language(void) {
    return BENCHMARK_LANGUAGE_C;
}

const char* test_format(void) {
    return "BSON";
}

const char* test_filename(void) {
    return __FILE__;
}
