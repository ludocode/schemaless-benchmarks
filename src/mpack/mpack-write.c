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
#include "mpack/mpack.h"

static object_t* root_object;

static void write_object(mpack_writer_t* writer, object_t* object) {
    switch (object->type) {
        case type_nil:    mpack_write_nil   (writer);                         break; 
        case type_bool:   mpack_write_bool  (writer, object->b);              break; 
        case type_double: mpack_write_double(writer, object->d);              break; 
        case type_int:    mpack_write_i64   (writer, object->i);              break; 
        case type_uint:   mpack_write_u64   (writer, object->u);              break; 
        case type_str:    mpack_write_str   (writer, object->str, object->l); break; 

        case type_array:
            mpack_start_array(writer, object->l);
            for (size_t i = 0; i < object->l; ++i)
                write_object(writer, object->children + i);
            mpack_finish_array(writer);
            break;

        case type_map:
            mpack_start_map(writer, object->l);

            for (size_t i = 0; i < object->l; ++i) {

                // we expect keys to be short strings
                object_t* key = object->children + i * 2;
                assert(key->type == type_str);
                mpack_write_str(writer, key->str, key->l);

                write_object(writer, object->children + i * 2 + 1);
            }
            mpack_finish_map(writer);
            break;

        default:
            assert(0);
            mpack_writer_flag_error(writer, mpack_error_bug);
            break;
    }
}

bool run_test(uint32_t* hash_out) {
    char* data;
    size_t size;
    mpack_writer_t writer;
    mpack_writer_init_growable(&writer, &data, &size);

    write_object(&writer, root_object);

    mpack_error_t error = mpack_writer_destroy(&writer);
    if (error != mpack_ok)
        return false;

    *hash_out = hash_str(*hash_out, data, size);
    free(data);
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
    return MPACK_VERSION_STRING;
}

const char* test_language(void) {
    return BENCHMARK_LANGUAGE_C;
}

const char* test_format(void) {
    return "MessagePack";
}

const char* test_filename(void) {
    return __FILE__;
}
