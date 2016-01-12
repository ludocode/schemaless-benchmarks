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

struct Hasher {
    Hasher(uint32_t initial_value) : hash(initial_value) {}

    bool Null()             {hash = hash_nil    (hash   ); return true;}
    bool Bool(bool b)       {hash = hash_bool   (hash, b); return true;}
    bool Double(double d)   {hash = hash_double (hash, d); return true;}

    // note: all ints are hashed as 64-bit (not all libraries read different sized types)
    bool Int(int i)         {hash = hash_i64(hash, i); return true;}
    bool Uint(unsigned u)   {hash = hash_u64(hash, u); return true;}
    bool Int64(int64_t i)   {hash = hash_i64(hash, i); return true;}
    bool Uint64(uint64_t u) {hash = hash_u64(hash, u); return true;}

    bool String(const char* str, SizeType length, bool copy) {
        hash = hash_str(hash, str, length);
        return true;
    }
    bool Key(const char* str, SizeType length, bool copy) {
        hash = hash_str(hash, str, length);
        return true;
    }

    bool StartObject() {
        return true;
    }
    bool EndObject(SizeType memberCount) {
        hash = hash_u32(hash, memberCount);
        return true;
    }

    bool StartArray() {
        return true;
    }
    bool EndArray(SizeType elementCount) {
        hash = hash_u32(hash, elementCount);
        return true;
    }

    uint32_t hash;
};

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    try {
        Hasher hasher(*hash_out);
        Reader reader;
        #if BENCHMARK_IN_SITU
        // why isn't there a helper for sax in-situ parsing?
        InsituStringStream s(data);
        reader.Parse<kParseDefaultFlags|kParseInsituFlag>(s, hasher);
        #else
        MemoryStream s(data, file_size);
        reader.Parse(s, hasher);
        #endif
        *hash_out = hasher.hash;
    } catch (...) {
        benchmark_in_situ_free(data);
        return false;
    }

    benchmark_in_situ_free(data);
    return true;
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
