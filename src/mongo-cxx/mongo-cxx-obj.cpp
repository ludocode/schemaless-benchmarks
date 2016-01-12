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
#include "mongo/bson/bson.h"
#include "mongo/version.h"

static char* file_data;
static size_t file_size;

bool hash_bson(mongo::BSONObj& obj, bool is_array, uint32_t* hash) {
    size_t count = 0;
    auto it = obj.begin();
    while (it.more()) {
        mongo::BSONElement e = it.next();
        if (e.eoo())
            break;
        ++count;
        
        if (!is_array) {
            mongo::StringData key = e.fieldNameStringData();
            *hash = hash_str(*hash, key.rawData(), key.size());
        }

        switch (e.type()) {
            case mongo::jstNULL: *hash = hash_nil(*hash); break;
            case mongo::Bool: *hash = hash_bool(*hash, e.Bool()); break;
            case mongo::NumberInt: *hash = hash_i64(*hash, e.numberInt()); break;
            case mongo::NumberLong: *hash = hash_i64(*hash, e.numberLong()); break;
            case mongo::NumberDouble: *hash = hash_double(*hash, e.numberDouble()); break;

            case mongo::String: {
                mongo::StringData str = e.checkAndGetStringData();
                *hash = hash_str(*hash, str.rawData(), str.size());
                break;
            }

            case mongo::Object: {
                mongo::BSONObj obj = e.Obj();
                hash_bson(obj, false, hash);
                break;
            }

            case mongo::Array: {
                mongo::BSONObj obj = e.Obj();
                hash_bson(obj, true, hash);
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

    bool ok = true;
    try {
        // there doesn't seem to be a BSONObj constructor that takes a
        // length. how is it supposed to check whether the data was
        // truncated? will it just read uninitialized memory??
        mongo::BSONObj obj(file_data);

        // as with libbson-iter, we look at the first key to see if
        // it's a string zero to test whether we have an array or map.
        bool is_array = (0 == strcmp(obj.begin().next().fieldName(), "0"));

        ok = hash_bson(obj, is_array, hash_out);

    } catch (...) {
        ok = false;
    }
    benchmark_in_situ_free(data);
    return ok;
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
    return mongo::client::kVersionString;
}

const char* test_language(void) {
    return BENCHMARK_LANGUAGE_CXX;
}

const char* test_format(void) {
    return "BSON";
}

const char* test_filename(void) {
    return __FILE__;
}
