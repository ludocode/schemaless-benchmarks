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
#include "json-builder.h"

static object_t* root_object;

static json_value* create_value(object_t* object) {
    switch (object->type) {
        case type_bool:   return json_boolean_new(object->b ? 1 : 0);
        case type_nil:    return json_null_new();
        case type_int:    return json_integer_new(object->i);
        case type_double:
            // note: doubles are what causes json-builder to not match the hash
            // output value of the other JSON encoders. it uses sprintf %g to
            // print doubles which changes the output precision.
            return json_double_new(object->d);

        case type_uint:
            // Note: The generator limits unsigned int to INT64_MAX, so we won't
            // ever hit this error, but we check anyway (because you would need
            // to check in your own code if you used u64.) json-builder doesn't
            // allow integers outside the range of signed int64.
            if (object->u > (uint64_t)INT64_MAX)
                return false;
            return json_integer_new((int64_t)object->u);

        case type_str:
            // we can't use nocopy because it takes ownership of the string.
            return json_string_new_length(object->l, object->str);

        case type_array: {
            json_value* parent = json_array_new(object->l);
            if (!parent)
                return NULL;
            for (size_t i = 0; i < object->l; ++i) {
                json_value* child = create_value(object->children + i);
                if (!child || !json_array_push(parent, child)) {
                    json_builder_free(parent);
                    return NULL;
                }
            }
            return parent;
        }

        case type_map: {
            json_value* parent = json_object_new(object->l);
            if (!parent)
                return NULL;
            for (size_t i = 0; i < object->l; ++i) {
                // we expect keys to be short strings. we can't use nocopy
                // because it takes ownership of the string.
                object_t* key = object->children + i * 2;
                json_value* child = create_value(object->children + i * 2 + 1);
                if (!child || !json_object_push_length(parent, key->l, key->str, child)) {
                    json_builder_free(parent);
                    return NULL;
                }
            }
            return parent;
        }

        default:
            break;
    }
    return false;
}

bool run_test(uint32_t* hash_out) {
    json_value* value = create_value(root_object);
    if (!value)
        return false;

    json_serialize_opts opts = {json_serialize_mode_packed, 0, 0};
    size_t len = json_measure_ex(value, opts);
    json_char* buf = (json_char*)malloc(len);
    if (!buf) {
        json_builder_free(value);
        return false;
    }
    json_serialize_ex(buf, value, opts);

    *hash_out = hash_str(*hash_out, (const char*)buf, len);
    free(buf);
    json_builder_free(value);
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
    return BENCHMARK_JSON_BUILDER_VERSION;
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

