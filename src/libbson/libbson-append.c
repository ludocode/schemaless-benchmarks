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

static object_t* root_object;

static bool append_document(bson_t* bson, object_t* object);
static bool append_array(bson_t* bson, object_t* object);

// libbson doesn't automatically write the shortest int
static bool append_int(bson_t* bson, const char* key, size_t length, int64_t value) {
    if (value >= INT32_MIN && value <= INT32_MAX)
        bson_append_int32(bson, key, length, (int32_t)value);
    else
        bson_append_int64(bson, key, length, value);
}

static bool append_value(bson_t* bson, const char* key, size_t length, object_t* value) {
    switch (value->type) {
        case type_nil:    bson_append_null(bson, key, length);                       break; 
        case type_bool:   bson_append_bool(bson, key, length, value->b);             break; 
        case type_double: bson_append_double(bson, key, length, value->d);           break; 
        case type_str:    bson_append_utf8(bson, key, length, value->str, value->l); break; 

        case type_int:    append_int(bson, key, length, value->i);                   break; 
        case type_uint:   append_int(bson, key, length, (int64_t)value->u);          break;

        case type_map: {
            bson_t child;
            bson_append_document_begin(bson, key, length, &child);
            append_document(&child, value);
            bson_append_document_end(bson, &child);
        } break;

        case type_array: {
            bson_t child;
            bson_append_array_begin(bson, key, length, &child);
            append_array(&child, value);
            bson_append_array_end(bson, &child);
        } break;

        default:
            return false;
    }
    return true;
}

static bool append_document(bson_t* bson, object_t* object) {
    for (size_t i = 0; i < object->l; ++i) {
        object_t* key   = object->children + i * 2;
        object_t* value = object->children + i * 2 + 1;
        if (!append_value(bson, key->str, key->l, value))
            return false;
    }
    return true;
}

static bool append_array(bson_t* bson, object_t* object) {
    for (size_t i = 0; i < object->l; ++i) {
        char str[16];
        const char* key;
        size_t len = bson_uint32_to_string(i, &key, str, sizeof(str));
        if (!append_value(bson, key, len, object->children + i))
            return false;
    }
    return true;
}

bool run_test(uint32_t* hash_out) {
    bson_t bson = BSON_INITIALIZER;

    bool ok;
    if (root_object->type == type_map)
        ok = append_document(&bson, root_object);
    else
        ok = append_array(&bson, root_object);

    if (!ok) {
        fprintf(stderr, "libbson error writing data!\n");
        bson_destroy(&bson);
        return false;
    }

    *hash_out = hash_str(*hash_out, (const char*)bson_get_data(&bson), bson.len);

    // The documentation says that bson_destroy() should be called
    // regardless of whether the bson_t was initialized via bson_init()
    // bson_new() or BSON_INITIALIZER. This is because it stores a flag
    // to say whether it should be freed when destroyed.
    // This causes a warning under -flto about freeing a stack object
    // even though the bson_t is set for static.
    bson_destroy(&bson);
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
