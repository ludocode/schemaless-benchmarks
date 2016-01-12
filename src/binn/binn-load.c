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
#include "binn.h"

// the type terminology can be a bit confusing here because what
// the data calls "map" is actually an "object" in binn since
// it has string keys. a binn "map" has integer keys (and is not
// used in this benchmark.)

static char* file_data;
static size_t file_size;

static bool hash_list(binn* parent, uint32_t* hash);
static bool hash_object(binn* parent, uint32_t* hash);

static bool hash_value(binn* value, uint32_t* hash) {

    switch (binn_type(value)) {

        case BINN_NULL: *hash = hash_nil(*hash); return true;
        case BINN_BOOL: *hash = hash_bool(*hash, value->vbool); return true;
        case BINN_DOUBLE: *hash = hash_double(*hash, value->vdouble); return true;
        case BINN_STRING: {
            // for strings, "size" appears to be zero. we have to scan for the
            // null-terminator. technically we could add a hash function for cstr
            const char* str = (const char*)value->ptr;
            *hash = hash_str(*hash, str, strlen(str));
            return true;
        }

        // the binn "type family" functions are all private in the library.
        // it seems the only way to get dynamically typed data out of binn
        // is to switch on the base type and fetch it from the union.
        // all ints are hashed as 64-bit (not all libraries read different
        // sized types)

        case BINN_UINT8:  *hash = hash_u64(*hash, value->vuint8);  return true;
        case BINN_UINT16: *hash = hash_u64(*hash, value->vuint16); return true;
        case BINN_UINT32: *hash = hash_u64(*hash, value->vuint32); return true;
        case BINN_UINT64: *hash = hash_u64(*hash, value->vuint64); return true;

        case BINN_INT8:  *hash = hash_i64(*hash, value->vint8);  return true;
        case BINN_INT16: *hash = hash_i64(*hash, value->vint16); return true;
        case BINN_INT32: *hash = hash_i64(*hash, value->vint32); return true;
        case BINN_INT64: *hash = hash_i64(*hash, value->vint64); return true;

        case BINN_LIST: return hash_list(value, hash);
        case BINN_OBJECT: return hash_object(value, hash);

        default:
            break;
    }
    return false;
}

static bool hash_list(binn* parent, uint32_t* hash) {
    binn_iter iter;
    binn child;

    // the foreach helper macros don't actually do any error
    // checking, so we do it ourselves. the "next" functions
    // return false on both error and end of data, so have to
    // track the count ourselves to make sure there was no error.
    uint32_t count = binn_count(parent);
    uint32_t i = 0;
    if (!binn_iter_init(&iter, parent, BINN_LIST))
        return false;
    while (binn_list_next(&iter, &child)) {
        ++i;
        if (!hash_value(&child, hash))
            return false;
    }

    if (i != count)
        return false;
    *hash = hash_u32(*hash, count);
    return true;
}

static bool hash_object(binn* parent, uint32_t* hash) {
    binn_iter iter;
    char key[256]; // binn keys have a length limit of 255 characters plus null-terminator.
    binn child;

    // as above, we have to do our own error checking.
    uint32_t count = binn_count(parent);
    uint32_t i = 0;
    if (!binn_iter_init(&iter, parent, BINN_OBJECT))
        return false;
    while (binn_object_next(&iter, key, &child)) {
        ++i;
        *hash = hash_str(*hash, key, strlen(key));
        if (!hash_value(&child, hash))
            return false;
    }

    if (i != count)
        return false;
    *hash = hash_u32(*hash, count);
    return true;
}

bool run_test(uint32_t* hash) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    // so this is terrifying. we can't actually give binn the size
    // of our data. if it's been truncated, binn will just keep on
    // reading uninitialized memory. maybe i'm misunderstanding how
    // to use this and calling the wrong function? otherwise this
    // should be considered a critical security vulnerability.
    binn root;
    binn_load(data, &root);

    bool ok;
    if (binn_type(&root) == BINN_LIST)
        ok = hash_list(&root, hash);
    else
        ok = hash_object(&root, hash);

    benchmark_in_situ_free(data);
    return ok;
}

bool setup_test(size_t object_size) {
    file_data = load_data_file(BENCHMARK_FORMAT_BINN, object_size, &file_size);
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
    return BENCHMARK_BINN_VERSION;
}

const char* test_language(void) {
    return BENCHMARK_LANGUAGE_C;
}

const char* test_format(void) {
    return "Binn";
}

const char* test_filename(void) {
    return __FILE__;
}
