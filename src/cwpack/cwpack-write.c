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
#include "basic_contexts.h"

// dynamic_memory_pack_context doesn't recommend a default buffer size. We use
// the size of a normal memory page assuming it gives the best performance (it
// seems to other libraries.)
#define INITIAL_BUFFER_SIZE 4096

static object_t* root_object;

static void write_object(cw_pack_context* writer, object_t* object) {
    switch (object->type) {
        case type_nil:    cw_pack_nil     (writer);                         break;
        case type_bool:   cw_pack_boolean (writer, object->b);              break;
        case type_double: cw_pack_double  (writer, object->d);              break;
        case type_int:    cw_pack_signed  (writer, object->i);              break;
        case type_uint:   cw_pack_unsigned(writer, object->u);              break;
        case type_str:    cw_pack_str     (writer, object->str, object->l); break;

        case type_array:
            cw_pack_array_size(writer, object->l);
            for (size_t i = 0; i < object->l; ++i)
                write_object(writer, object->children + i);
            break;

        case type_map:
            cw_pack_map_size(writer, object->l);

            for (size_t i = 0; i < object->l; ++i) {

                // we expect keys to be short strings
                object_t* key = object->children + i * 2;
                assert(key->type == type_str);
                cw_pack_str(writer, key->str, key->l);

                write_object(writer, object->children + i * 2 + 1);
            }
            break;

        default:
            assert(0);
            break;
    }
}

bool run_test(uint32_t* hash_out) {
    dynamic_memory_pack_context dmpc;
    init_dynamic_memory_pack_context(&dmpc, INITIAL_BUFFER_SIZE);

    write_object(&dmpc.pc, root_object);

    int error = dmpc.pc.err_no;
    if (error != CWP_RC_OK) {
        free_dynamic_memory_pack_context(&dmpc);
        return false;
    }

    *hash_out = hash_str(*hash_out, (const char*)dmpc.pc.start, dmpc.pc.current - dmpc.pc.start);
    free_dynamic_memory_pack_context(&dmpc);
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
