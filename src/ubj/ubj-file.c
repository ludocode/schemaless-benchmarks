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

static UBJ_TYPE get_ubj_type(object_t* object) {
    switch (object->type) {
        case type_nil: return UBJ_NULLTYPE;
        case type_bool: return object->b ? UBJ_BOOL_TRUE : UBJ_BOOL_FALSE;
        case type_str: return UBJ_STRING;
        case type_double: return UBJ_FLOAT64;

        // Note that ubjw_min_integer_type() has a bug where it incorrectly
        // excludes the minimum bound of each signed integer type. For example
        // ubjw_min_integer_type(INT8_MIN) returns UBJ_INT16, not UBJ_INT8.
        // This is contrary to the spec which allows -128 in an int8.
        case type_int: return ubjw_min_integer_type(object->i);
        case type_uint: return ubjw_min_integer_type(object->u);

        case type_array: return UBJ_ARRAY;
        case type_map: return UBJ_OBJECT;
        default: break;
    }
    return UBJ_MIXED;
}

static bool write_object(ubjw_context_t* dst, object_t* object, bool sized) {
    switch (object->type) {
        case type_nil:    ubjw_write_null   (dst);            break;
        case type_bool:   ubjw_write_bool   (dst, object->b); break;
        case type_double: ubjw_write_float64(dst, object->d); break;
        case type_int:    ubjw_write_integer(dst, object->i); break;

        case type_str:
            ubjw_write_string(dst, object->str);
            break;

        case type_uint:
            // UBJSON does not support unsigned 64-bit numbers (unless
            // they are written as high precision big integers, represented
            // by a decimal string.)
            if (object->u > INT64_MAX)
                return false;
            ubjw_write_integer(dst, (int64_t)object->u);
            break;

        case type_array: {
            UBJ_TYPE type = UBJ_MIXED;

            // in order to try to take advantage as much as possible of ubjson's
            // space-saving optimization, we check if each element of the array
            // is the same type. (we need all integers to be the same width to
            // use an int type, otherwise it would be a waste of space.)
            if (sized && object->l >= 2) {
                type = get_ubj_type(&object->children[0]);
                if (type != UBJ_MIXED) {
                    for (size_t i = 1; i < object->l; ++i) {
                        if (get_ubj_type(&object->children[i]) != type) {
                            type = UBJ_MIXED;
                            break;
                        }
                    }
                }
            }

            ubjw_begin_array(dst, type, sized ? object->l : 0);
            for (size_t i = 0; i < object->l; ++i)
                write_object(dst, object->children + i, sized);
            ubjw_end(dst);
        } break;

        case type_map: {
            UBJ_TYPE type = UBJ_MIXED;

            // as above, check if all map values are the same type
            if (sized && object->l >= 2) {
                type = get_ubj_type(&object->children[1]);
                if (type != UBJ_MIXED) {
                    for (size_t i = 1; i < object->l; ++i) {
                        if (get_ubj_type(&object->children[i * 2 + 1]) != type) {
                            type = UBJ_MIXED;
                            break;
                        }
                    }
                }
            }

            ubjw_begin_object(dst, type, sized ? object->l : 0);
            for (size_t i = 0; i < object->l; ++i) {

                // we expect keys to be short strings
                object_t* key = object->children + i * 2;
                assert(key->type == type_str);
                ubjw_write_key(dst, key->str);

                write_object(dst, object->children + i * 2 + 1, sized);
            }
            ubjw_end(dst);
        } break;

        default:
            return false;
    }

    return true;
}

bool write_file(size_t object_size, bool sized) {
    char filename[64];
    benchmark_filename(filename, sizeof(filename), object_size, BENCHMARK_FORMAT_UBJSON, sized ? NULL : "-unopt");

    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "error opening file for writing!\n");
        return false;
    }

    // ubjw_open_file() doesn't register any error handler, and
    // does not check the return value of calls to fwrite() either.
    // it doesn't seem to have any way of handling i/o errors at all,
    // such as disk full errors or broken pipes. this seems like
    // a serious flaw; there's no way to tell if the data was
    // truncated.
    ubjw_context_t* dst = ubjw_open_file(file);

    object_t* object = benchmark_object_create(object_size);
    bool ok = write_object(dst, object, sized);
    object_destroy(object);

    ubjw_close_context(dst);
    return ok;
}

bool setup_test(size_t object_size) {
    if (!write_file(object_size, true))
        return false;
    if (!write_file(object_size, false))
        return false;
    return true;
}

bool run_test(uint32_t* hash_out) {
    return false;
}

void teardown_test(void) {
}

bool is_benchmark(void) {
    return false;
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
