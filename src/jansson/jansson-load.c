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

// Jansson does not support iterating over objects in their original
// order. We implement a bunch of hacks that access the internals in
// order to iterate in order so we can check that the hash values
// actually work out. This is not on by default obviously.

#if PRESERVE_ORDER
#include "jansson_private.h"

struct entry {
    size_t serial;
    const char* key;
    json_t* child;
};

int compare_entry(const void* left, const void* right) {
    size_t ls = ((const struct entry*)left)->serial;
    size_t rs = ((const struct entry*)right)->serial;
    return (ls < rs) ? -1 : (ls > rs) ? 1 : 0;
}
#endif

static char* file_data;
static size_t file_size;

static bool hash_json(json_t* json, uint32_t* hash) {
    switch (json_typeof(json)) {
        case JSON_NULL:    *hash = hash_nil(*hash); return true;
        case JSON_TRUE:    *hash = hash_bool(*hash, true); return true;
        case JSON_FALSE:   *hash = hash_bool(*hash, false); return true;
        case JSON_REAL:    *hash = hash_double(*hash, json_real_value(json)); return true;
        case JSON_STRING:  *hash = hash_str(*hash, json_string_value(json), json_string_length(json)); return true;

        case JSON_INTEGER:
            // Jansson does not support JSON big integers at all. Unlike
            // YAJL, it does not even allow us to parse them ourselves by
            // giving us the original string. If you don't set a flag to
            // convert ints as reals, they will cause a parse error.
            *hash = hash_i64(*hash, json_integer_value(json));
            return true;

        case JSON_ARRAY: {
            size_t index;
            json_t* child;
            json_array_foreach(json, index, child)
                if (!hash_json(child, hash))
                    return false;
            *hash = hash_u32(*hash, json_array_size(json));
            return true;
        }

        case JSON_OBJECT: {
            #if PRESERVE_ORDER
            size_t count = json_object_size(json);
            struct entry* entries = malloc(sizeof(struct entry) * count);

            size_t i = 0;
            const char* key;
            json_t* child;
            json_object_foreach(json, key, child) {
                entries[i].serial = hashtable_iter_serial(json_object_key_to_iter(key));
                entries[i].key = key;
                entries[i].child = child;
                ++i;
            }

            qsort(entries, count, sizeof(struct entry), &compare_entry);

            for (i = 0; i < count; ++i) {
                *hash = hash_str(*hash, entries[i].key, strlen(entries[i].key));
                if (!hash_json(entries[i].child, hash))
                    return false;
            }

            free(entries);
            #else
            const char* key;
            json_t* child;
            json_object_foreach(json, key, child) {
                *hash = hash_str(*hash, key, strlen(key));
                hash_json(child, hash);
            }
            #endif

            *hash = hash_u32(*hash, json_object_size(json));
            return true;
        }

        default:
            break;
    }

    return true;
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    // Jansson has a flag to reject duplicate keys, but for some reason it's
    // not on by default, even though it puts everything in hashtables
    // and re-orders everything and does not even support iterating objects
    // in their original order. How are you supposed to deal with duplicates
    // if it re-orders them?? Whatever, we'll just let it do its thing.
    // (We don't require libraries in this benchmark to check for duplicates
    // anyway.)
    int flags = 0;

    json_error_t error;
    json_t* root = json_loadb(data, file_size, flags, &error);
    if (!root) {
        benchmark_in_situ_free(data);
        return false;
    }

    bool ok = hash_json(root, hash_out);
    json_decref(root);
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

