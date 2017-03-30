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
#include "cwpack.h"

static char* file_data;
static size_t file_size;

static void hash_element(cw_unpack_context* context, uint32_t* hash) {
    cw_unpack_next(context);

    switch (context->item.type) {
        case CWP_ITEM_NIL: *hash = hash_nil(*hash); return;
        case CWP_ITEM_BOOLEAN: *hash = hash_bool(*hash, context->item.as.boolean); return;
        case CWP_ITEM_NEGATIVE_INTEGER: *hash = hash_i64(*hash, context->item.as.i64); return;
        case CWP_ITEM_POSITIVE_INTEGER: *hash = hash_u64(*hash, context->item.as.u64); return;

        // note: all reals are hashed as doubles (not all libraries read different sized types)
        case CWP_ITEM_DOUBLE: *hash = hash_double(*hash, context->item.as.long_real); return;
        case CWP_ITEM_FLOAT: *hash = hash_float(*hash, context->item.as.real); return;

        case CWP_ITEM_STR:
            if (context->err_no != CWP_RC_OK)
                return;
            *hash = hash_str(*hash, (const char*)context->item.as.str.start, context->item.as.str.length);
            return;

        case CWP_ITEM_ARRAY: {
            uint32_t size = context->item.as.array.size;
            for (size_t i = 0; i < size; ++i) {
                hash_element(context, hash);
                if (context->err_no != CWP_RC_OK)
                    return;
            }
            *hash = hash_u32(*hash, size);
            return;
        }

        case CWP_ITEM_MAP: {
            uint32_t size = context->item.as.map.size;
            for (size_t i = 0; i < size; ++i) {

                // we expect keys to be short strings
                cw_unpack_next(context);
                if (context->err_no != CWP_RC_OK)
                    return;
                if (context->item.type != CWP_ITEM_STR) {
                    context->err_no = CWP_RC_MALFORMED_INPUT;
                    return;
                }
                *hash = hash_str(*hash, (const char*)context->item.as.str.start, context->item.as.str.length);

                hash_element(context, hash);
            }
            *hash = hash_u32(*hash, size);
            return;
        }

        default:
            context->err_no = CWP_RC_MALFORMED_INPUT;
            break;
    }
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    cw_unpack_context context;
    cw_unpack_context_init(&context, data, file_size, NULL);

    hash_element(&context, hash_out);

    benchmark_in_situ_free(data);
    return context.err_no == CWP_RC_OK;
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
    static char buf[16];
    snprintf(buf, sizeof(buf), "v%s", BENCHMARK_CWPACK_VERSION);
    return buf;
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
