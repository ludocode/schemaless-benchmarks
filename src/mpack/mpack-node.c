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

static char* file_data;
static size_t file_size;

static void hash_node(mpack_node_t node, uint32_t* hash) {
    switch (mpack_node_type(node)) {
        case mpack_type_nil:    *hash = hash_nil(*hash); return;
        case mpack_type_bool:   *hash = hash_bool(*hash, mpack_node_bool(node)); return;
        case mpack_type_double: *hash = hash_double(*hash, mpack_node_double(node)); return;
        case mpack_type_int:    *hash = hash_i64(*hash, mpack_node_i64(node)); return;
        case mpack_type_uint:   *hash = hash_u64(*hash, mpack_node_u64(node)); return;

        case mpack_type_str:
            #if CHECK_UTF8
            mpack_node_check_utf8(node);
            #endif
            *hash = hash_str(*hash, mpack_node_data(node), mpack_node_data_len(node));
            return;

        case mpack_type_array: {
            uint32_t count = mpack_node_array_length(node);
            for (uint32_t i = 0; i < count; ++i) {
                hash_node(mpack_node_array_at(node, i), hash);
                if (mpack_node_error(node) != mpack_ok)
                    return;
            }
            *hash = hash_u32(*hash, count);
            return;
        }

        case mpack_type_map: {
            uint32_t count = mpack_node_map_count(node);
            for (uint32_t i = 0; i < count; ++i) {

                // we expect keys to be short strings
                mpack_node_t key = mpack_node_map_key_at(node, i);
                #if CHECK_UTF8
                mpack_node_check_utf8(key);
                #endif
                *hash = hash_str(*hash, mpack_node_str(key), mpack_node_strlen(key));

                hash_node(mpack_node_map_value_at(node, i), hash);

                if (mpack_node_error(node) != mpack_ok)
                    return;
            }
            *hash = hash_u32(*hash, count);
            return;
        }

        default:
            mpack_node_flag_error(node, mpack_error_data);
            break;
    }
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    mpack_tree_t tree;
    mpack_tree_init(&tree, data, file_size);
    hash_node(mpack_tree_root(&tree), hash_out);

    mpack_error_t error = mpack_tree_destroy(&tree);
    benchmark_in_situ_free(data);
    return error == mpack_ok;
}

bool setup_test(size_t object_size) {
    file_data = load_data_file(BENCHMARK_FORMAT_MESSAGEPACK, object_size, &file_size);
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
