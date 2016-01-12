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

// the "natural" way to use binn (and probably the fastest) is
// to write keys and typed values together, which is why the value
// switch is implemented twice: one to append values to a list, and
// one to set values along with their key on an object.

// binn is in C89 and does not use const, so we have to cast it
// away. binn also uses null-terminated strings throughout.
#define CONST_CAST(s) ((char*)(s))

static bool write_list(binn* parent, object_t* object);
static bool write_object(binn* parent, object_t* object);

static bool write_list(binn* parent, object_t* object) {
    for (size_t i = 0; i < object->l; ++i) {
        object_t* value = object->children + i;
        switch (value->type) {
            case type_nil:     if (!binn_list_add_null(parent))                        return false;  break;
            case type_bool:    if (!binn_list_add_bool(parent, (BOOL)value->b))        return false;  break;
            case type_double:  if (!binn_list_add_double(parent, value->d))            return false;  break;
            case type_int:     if (!binn_list_add_int64(parent, value->i))             return false;  break;
            case type_uint:    if (!binn_list_add_uint64(parent, value->u))            return false;  break;
            case type_str:     if (!binn_list_add_str(parent, CONST_CAST(value->str))) return false;  break;

            case type_array: {
                binn child;
                binn_create_list(&child);
                bool ok = write_list(&child, value);
                if (ok)
                    ok = binn_list_add_object(parent, &child);
                binn_free(&child);
                if (!ok)
                    return false;
            } break;

            case type_map: {
                binn child;
                binn_create_object(&child);
                bool ok = write_object(&child, value);
                if (ok)
                    ok = binn_list_add_object(parent, &child);
                binn_free(&child);
                if (!ok)
                    return false;
            } break;

            default:
                return false;
        }
    }
    return true;
}

static bool write_object(binn* parent, object_t* object) {
    for (size_t i = 0; i < object->l; ++i) {
        char* key = CONST_CAST(object->children[i * 2].str);
        object_t* value = object->children + i * 2 + 1;
        switch (value->type) {
            case type_nil:     if (!binn_object_set_null(parent, key))                        return false;  break;
            case type_bool:    if (!binn_object_set_bool(parent, key, (BOOL)value->b))        return false;  break;
            case type_double:  if (!binn_object_set_double(parent, key, value->d))            return false;  break;
            case type_int:     if (!binn_object_set_int64(parent, key, value->i))             return false;  break;
            case type_uint:    if (!binn_object_set_uint64(parent, key, value->u))            return false;  break;
            case type_str:     if (!binn_object_set_str(parent, key, CONST_CAST(value->str))) return false;  break;

            case type_array: {
                binn child;
                binn_create_list(&child);
                bool ok = write_list(&child, value);
                if (ok)
                    ok = binn_object_set_object(parent, key, &child);
                binn_free(&child);
                if (!ok)
                    return false;
            } break;

            case type_map: {
                binn child;
                binn_create_object(&child);
                bool ok = write_object(&child, value);
                if (ok)
                    ok = binn_object_set_object(parent, key, &child);
                binn_free(&child);
                if (!ok)
                    return false;
            } break;

            default:
                return false;
        }
    }
    return true;
}

bool setup_test(size_t object_size) {
    object_t* object = benchmark_object_create(object_size);
    binn root;
    bool ok;
    if (object->type == type_map) {
        binn_create_object(&root);
        ok = write_object(&root, object);
    } else {
        binn_create_list(&root);
        ok = write_list(&root, object);
    }
    object_destroy(object);

    if (!ok) {
        fprintf(stderr, "binn error writing data!\n");
        binn_free(&root);
        return false;
    }

    char filename[64];
    benchmark_filename(filename, sizeof(filename), object_size, BENCHMARK_FORMAT_BINN, NULL);
    FILE* file = fopen(filename, "wb");
    fwrite(binn_ptr(&root), binn_size(&root), 1, file);
    fclose(file);

    // like libbson, we call binn_free() regardless of whether
    // we placed it on the stack or the heap; it stores a flag
    // to say whether it should be freed. this apparently doesn't
    // cause warnings since GCC chooses not to inline binn_free().
    binn_free(&root);
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
