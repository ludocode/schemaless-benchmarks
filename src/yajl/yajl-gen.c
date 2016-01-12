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
#include "yajl/yajl_gen.h"
#include "yajl/yajl_version.h"

static object_t* root_object;

static bool gen_object(yajl_gen gen, object_t* object) {
    switch (object->type) {
        case type_bool:   return yajl_gen_bool(gen, object->b) == yajl_gen_status_ok;
        case type_nil:    return yajl_gen_null(gen) == yajl_gen_status_ok;
        case type_int:    return yajl_gen_integer(gen, object->i) == yajl_gen_status_ok;
        case type_double: return yajl_gen_double(gen, object->d) == yajl_gen_status_ok;

        case type_uint:
            // Note: The generator limits unsigned int to INT64_MAX. To print a number
            // in the range [INT64_MAX, UINT64_MAX) with YAJL, you would need to convert
            // it to string yourself (and parse all numbers from strings yourself as well.)
            return yajl_gen_integer(gen, (int64_t)object->u) == yajl_gen_status_ok;

        case type_str:
            return yajl_gen_string(gen, (const unsigned char*)object->str, object->l) == yajl_gen_status_ok;

        case type_array:
            if (yajl_gen_array_open(gen) != yajl_gen_status_ok)
                return false;
            for (size_t i = 0; i < object->l; ++i)
                if (!gen_object(gen, object->children + i))
                    return false;
            return yajl_gen_array_close(gen) == yajl_gen_status_ok;

        case type_map:
            if (yajl_gen_map_open(gen) != yajl_gen_status_ok)
                return false;
            for (size_t i = 0; i < object->l; ++i) {

                // we expect keys to be short strings
                object_t* key = object->children + i * 2;
                if (yajl_gen_string(gen, (const unsigned char*)key->str, key->l) != yajl_gen_status_ok)
                    return false;

                if (!gen_object(gen, object->children + i * 2 + 1))
                    return false;
            }
            return yajl_gen_map_close(gen) == yajl_gen_status_ok;

        default:
            break;
    }
    return false;
}

bool run_test(uint32_t* hash_out) {
    yajl_gen gen = yajl_gen_alloc(NULL);

    if (!gen_object(gen, root_object))
        return false;

    const unsigned char* buf;
    size_t len;
    yajl_gen_get_buf(gen, &buf, &len);
    *hash_out = hash_str(*hash_out, (const char*)buf, len);

    yajl_gen_free(gen);
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

