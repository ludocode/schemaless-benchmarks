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
#include "jansson.h"

// Jansson does not preserve order by default. We can turn this on
// to verify that it is hashing correctly, but otherwise we let it
// re-order the data however it wants. It seems marginally faster
// without preserving order (because it puts everything in hashtables,
// and has to re-sort by "serial number" to print in order.)

static object_t* root_object;

static json_t* convert(object_t* object) {
    switch (object->type) {
        case type_bool:   return json_boolean(object->b);
        case type_nil:    return json_null();
        case type_int:    return json_integer(object->i);
        case type_double: return json_real(object->d);

        case type_uint:
            // Note: The generator limits unsigned int to INT64_MAX. Jansson
            // doesn't allow integers outside the range of signed int64.
            return json_integer((int64_t)object->u);

        case type_str:
            return json_stringn((const char*)object->str, object->l);

        case type_array: {
            json_t* array = json_array();
            for (size_t i = 0; i < object->l; ++i) {
                json_t* child = convert(object->children + i);
                if (!child || json_array_append_new(array, child) != 0) {
                    json_decref(array);
                    return NULL;
                }
            }
            return array;
        }

        case type_map: {
            json_t* map = json_object();
            for (size_t i = 0; i < object->l; ++i) {
                const char* key = object->children[i * 2].str;
                json_t* child = convert(object->children + i * 2 + 1);
                if (!child || json_object_set_new(map, key, child) != 0) {
                    json_decref(map);
                    return NULL;
                }
            }
            return map;
        }

        default:
            assert(0);
            break;
    }
    return false;
}

bool run_test(uint32_t* hash_out) {
    json_t* root = convert(root_object);
    if (!root)
        return false;

    size_t flags = JSON_COMPACT;
    #if PRESERVE_ORDER
    flags |= JSON_PRESERVE_ORDER;
    #endif

    // we have to strlen() to get the result size i guess?
    char* result = json_dumps(root, flags);
    *hash_out = hash_str(*hash_out, result, strlen(result));
    free(result);

    json_decref(root);
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
    return JANSSON_VERSION;
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

