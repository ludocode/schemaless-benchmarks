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
#include "msgpack.h"

static char* file_data;
static size_t file_size;

static bool hash_object(msgpack_object* object, uint32_t* hash) {
    switch (object->type) {
        case MSGPACK_OBJECT_NIL:              *hash = hash_nil(*hash); return true;
        case MSGPACK_OBJECT_BOOLEAN:          *hash = hash_bool(*hash, object->via.boolean); return true;
        case MSGPACK_OBJECT_FLOAT:            *hash = hash_double(*hash, object->via.f64); return true;
        case MSGPACK_OBJECT_NEGATIVE_INTEGER: *hash = hash_i64(*hash, object->via.i64); return true;
        case MSGPACK_OBJECT_POSITIVE_INTEGER: *hash = hash_u64(*hash, object->via.u64); return true;
        case MSGPACK_OBJECT_STR:              *hash = hash_str(*hash, object->via.str.ptr, object->via.str.size); return true;

        case MSGPACK_OBJECT_ARRAY:
            for (size_t i = 0; i < object->via.array.size; ++i)
                if (!hash_object(object->via.array.ptr + i, hash))
                    return false;
            *hash = hash_u32(*hash, object->via.array.size);
            return true;

        case MSGPACK_OBJECT_MAP:
            for (size_t i = 0; i < object->via.map.size; ++i) {

                // we expect keys to be short strings
                msgpack_object* key = &object->via.map.ptr[i].key;
                assert(key->type == MSGPACK_OBJECT_STR);
                *hash = hash_str(*hash, key->via.str.ptr, key->via.str.size);

                if (!hash_object(&object->via.map.ptr[i].val, hash))
                    return false;
            }
            *hash = hash_u32(*hash, object->via.array.size);
            return true;

        default:
            break;
    }

    return false;
}

bool run_test(uint32_t* hash_out) {
    char* data = benchmark_in_situ_copy(file_data, file_size);
    if (!data)
        return false;

    // The very first example code in the msgpack-c README shows how to
    // use msgpack_unpack(), but it's apparently obsolete.
    //
    // Instead, the quickstart guide shows using a msgpack_unpacked with
    // msgpack_unpacked_next(). The quickstart guide claims it returns
    // bool, but it actually returns an enum. That example also doesn't
    // destroy the msgpack_unpacked, so it leaks its zone. We fix those
    // problems here.

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_return ret = msgpack_unpack_next(&msg, data, file_size, NULL);

    if (ret != MSGPACK_UNPACK_SUCCESS) {
        benchmark_in_situ_free(data);
        return false;
    }

    bool ok = hash_object(&msg.data, hash_out);
    msgpack_unpacked_destroy(&msg);
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
    return MSGPACK_VERSION;
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
