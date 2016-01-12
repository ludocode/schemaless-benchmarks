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

static object_t* root_object;

static bool pack_object(msgpack_packer* packer, object_t* object) {
    switch (object->type) {
        case type_bool:
            return (object->b ? msgpack_pack_true(packer) : msgpack_pack_false(packer)) == 0;

        case type_nil:    return msgpack_pack_nil(packer) == 0;
        case type_int:    return msgpack_pack_long_long(packer, object->i) == 0;
        case type_uint:   return msgpack_pack_unsigned_long_long(packer, object->u) == 0;
        case type_double: return msgpack_pack_double(packer, object->d) == 0;

        case type_str:
            if (msgpack_pack_str(packer, object->l) != 0)
                return false;
            return msgpack_pack_str_body(packer, object->str, object->l) == 0;

        case type_array:
            if (msgpack_pack_array(packer, object->l) != 0)
                return false;
            for (size_t i = 0; i < object->l; ++i)
                if (!pack_object(packer, object->children + i))
                    return false;
            return true;

        case type_map:
            if (msgpack_pack_map(packer, object->l) != 0)
                return false;
            for (size_t i = 0; i < object->l; ++i) {

                // we expect keys to be short strings
                object_t* key = object->children + i * 2;
                assert(key->type == type_str);
                if (msgpack_pack_str(packer, key->l) != 0)
                    return false;
                if (msgpack_pack_str_body(packer, key->str, key->l) != 0)
                    return false;

                if (!pack_object(packer, object->children + i * 2 + 1))
                    return false;
            }
            return true;

        default:
            assert(0);
            break;
    }
    return false;
}

bool run_test(uint32_t* hash_out) {
    msgpack_sbuffer buffer;
    msgpack_sbuffer_init(&buffer);
    msgpack_packer packer;
    msgpack_packer_init(&packer, &buffer, msgpack_sbuffer_write);

    if (!pack_object(&packer, root_object))
        return false;

    *hash_out = hash_str(*hash_out, buffer.data, buffer.size);
    msgpack_sbuffer_destroy(&buffer);
    return true;
}

bool setup_test(size_t object_size) {
    root_object = benchmark_object_create(object_size);
    return true;
}

void teardown_test(void) {
    object_destroy(root_object);
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
