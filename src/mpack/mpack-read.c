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

static void hash_element(mpack_reader_t* reader, uint32_t* hash) {
    const mpack_tag_t tag = mpack_read_tag(reader);

    switch (tag.type) {
        case mpack_type_nil: *hash = hash_nil(*hash); return;
        case mpack_type_bool: *hash = hash_bool(*hash, tag.v.b); return;
        case mpack_type_int: *hash = hash_i64(*hash, tag.v.i); return;
        case mpack_type_uint: *hash = hash_u64(*hash, tag.v.u); return;

        // note: all reals are hashed as doubles (not all libraries read different sized types)
        case mpack_type_double: *hash = hash_double(*hash, tag.v.d); return;
        case mpack_type_float: *hash = hash_float(*hash, tag.v.f); return;

        case mpack_type_str: {
            const char* str =
                    #if CHECK_UTF8
                    mpack_read_utf8_inplace(reader, tag.v.l);
                    #else
                    mpack_read_bytes_inplace(reader, tag.v.l);
                    #endif
            if (mpack_reader_error(reader) != mpack_ok)
                return;
            *hash = hash_str(*hash, str, tag.v.l);
            mpack_done_str(reader);
            return;
        }

        case mpack_type_array:
            for (size_t i = 0; i < tag.v.n; ++i) {
                hash_element(reader, hash);
                if (mpack_reader_error(reader) != mpack_ok)
                    return;
            }
            *hash = hash_u32(*hash, tag.v.n);
            mpack_done_array(reader);
            return;

        case mpack_type_map:
            for (size_t i = 0; i < tag.v.n; ++i) {

                // we expect keys to be short strings
                uint32_t len = mpack_expect_str(reader);
                const char* str =
                        #if CHECK_UTF8
                        mpack_read_utf8_inplace(reader, len);
                        #else
                        mpack_read_bytes_inplace(reader, len);
                        #endif
                if (mpack_reader_error(reader) != mpack_ok)
                    return;
                *hash = hash_str(*hash, str, len);
                mpack_done_str(reader);

                hash_element(reader, hash);
            }
            *hash = hash_u32(*hash, tag.v.n);
            mpack_done_map(reader);
            return;

        default:
            mpack_reader_flag_error(reader, mpack_error_data);
            break;
    }
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    mpack_reader_t reader;
    mpack_reader_init_data(&reader, data, file_size);

    hash_element(&reader, hash_out);

    mpack_error_t error = mpack_reader_destroy(&reader);
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
