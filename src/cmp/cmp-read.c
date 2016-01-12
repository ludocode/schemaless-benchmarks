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
#include "cmp.h"

static char* file_data;
static size_t file_size;



// cmp doesn't have built-in support for reading from a memory
// buffer, so we implement one here. this means that cmp needs
// to memcpy all data before reading it!

typedef struct buffer_t {
    const char* data;
    size_t left;
} buffer_t;

static bool buffer_cmp_reader(cmp_ctx_t* ctx, void* data, size_t count) {
    buffer_t* buffer = (buffer_t*)ctx->buf;
    if (count > buffer->left)
        return false;
    memcpy(data, buffer->data, count);
    buffer->data += count;
    buffer->left -= count;
    return true;
}



static bool hash_element(cmp_ctx_t* cmp, uint32_t* hash) {
    buffer_t* buffer = (buffer_t*)cmp->buf;

    cmp_object_t object;
    if (!cmp_read_object(cmp, &object))
        return false;

    // note: we fetch values out of the cmp_object_t directly rather
    // than going through the cmp_object_is/as* functions. it's much
    // faster this way.

    switch (object.type) {
        case CMP_TYPE_NIL: *hash = hash_nil(*hash); return true;
        case CMP_TYPE_BOOLEAN: *hash = hash_bool(*hash, object.as.boolean); return true;
        case CMP_TYPE_DOUBLE: *hash = hash_double(*hash, object.as.dbl); return true;

        // note: all ints are hashed as 64-bit (not all libraries read different sized types)

        case CMP_TYPE_POSITIVE_FIXNUM: *hash = hash_u64(*hash, object.as.u8); return true;
        case CMP_TYPE_UINT8: *hash = hash_u64(*hash, object.as.u8); return true;
        case CMP_TYPE_UINT16: *hash = hash_u64(*hash, object.as.u16); return true;
        case CMP_TYPE_UINT32: *hash = hash_u64(*hash, object.as.u32); return true;
        case CMP_TYPE_UINT64: *hash = hash_u64(*hash, object.as.u64); return true;

        case CMP_TYPE_NEGATIVE_FIXNUM: *hash = hash_i64(*hash, object.as.s8); return true;
        case CMP_TYPE_SINT8: *hash = hash_i64(*hash, object.as.s8); return true;
        case CMP_TYPE_SINT16: *hash = hash_i64(*hash, object.as.s16); return true;
        case CMP_TYPE_SINT32: *hash = hash_i64(*hash, object.as.s32); return true;
        case CMP_TYPE_SINT64: *hash = hash_i64(*hash, object.as.s64); return true;

        case CMP_TYPE_FIXSTR:
        case CMP_TYPE_STR8:
        case CMP_TYPE_STR16:
        case CMP_TYPE_STR32:
        {
            uint32_t len = object.as.str_size;
            if (buffer->left < len)
                return false;
            *hash = hash_str(*hash, buffer->data, len);
            buffer->data += len;
            buffer->left -= len;
            return true;
        }

        case CMP_TYPE_FIXARRAY:
        case CMP_TYPE_ARRAY16:
        case CMP_TYPE_ARRAY32:
            for (size_t i = 0; i < object.as.array_size; ++i){
                if (!hash_element(cmp, hash))
                    return false;
            }
            *hash = hash_u32(*hash, object.as.array_size);
            return true;

        case CMP_TYPE_FIXMAP:
        case CMP_TYPE_MAP16:
        case CMP_TYPE_MAP32:
        {
            for (size_t i = 0; i < object.as.map_size; ++i) {

                // we expect keys to be short strings
                char buf[16];
                uint32_t size = sizeof(buf);
                if (!cmp_read_str(cmp, buf, &size))
                    return false;
                *hash = hash_str(*hash, buf, size);

                if (!hash_element(cmp, hash))
                    return false;
            }
            *hash = hash_u32(*hash, object.as.map_size);
            return true;
        }

        default:
            break;
    }

    return false;
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    buffer_t buffer;
    buffer.data = data;
    buffer.left = file_size;

    cmp_ctx_t cmp;
    cmp_init(&cmp, &buffer, buffer_cmp_reader, NULL);

    bool ok = hash_element(&cmp, hash_out);
    benchmark_in_situ_free(data);
    return ok;
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
    snprintf(buf, sizeof(buf), "v%u", cmp_version());
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
