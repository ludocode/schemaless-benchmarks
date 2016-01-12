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
#include "mongo/bson/bson.h"
#include "mongo/version.h"

static object_t* root_object;

static void append_object(mongo::BSONObjBuilder& builder, object_t* object);
static void append_array(mongo::BSONArrayBuilder& builder, object_t* object);

// The builders have a function appendNumber() to write the shortest
// representation, but for some bizarre reason they convert ints
// in the 30-40 bit range to doubles

static void append_object_int(mongo::BSONObjBuilder& builder, mongo::StringData& keystr, long long value) {
    if (value >= INT32_MIN && value <= INT32_MAX)
        builder.append(keystr, (int)value);
    else
        builder.append(keystr, value);
}

static void append_array_int(mongo::BSONArrayBuilder& builder, long long value) {
    if (value >= INT32_MIN && value <= INT32_MAX)
        builder.append((int)value);
    else
        builder.append(value);
}

static void append_object(mongo::BSONObjBuilder& builder, object_t* object) {
    for (size_t i = 0; i < object->l; ++i) {
        object_t* key = object->children + i * 2;
        object_t* value = object->children + i * 2 + 1;
        mongo::StringData keystr(key->str, key->l);

        switch (value->type) {
            case type_nil:    builder.appendNull(keystr);                                      break;
            case type_bool:   builder.appendBool(keystr, value->b);                            break;
            case type_double: builder.append(keystr, value->d);                                break;
            case type_str:    builder.append(keystr, mongo::StringData(value->str, value->l)); break;

            case type_int:    append_object_int(builder, keystr, (long long)value->i);         break;
            case type_uint:   append_object_int(builder, keystr, (long long)value->u);         break;

            case type_array: {
                mongo::BSONArrayBuilder sub(builder.subarrayStart(keystr));
                append_array(sub, value);
                sub.done();
                break;
            }

            case type_map: {
                mongo::BSONObjBuilder sub(builder.subobjStart(keystr));
                append_object(sub, value);
                sub.done();
                break;
            }

            default:
                throw std::exception();
        }
    }
}

static void append_array(mongo::BSONArrayBuilder& builder, object_t* object) {
    for (size_t i = 0; i < object->l; ++i) {
        object_t* value = object->children + i;

        switch (value->type) {
            case type_nil:    builder.appendNull();                                    break;
            case type_bool:   builder.appendBool(value->b);                            break;
            case type_double: builder.append(value->d);                                break;
            case type_str:    builder.append(mongo::StringData(value->str, value->l)); break;

            case type_int:    append_array_int(builder, (long long)value->i);          break;
            case type_uint:   append_array_int(builder, (long long)value->u);          break;

            case type_array: {
                mongo::BSONArrayBuilder sub(builder.subarrayStart());
                append_array(sub, value);
                sub.done();
                break;
            }

            case type_map: {
                mongo::BSONObjBuilder sub(builder.subobjStart());
                append_object(sub, value);
                sub.done();
                break;
            }

            default:
                throw std::exception();
        }
    }
}

bool run_test(uint32_t* hash_out) {
    try {
        mongo::BSONObj obj;
        if (root_object->type == type_map) {
            mongo::BSONObjBuilder builder;
            append_object(builder, root_object);
            obj = builder.obj();
        } else {
            mongo::BSONArrayBuilder builder;
            append_array(builder, root_object);
            obj = builder.obj();
        }

        *hash_out = hash_str(*hash_out, obj.objdata(), obj.objsize());

    } catch (std::exception e) {
        return false;
    }
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
    return mongo::client::kVersionString;
}

const char* test_language(void) {
    return BENCHMARK_LANGUAGE_CXX;
}

const char* test_format(void) {
    return "BSON";
}

const char* test_filename(void) {
    return __FILE__;
}
