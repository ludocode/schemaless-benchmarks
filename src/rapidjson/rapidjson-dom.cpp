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

#include "rapidjson/rapidjson.h"
#include "rapidjson/memorystream.h"
#include "rapidjson/document.h"

using namespace rapidjson;

static char* file_data;
static size_t file_size;

static bool hash_value(Value& value, uint32_t* hash) {
    switch (value.GetType()) {
        case kNullType:    *hash = hash_nil(*hash); return true;
        case kFalseType:   *hash = hash_bool(*hash, false); return true;
        case kTrueType:    *hash = hash_bool(*hash, true); return true;

        case kNumberType:
            if (value.IsDouble()) {
                *hash = hash_double(*hash, value.GetDouble());
                return true;
            }
            if (value.IsInt64()) {
                *hash = hash_i64(*hash, value.GetInt64());
                return true;
            }
            *hash = hash_u64(*hash, value.GetUint64());
            return true;

        case kStringType:
            *hash = hash_str(*hash, value.GetString(), value.GetStringLength());
            return true;

        case kArrayType: {
            auto it = value.Begin(), end = value.End();
            for (; it != end; ++it)
                if (!hash_value(*it, hash))
                    return false;
            *hash = hash_u32(*hash, value.Size());
            return true;
        }

        case kObjectType: {
            auto it = value.MemberBegin(), end = value.MemberEnd();
            for (; it != end; ++it) {
                *hash = hash_str(*hash, it->name.GetString(), it->name.GetStringLength());
                if (!hash_value(it->value, hash))
                    return false;
            }
            *hash = hash_u32(*hash, value.MemberCount());
            return true;
        }

        default:
            assert(0);
            break;
    }

    return false;
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    Document document;
    #if BENCHMARK_IN_SITU
    document.ParseInsitu(data);
    #else
    MemoryStream s(data, file_size);
    document.ParseStream(s);
    #endif

    bool ok = hash_value(document, hash_out);
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
    return RAPIDJSON_VERSION_STRING;
}

const char* test_language(void) {
    return BENCHMARK_LANGUAGE_CXX;
}

const char* test_format(void) {
    return "JSON";
}

const char* test_filename(void) {
    return __FILE__;
}
