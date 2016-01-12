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
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

static object_t* root_object;

static void write_object(Writer<StringBuffer>& writer, object_t* object) {
    switch (object->type) {
        case type_nil:    writer.Null();              break;
        case type_bool:   writer.Bool(object->b);     break;
        case type_double: writer.Double(object->d);   break;
        case type_int:    writer.Int64(object->i);    break;
        case type_uint:   writer.Uint64(object->u);   break;
        case type_str:    writer.String(object->str, object->l); break;

        case type_array:
            writer.StartArray();
            for (size_t i = 0; i < object->l; ++i)
                write_object(writer, object->children + i);
            writer.EndArray();
            break;

        case type_map:
            writer.StartObject();
            for (size_t i = 0; i < object->l; ++i) {
                writer.Key(object->children[i * 2].str, object->children[i * 2].l);
                write_object(writer, object->children + i * 2 + 1);
            }
            writer.EndObject();
            break;

        default:
            assert(0);
            break;
    }
}

bool run_test(uint32_t* hash_out) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    write_object(writer, root_object);
    *hash_out = hash_str(*hash_out, buffer.GetString(), buffer.GetSize());
    return true;
}

bool setup_test(size_t object_size) {
    root_object = benchmark_object_create(object_size);
    return true;
}

void teardown_test(void) {
    object_destroy(root_object);
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
