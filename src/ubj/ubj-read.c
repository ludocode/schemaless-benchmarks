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
#include "ubj.h"

static char* file_data;
static size_t file_size;

// A ubjr_array_t or ubjr_object_t contains an array of its values
// stored as a void*. If the array is not of a fixed type, the void*
// is a ubjr_dynamic_t*, since each element in the array stores its
// own type. Otherwise the void pointer is an array of specific
// types (such as int64_t, double, char*, ubjr_array_t, etc.)
//
// In order to handle this, hash_value() takes a void pointer
// and switches on the type to figure out what the void* contains
// (which is very similar to what ubj does internally.) It has a
// special case for UBJ_MIXED when reading values of a mixed
// array or object that figures out the type, offsets to the
// content of the dynamic and recurses into itself to hash it.
// This way an array or object reader can simply pass its own
// fixed type to hash_value(), and if it's mixed, it will sort
// out the type for each element.
//
// This seems like a very unnatural way to access the data, but that's
// pretty much what the library does, and I don't know how to make it
// better.

static void hash_value(void* data, UBJ_TYPE type, size_t index, uint32_t* hash) {
    switch (type) {
        case UBJ_MIXED: {
            ubjr_dynamic_t* dynamic = &((ubjr_dynamic_t*)data)[index];
            switch (dynamic->type) {
                case UBJ_INT8:
                case UBJ_UINT8:
                case UBJ_INT16:
                case UBJ_INT32:
                case UBJ_INT64:
                    // the union in a dynamic doesn't actually contain these types, so
                    // we can't just cast it and recurse like the others here. if an
                    // integer is read in a dynamic, the int64_t integer is set to the
                    // value read in priv_ubjr_pointer_to_dynamic(), but integers can
                    // be smaller sizes in fixed-type arrays and objects.
                    *hash = hash_i64(*hash, dynamic->integer);
                    return;

                case UBJ_FLOAT64: hash_value(&dynamic->real, dynamic->type, 0, hash); return;
                case UBJ_STRING:  hash_value(&dynamic->string, dynamic->type, 0, hash); return;
                case UBJ_ARRAY:   hash_value(&dynamic->container_array, dynamic->type, 0, hash); return;
                case UBJ_OBJECT:  hash_value(&dynamic->container_object, dynamic->type, 0, hash); return;

                default: break;
            }
            hash_value(NULL, dynamic->type, 0, hash);
            return;
        }

        case UBJ_NULLTYPE: *hash = hash_nil(*hash); return;
        case UBJ_BOOL_TRUE: *hash = hash_bool(*hash, true); return;
        case UBJ_BOOL_FALSE: *hash = hash_bool(*hash, false); return;
        case UBJ_FLOAT64: *hash = hash_double(*hash, ((double*)data)[index]); return;

        // we assume that the type here comes from a fixed-size array, not from
        // a dynamic (which should have been handled above.)
        case UBJ_INT8:  *hash = hash_i64(*hash, (( int8_t*)data)[index]); return;
        case UBJ_UINT8: *hash = hash_i64(*hash, ((uint8_t*)data)[index]); return;
        case UBJ_INT16: *hash = hash_i64(*hash, ((int16_t*)data)[index]); return;
        case UBJ_INT32: *hash = hash_i64(*hash, ((int32_t*)data)[index]); return;
        case UBJ_INT64: *hash = hash_i64(*hash, ((int64_t*)data)[index]); return;

        case UBJ_STRING:
        case UBJ_CHAR: {
            ubjr_string_t str = ((ubjr_string_t*)data)[index];
            *hash = hash_str(*hash, str, strlen(str));
            return;
        }

        case UBJ_ARRAY: {
            ubjr_array_t* array = &((ubjr_array_t*)data)[index];
            for (size_t i = 0; i < array->size; ++i)
                hash_value(array->values, array->type, i, hash);
            *hash = hash_u32(*hash, array->size);
            return;
        }

        case UBJ_OBJECT:
        {
            ubjr_object_t* object = &((ubjr_object_t*)data)[index];
            for (size_t i = 0; i < object->size; ++i) {
                *hash = hash_str(*hash, object->keys[i], strlen(object->keys[i]));
                hash_value(object->values, object->type, i, hash);
            }
            *hash = hash_u32(*hash, object->size);
            return;
        }

        default:
            break;
    }
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    ubjr_context_t* src = ubjr_open_memory((uint8_t*)data, (uint8_t*)(data + file_size));
    ubjr_dynamic_t dynamic = ubjr_read_dynamic(src);
    hash_value(&dynamic, UBJ_MIXED, 0, hash_out);
    ubjr_cleanup_dynamic(&dynamic);

    // ubjr has a bug where it leaks the userdata. it never calls the
    // close_cb, but even if it did, free() in memclose() is
    // commented out. this ugly hack frees it manually.
    free(((void**)src)[4]);

    ubjr_close_context(src);
    benchmark_in_situ_free(data);

    // ubjr_open_memory() sets NULL as the error handling callback.
    // I don't know how it is meant to handle errors, and it seems to
    // have lots of bugs (for example priv_ubjw_read_integer() returns
    // 0 on error, but 0 is also a valid integer.) The return value
    // of memread() is also never checked so if the data is truncated
    // it just continues using uninitialized memory, which seems to
    // easily throw it into an infinite loop or infinite recursion.
    //
    // For now we'll have to skip error checking, but this seems like
    // an unfair advantage for ubj. Data validation is fundamental
    // to security and it affects performance.
    return true;
}

bool setup_test(size_t object_size) {
    const char* config =
            #if BENCHMARK_UBJ_OPTIMIZED
            NULL
            #else
            "-unopt"
            #endif
            ;
    file_data = load_data_file_ex(BENCHMARK_FORMAT_UBJSON, object_size, &file_size, config);
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
    return BENCHMARK_UBJ_VERSION;
}

const char* test_language(void) {
    return BENCHMARK_LANGUAGE_C;
}

const char* test_format(void) {
    return "UBJSON";
}

const char* test_filename(void) {
    return __FILE__;
}
