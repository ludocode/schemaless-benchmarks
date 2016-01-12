# Introduction

This is a benchmarking suite to test the performance of parsers for schemaless data serialization formats. The benchmarking suite currently supports:

[cmp]: https://github.com/camgunz/cmp
[msgpack]: https://github.com/msgpack/msgpack-c
[mpack]: https://github.com/ludocode/mpack
[json-builder-lib]: https://github.com/udp/json-builder
[yajl]: http://lloyd.github.io/yajl/
[binn]: https://github.com/liteserver/binn
[rapidjson]: http://rapidjson.org/
[json-parser-lib]: https://github.com/udp/json-parser
[jansson]: http://www.digip.org/jansson/
[mongo-cxx]: https://github.com/mongodb/mongo-cxx-driver
[ubj]: https://github.com/Steve132/ubj
[libbson]: https://github.com/mongodb/libbson

- JSON
    - [RapidJSON][rapidjson]
    - [YAJL][yajl]
    - [Jansson][jansson]
    - [json-parser][json-parser-lib]/[json-builder][json-builder-lib]
- BSON
    - [libbson][libbson]
    - [MongdoDB C++ Driver][mongo-cxx]
- MessagePack
    - [msgpack C/C++][msgpack]
    - [MPack][mpack]
    - [CMP][cmp]
- UBJSON
    - [ubj][ubj]
- Binn
    - [binn][binn]

Disclaimer: This benchmark is written by the author of MPack. The benchmark may be biased towards MPack and vice versa.

# Benchmarks

[extended-results]: https://github.com/ludocode/schemaless-benchmarks/blob/master/full-results-extended.md

The benchmarks are separated into three test categories: Encoder, Incremental parser, and Tree parser. Not all libraries support all categories, and some tests participate in multiple categories. All categories are purely testing dynamic type serialization. This means there are no values of known or expected type. The data is randomly generated and does not conform to any schema.

Each benchmark is written using the most natural API and error-checking facilities for the given library and test category. For example the encoder tests use a growable data buffer to encode data. (There are arguably a few exceptions to this rule, such as avoiding null-terminated strings where possible.) The resulting data is hashed, and a hash value is output by each benchmark to compare results and to ensure that serialization was performed correctly.

The most recent release version of each library is used if they do regular releases, otherwise the most recent git commit on their default branch is used. All libraries and tests are compiled with GCC 5.3.0 on latest Arch Linux with the following compiler and optimization options. Some libraries may alter or override some of them but we always ensure `-O3` and `-flto` are used. (We do not alter -fstrict-aliasing. The default is enabled in C and C++.)

    -Wall -O3 -flto -fno-fat-lto-objects -DPIC -fPIC -DNDEBUG

Each test is run 7 times, and performs as many iterations as possible in 10 seconds. The best and worst times are dropped, and the resulting speed is the average time per iteration. The speed results tables are sorted by running time for a single iteration (lower is better.)

The executables are stripped to calculate the final size. The size results tables are sorted by compiled code size (lower is better.)

## Error Checking

All benchmark tests perform complete error-checking (where possible) in order to more closely match real-world use. If the library doesn't store an error state or throw an exception, we return the success/fail result from each encoding or hashing step (which may be a boolean flag, or a pointer to an allocated value as in Jansson.) Otherwise we return void.

We pass the intermediate hash always as a pointer argument, except for stateful SAX-style parsers (like rapidjson-sax or yajl-parse) where it is stored as a member variable of the SAX callback object.

In all tests, we check for error at least once in every iteration of all loops in order to early exit (unless the library throws an exception on errors, in which case we also throw exceptions for unhandled types.)

## Test Data

The benchmarking suite includes a random structured data generator. This generates an object containing data of random types and values suitable for conversion to all formats supported by this benchmark. This means it uses a subset of the supported data types of MessagePack, BSON and JSON that are well supported by all libraries:

- The root object is an array or map;
- All map keys are alphanumeric strings;
- Most value strings are ASCII, but some will contain significant non-ASCII characters (in UTF-8) and characters requiring escape sequences in some formats (e.g. `\"` or `\n`);
- There are no binary blobs;
- Integers may be signed or unsigned;
- All integers are in the range `INT64_MIN` to `INT64_MAX` (not `UINT64_MAX` since BSON does not support it and some JSON parsers cannot parse it);
- Real numbers are stored and serialized in double precision, and only typical floats are used (not NaN, infinity, or subnormal floats.)

The generated object ostensibly resembles real-world structured data. Here's an excerpt of the data in pretty-printed JSON:

```JSON
{
    "oyyrnnt": "opl fw pbpx",
    "tgbsxnaiqh": 137,
    "asmngixg": true,
    "qb": -125,
    "xveu": "þùqÏfl Æfvkn rhÇwst gi gçæ ºx0g ÏÈoubk dwt qy iÙbwfÊ amo hÂvpsÒza» jhtza×Î abbyps casvuþÿxe ·m gdhnxlf åjcbva gzyvgp Þkn",
    "pm": 257,
    "flof": "hluikavf ecntokuoh r\nmujnd t",
    "gabevbahfc": null,
    "uawawtzic": "bp tifh uzkk am ",
    "xghv": {
        "ahatnig": 149,
        "gzcbw": {
            "weovoatgqw": false,
            "rniwihefgs": 456
        },
        "bkzd": "hikawjwdv fg vs ckpt qsqw nffkxhd nlbmlkucs fksqbqdf hd pkxsoes st arb xze phcyo ik",
        "aqn": -39.85156250231684,
        "dhpjiz": true
    }
}
```

The benchmarking suite supports arbitrary "sizes" from 1 to 5. The results shown here are for sizes 2 (small) and 4 (large). The generated data has the following sizes in bytes:

| Object Size                       | Small (2) | Large (4) |
|:----------------------------------|----------:|----------:|
| MessagePack                       |      2367 |    248571 |
| Binn                              |      2678 |    266580 |
| BSON                              |      3650 |    337346 |
| UBJSON (size- and type-optimized) |      2800 |    272380 |
| UBJSON (unsized, unoptimized)     |      2644 |    266603 |
| JSON (minified)                   |      3183 |    308908 |
| JSON (pretty, tabs)               |      4562 |    418447 |

Note that the UBJSON unoptimized data is smaller than the optimized data. This is correct, since storing array and object counts takes up additional space.

## Write Test

The write benchmark is a test of how quickly an encoder can encode the above structured data.

The resulting hash value is the hash of the output data, so the hash will be different between encoders of different formats. In addition, different encoders can have different hash results if there are multiple ways to encode the same value for a given data format. For example JSON encoders may not preserve key order or may format real numbers with different precision. Most MessagePack encoders will write any non-negative signed integer as unsigned, but this is not required by the standard (nor is writing integers in the shortest representation), so they may write them differently.

We also test the performance of hashing representative data alone, and subtract the execution time and executable size from the results of all tests. This attempts to eliminate the cost of actually accessing the encoded data from the results (under the assumption that the point of encoding the data is to do something with it.) A chunk of random data roughly the size of the smaller formats is hashed, and the same result is subtracted from all tests for fairness (since libraries that produce larger data would suffer from larger access times, so the overhead of their bloat should be included in the results.)

The _time overhead_ column in the [extended test results][extended-results] gives the percentage of additional time spent beyond the data hash time. This gives an idea of the cost of serializing an object relative to the cost of accessing the resulting encoded data.

## Tree test

The Tree test is a test of parsing data into a structure that can be accessed with a DOM-style API. This usually means the data is parsed into a tree of dynamically typed nodes in memory, and the programmer can then access the data in the tree at will in any order.

Tree parsers are typically slower than streaming parsers and require more memory, but they are much more convenient to use. They make error handling much easier, and will typically flag decoding errors immediately upon parsing the tree. Examples are the Node API in MPack, or the Document API in RapidJSON.

The Tree tests parse the data output from the structured data generator. The generated data is first serialized to all supported data formats and written to disk. The tests then load the data from disk into memory before starting the benchmark. The benchmark score is the average time to decode the object (minus the time to hash it.) Lower is better.

All tests output a hash value which should match regardless of data format. This ensures that deserialization has not lost precision or type information or otherwise mangled the data. (The exception is Jansson in the default mode since it re-orders object entries. An option to preserve order has been added to the test to allow it to match.)

We also test the performance of hashing the generated object alone, and subtract the execution time and executable size from the results of all tests. This attempts to eliminate the cost of actually accessing the data from the results (under the assumption that the point of decoding the data is to do something with it.)

The _time overhead_ column in the [extended test results][extended-results] gives the percentage of additional time spent above the raw object hash time. This gives an idea of the cost of deserializing data relative to the cost of accessing the resulting object.

## Incremental Parse Test

The Incremental test is a test of how quickly libraries can parse data incrementally. This includes SAX-style parsers (which use callback functions for each data type as they are parsed) and imperative parsers (which return one basic element at a time from a parse function). These should use very little memory, and should not allocate additional memory for each element.

The Incremental tests are not, however, tests of data of a hardcoded schema, where you expect the data to be of specific known types. The MPack Expect API or CMP's typed readers would be examples of this. See the discussion of schemas below.

As with the Tree tests above, the Incremental tests parse the previous randomly generated data, and the same hash subtraction is performed. The benchmark score is the average time to decode the data (minus the time to hash it.)

All tests should give the same hash value as each other and as the Tree tests regardless of data format. There is no allowance for re-ordering since incremental tests must parse in order.

# Results

These are the current results for popular libraries and formats for this test. More libraries, formats and configurations are available in the [extended results][extended-results], along with additional data such as standard deviation, hash results, time overhead, etc.

- [Speed - Desktop PC](#speed---desktop-pc)
  - [Small Data](#small-data): [Write](#write-test-1), [Tree](#tree-test-1), [Incremental](#incremental-parse-test-1)
  - [Large Data](#large-data): [Write](#write-test-2), [Tree](#tree-test-2), [Incremental](#incremental-parse-test-2)
- [Speed - Raspberry Pi](#speed---raspberry-pi)
  - [Small Data](#small-data-1): [Write](#write-test-3), [Tree](#tree-test-3), [Incremental](#incremental-parse-test-3)
  - [Large Data](#large-data-1): [Write](#write-test-4), [Tree](#tree-test-4), [Incremental](#incremental-parse-test-4)
- [Size - Raspberry Pi](#size---raspberry-pi)
  - [Small Data](#small-data-2): [Write](#write-test-5), [Tree](#tree-test-5), [Incremental](#incremental-parse-test-5)

[benchmark-c]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/common/benchmark.c
[buffer-h]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/common/buffer.h

[mpack-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-read.c
[rapidjson-sax]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-sax.cpp
[cmp-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/cmp/cmp-read.c
[libbson-iter]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/libbson/libbson-iter.c
[ubj-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/ubj/ubj-read.c
[mpack-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-write.c
[mpack-utf8-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-read.c
[rapidjson-dom]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-dom.cpp
[mpack-utf8-node]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-node.c
[hash-data]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/hash/hash-data.c
[rapidjson-insitu-dom]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-dom.cpp
[libbson-append]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/libbson/libbson-append.c
[jansson-ordered-load]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/jansson/jansson-load.c
[mpack-tracking-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-read.c
[hash-object]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/hash/hash-object.c
[cmp-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/cmp/cmp-write.c
[jansson-load]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/jansson/jansson-load.c
[rapidjson-insitu-sax]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-sax.cpp
[yajl-gen]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/yajl/yajl-gen.c
[ubj-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/ubj/ubj-write.c
[mongo-cxx-obj]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mongo-cxx/mongo-cxx-obj.cpp
[ubj-opt-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/ubj/ubj-read.c
[rapidjson-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-write.cpp
[mpack-node]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-node.c
[yajl-tree]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/yajl/yajl-tree.c
[ubj-opt-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/ubj/ubj-write.c
[mongo-cxx-builder]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mongo-cxx/mongo-cxx-builder.cpp
[binn-load]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/binn/binn-load.c
[msgpack-cpp-pack]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/msgpack/msgpack-cpp-pack.cpp
[yajl-parse]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/yajl/yajl-parse.c
[jansson-dump]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/jansson/jansson-dump.c
[json-builder]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/udp-json/json-builder.c
[msgpack-cpp-unpack]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/msgpack/msgpack-cpp-unpack.cpp
[mpack-tracking-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-write.c
[jansson-ordered-dump]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/jansson/jansson-dump.c
[json-parser]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/udp-json/json-parser.c
[msgpack-c-pack]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/msgpack/msgpack-c-pack.c
[msgpack-c-unpack]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/msgpack/msgpack-c-unpack.c
[binn-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/binn/binn-write.c


## Speed - Desktop PC

These results are for a speed-optimized build (`-O3 -flto`) on a typical, somewhat outdated desktop PC. The tests are run on Arch Linux in recovery mode.

CPU model: AMD Phenom(tm) II X6 1090T Processor  
Architecture: x86\_64  
Bogomips: 6402.44


### Small Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 2.26 | 19336 | subtracted from Write tests |
| [hash-object.c][hash-object] | 3.61 | 20016 | subtracted from Tree and Incremental tests |

_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including hashing and object generation code (which is included in all tests.)_


#### Write Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [MPack][mpack] (0.8.1) | [mpack-write.c][mpack-write] | MessagePack | 2.13 | 4088 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-pack.cpp][msgpack-cpp-pack] | MessagePack | 2.77 | 8616 |
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-pack.c][msgpack-c-pack] | MessagePack | 6.42 | 4088 |
| [CMP][cmp] (v14) | [cmp-write.c][cmp-write] | MessagePack | 7.33 | 3232 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-builder.cpp][mongo-cxx-builder] | BSON | 10.21 | 180952 |
| [RapidJSON][rapidjson] (1.0.2) | [rapidjson-write.cpp][rapidjson-write] | JSON | 12.26 | 16904 |
| [libbson][libbson] (1.3.0) | [libbson-append.c][libbson-append] | BSON | 14.47 | 21112 |
| [YAJL][yajl] (2.1.0) | [yajl-gen.c][yajl-gen] | JSON | 37.86 | 6752 |
| [Jansson][jansson] (2.7) | [jansson-dump.c][jansson-dump] | JSON | 63.40 | 12528 |

_The Time and Code Size columns show the net result after subtracting the hash-data time and size. In both columns, lower is better._


#### Tree Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 4.45 | 3496 |
| [MPack][mpack] (0.8.1) | [mpack-node.c][mpack-node] | MessagePack | 4.99 | 5624 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 5.99 | 180280 |
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-unpack.c][msgpack-c-unpack] | MessagePack | 6.47 | 5200 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-unpack.cpp][msgpack-cpp-unpack] | MessagePack | 9.40 | 19152 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-dom.cpp][rapidjson-insitu-dom] | JSON | 10.58 | 14600 |
| [YAJL][yajl] (2.1.0) | [yajl-tree.c][yajl-tree] | JSON | 99.31 | 22592 |
| [Jansson][jansson] (2.7) | [jansson-load.c][jansson-load] | JSON | 105.18 | 14440 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._


#### Incremental Parse Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [MPack][mpack] (0.8.1) | [mpack-read.c][mpack-read] | MessagePack | 3.20 | 5336 |
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 4.45 | 3496 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-sax.cpp][rapidjson-insitu-sax] | JSON | 5.57 | 6504 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 5.99 | 180280 |
| [CMP][cmp] (v14) | [cmp-read.c][cmp-read] | MessagePack | 7.43 | 3440 |
| [YAJL][yajl] (2.1.0) | [yajl-parse.c][yajl-parse] | JSON | 19.86 | 12512 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._


### Large Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 144.28 | 19336 | subtracted from Write tests |
| [hash-object.c][hash-object] | 353.45 | 20016 | subtracted from Tree and Incremental tests |

_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including hashing and object generation code (which is included in all tests.)_


#### Write Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [MPack][mpack] (0.8.1) | [mpack-write.c][mpack-write] | MessagePack | 307.27 | 4088 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-pack.cpp][msgpack-cpp-pack] | MessagePack | 347.14 | 8616 |
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-pack.c][msgpack-c-pack] | MessagePack | 544.24 | 4088 |
| [CMP][cmp] (v14) | [cmp-write.c][cmp-write] | MessagePack | 660.19 | 3232 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-builder.cpp][mongo-cxx-builder] | BSON | 715.26 | 180952 |
| [libbson][libbson] (1.3.0) | [libbson-append.c][libbson-append] | BSON | 1048.95 | 21112 |
| [RapidJSON][rapidjson] (1.0.2) | [rapidjson-write.cpp][rapidjson-write] | JSON | 1097.71 | 16904 |
| [YAJL][yajl] (2.1.0) | [yajl-gen.c][yajl-gen] | JSON | 3125.41 | 6752 |
| [Jansson][jansson] (2.7) | [jansson-dump.c][jansson-dump] | JSON | 6540.87 | 12528 |

_The Time and Code Size columns show the net result after subtracting the hash-data time and size. In both columns, lower is better._


#### Tree Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 301.90 | 3496 |
| [MPack][mpack] (0.8.1) | [mpack-node.c][mpack-node] | MessagePack | 414.16 | 5624 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 453.91 | 180280 |
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-unpack.c][msgpack-c-unpack] | MessagePack | 750.87 | 5200 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-unpack.cpp][msgpack-cpp-unpack] | MessagePack | 1004.67 | 19152 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-dom.cpp][rapidjson-insitu-dom] | JSON | 1129.46 | 14600 |
| [YAJL][yajl] (2.1.0) | [yajl-tree.c][yajl-tree] | JSON | 8236.40 | 22592 |
| [Jansson][jansson] (2.7) | [jansson-load.c][jansson-load] | JSON | 9529.31 | 14440 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._


#### Incremental Parse Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [MPack][mpack] (0.8.1) | [mpack-read.c][mpack-read] | MessagePack | 202.09 | 5336 |
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 301.90 | 3496 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 453.91 | 180280 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-sax.cpp][rapidjson-insitu-sax] | JSON | 532.73 | 6504 |
| [CMP][cmp] (v14) | [cmp-read.c][cmp-read] | MessagePack | 533.46 | 3440 |
| [YAJL][yajl] (2.1.0) | [yajl-parse.c][yajl-parse] | JSON | 1589.58 | 12512 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._


## Speed - Raspberry Pi

These results are for a speed-optimized build (`-O3 -flto`) on a first-generation Raspberry Pi, running a fresh install of Arch Linux ARM.

CPU model: ARMv6-compatible processor rev 7 (v6l)  
Hardware Revision: 000f (Model B rev2, 512 MB RAM)


### Small Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 30.31 | 16112 | subtracted from Write tests |
| [hash-object.c][hash-object] | 36.07 | 17104 | subtracted from Tree and Incremental tests |

_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including hashing and object generation code (which is included in all tests.)_


#### Write Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [MPack][mpack] (0.8.1) | [mpack-write.c][mpack-write] | MessagePack | 38.31 | 4032 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-pack.cpp][msgpack-cpp-pack] | MessagePack | 38.36 | 7628 |
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-pack.c][msgpack-c-pack] | MessagePack | 69.21 | 4384 |
| [CMP][cmp] (v14) | [cmp-write.c][cmp-write] | MessagePack | 89.75 | 3396 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-builder.cpp][mongo-cxx-builder] | BSON | 188.51 | 160444 |
| [libbson][libbson] (1.3.0) | [libbson-append.c][libbson-append] | BSON | 229.45 | 17604 |
| [RapidJSON][rapidjson] (1.0.2) | [rapidjson-write.cpp][rapidjson-write] | JSON | 231.33 | 18124 |
| [YAJL][yajl] (2.1.0) | [yajl-gen.c][yajl-gen] | JSON | 752.41 | 6840 |
| [Jansson][jansson] (2.7) | [jansson-dump.c][jansson-dump] | JSON | 1789.29 | 14884 |

_The Time and Code Size columns show the net result after subtracting the hash-data time and size. In both columns, lower is better._


#### Tree Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 63.48 | 3876 |
| [MPack][mpack] (0.8.1) | [mpack-node.c][mpack-node] | MessagePack | 69.05 | 5612 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 84.01 | 159648 |
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-unpack.c][msgpack-c-unpack] | MessagePack | 87.54 | 5884 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-unpack.cpp][msgpack-cpp-unpack] | MessagePack | 142.66 | 17300 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-dom.cpp][rapidjson-insitu-dom] | JSON | 170.34 | 13684 |
| [Jansson][jansson] (2.7) | [jansson-load.c][jansson-load] | JSON | 2604.03 | 16848 |
| [YAJL][yajl] (2.1.0) | [yajl-tree.c][yajl-tree] | JSON | 3416.81 | 18632 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._


#### Incremental Parse Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [MPack][mpack] (0.8.1) | [mpack-read.c][mpack-read] | MessagePack | 29.45 | 5460 |
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 63.48 | 3876 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 84.01 | 159648 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-sax.cpp][rapidjson-insitu-sax] | JSON | 99.28 | 6368 |
| [CMP][cmp] (v14) | [cmp-read.c][cmp-read] | MessagePack | 100.14 | 3316 |
| [YAJL][yajl] (2.1.0) | [yajl-parse.c][yajl-parse] | JSON | 349.72 | 11820 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._


### Large Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 3956.91 | 16112 | subtracted from Write tests |
| [hash-object.c][hash-object] | 6161.16 | 17104 | subtracted from Tree and Incremental tests |

_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including hashing and object generation code (which is included in all tests.)_



#### Write Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [MPack][mpack] (0.8.1) | [mpack-write.c][mpack-write] | MessagePack | 5610.92 | 4032 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-pack.cpp][msgpack-cpp-pack] | MessagePack | 6098.33 | 7628 |
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-pack.c][msgpack-c-pack] | MessagePack | 8142.64 | 4384 |
| [CMP][cmp] (v14) | [cmp-write.c][cmp-write] | MessagePack | 9206.05 | 3396 |
| [RapidJSON][rapidjson] (1.0.2) | [rapidjson-write.cpp][rapidjson-write] | JSON | 15235.80 | 18124 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-builder.cpp][mongo-cxx-builder] | BSON | 15984.76 | 160444 |
| [libbson][libbson] (1.3.0) | [libbson-append.c][libbson-append] | BSON | 18287.50 | 17604 |
| [YAJL][yajl] (2.1.0) | [yajl-gen.c][yajl-gen] | JSON | 61838.05 | 6840 |
| [Jansson][jansson] (2.7) | [jansson-dump.c][jansson-dump] | JSON | 151609.98 | 14884 |

_The Time and Code Size columns show the net result after subtracting the hash-data time and size. In both columns, lower is better._


#### Tree Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 2745.93 | 3876 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 4315.50 | 159648 |
| [MPack][mpack] (0.8.1) | [mpack-node.c][mpack-node] | MessagePack | 6316.98 | 5612 |
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-unpack.c][msgpack-c-unpack] | MessagePack | 10287.89 | 5884 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-unpack.cpp][msgpack-cpp-unpack] | MessagePack | 13581.87 | 17300 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-dom.cpp][rapidjson-insitu-dom] | JSON | 17504.30 | 13684 |
| [Jansson][jansson] (2.7) | [jansson-load.c][jansson-load] | JSON | 187014.94 | 16848 |
| [YAJL][yajl] (2.1.0) | [yajl-tree.c][yajl-tree] | JSON | 227495.40 | 18632 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._



#### Incremental Parse Test

| Library | Benchmark | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) |
|----|----|----|---:|---:|
| [MPack][mpack] (0.8.1) | [mpack-read.c][mpack-read] | MessagePack | 361.43 | 5460 |
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 2745.93 | 3876 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 4315.50 | 159648 |
| [CMP][cmp] (v14) | [cmp-read.c][cmp-read] | MessagePack | 5713.58 | 3316 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-sax.cpp][rapidjson-insitu-sax] | JSON | 7387.73 | 6368 |
| [YAJL][yajl] (2.1.0) | [yajl-parse.c][yajl-parse] | JSON | 24252.07 | 11820 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._



## Size - Raspberry Pi

These results are for a size-optimized build (`-Os -flto`) on a first-generation Raspberry Pi, running a fresh install of Arch Linux ARM.

For brevity we are only showing the results for a Raspberry Pi with small data, since you probably aren't worried about size optimization on desktops and servers, or parsing large data files on a Raspberry Pi.

The tables are sorted by code size.

CPU model: ARMv6-compatible processor rev 7 (v6l)  
Hardware Revision: 000f (Model B rev2, 512 MB RAM)


### Small Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 37.29 | 9036 | subtracted from Write tests |
| [hash-object.c][hash-object] | 43.94 | 9488 | subtracted from Tree and Incremental tests |


_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including hashing and object generation code (which is included in all tests.)_


#### Write Test

| Library | Benchmark | Format | Time<br>(μs) | Code Size<br>(bytes) ▲ |
|----|----|----|---:|---:|
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-pack.c][msgpack-c-pack] | MessagePack | 67.35 | 1528 |
| [CMP][cmp] (v14) | [cmp-write.c][cmp-write] | MessagePack | 100.66 | 2324 |
| [MPack][mpack] (0.8.1) | [mpack-write.c][mpack-write] | MessagePack | 38.90 | 3124 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-pack.cpp][msgpack-cpp-pack] | MessagePack | 68.38 | 3324 |
| [YAJL][yajl] (2.1.0) | [yajl-gen.c][yajl-gen] | JSON | 757.12 | 6012 |
| [RapidJSON][rapidjson] (1.0.2) | [rapidjson-write.cpp][rapidjson-write] | JSON | 345.26 | 8328 |
| [Jansson][jansson] (2.7) | [jansson-dump.c][jansson-dump] | JSON | 1883.20 | 10576 |
| [libbson][libbson] (1.3.0) | [libbson-append.c][libbson-append] | BSON | 264.93 | 14192 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-builder.cpp][mongo-cxx-builder] | BSON | 270.70 | 160500 |

_The Time and Code Size columns show the net result after subtracting the hash-data time and size. In both columns, lower is better._


#### Tree Test

| Library | Benchmark | Format | Time<br>(μs) | Code Size<br>(bytes) ▲ |
|----|----|----|---:|---:|
| [MPack][mpack] (0.8.1) | [mpack-node.c][mpack-node] | MessagePack | 90.79 | 3192 |
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 84.31 | 3252 |
| [msgpack C][msgpack] (1.3.0) | [msgpack-c-unpack.c][msgpack-c-unpack] | MessagePack | 107.23 | 3948 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-dom.cpp][rapidjson-insitu-dom] | JSON | 322.89 | 9236 |
| [msgpack C++][msgpack] (1.3.0) | [msgpack-cpp-unpack.cpp][msgpack-cpp-unpack] | MessagePack | 175.38 | 11044 |
| [Jansson][jansson] (2.7) | [jansson-load.c][jansson-load] | JSON | 2822.15 | 13476 |
| [YAJL][yajl] (2.1.0) | [yajl-tree.c][yajl-tree] | JSON | 3339.01 | 13972 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 89.95 | 161096 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._


#### Incremental Parse Test

| Library | Benchmark | Format | Time<br>(μs) | Code Size<br>(bytes) ▲ |
|----|----|----|---:|---:|
| [CMP][cmp] (v14) | [cmp-read.c][cmp-read] | MessagePack | 107.18 | 3192 |
| [MPack][mpack] (0.8.1) | [mpack-read.c][mpack-read] | MessagePack | 87.52 | 3208 |
| [libbson][libbson] (1.3.0) | [libbson-iter.c][libbson-iter] | BSON | 84.31 | 3252 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] | [rapidjson-sax.cpp][rapidjson-insitu-sax] | JSON | 207.46 | 4284 |
| [YAJL][yajl] (2.1.0) | [yajl-parse.c][yajl-parse] | JSON | 376.31 | 10180 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) | [mongo-cxx-obj.cpp][mongo-cxx-obj] | BSON | 89.95 | 161096 |

_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._


# Discussion

What follows is a series of highly biased and opinionated ramblings by the author of [MPack](https://github.com/ludocode/mpack). Caveat emptor.


## On Formats

This benchmark currently only tests a few different formats:

- JSON is of course extremely popular. There are dozens of implementations available.

- BSON is the storage and RPC format for MongoDB. All MongoDB installations probably outnumber the total number of applications in production that use all other binary schemaless formats, and there is almost certainly more BSON data in the real world than any other binary schemaless format.

- MessagePack is perhaps the most widely used out of schemaless binary data serialization formats in terms of different applications that use it. It is still pretty much insignificant compared to JSON though. MessagePack seems most commonly used for RPC and other non-HTTP network communication since most of its libraries are built around that.

A couple more formats are included in the extended results:

- UBJSON attempts to be yet another binary encoding of JSON. It has some novel features such as containers of fixed types, but these make decoding extremely complicated and some libraries [expose this complexity][ubj-read] in the API.

- Binn doesn't seem to be of much significance yet, but I wanted to include it to have two different schemaless binary formats that are traversible in place.

There are other binary formats that could be added to this benchmark. [Smile](https://en.wikipedia.org/wiki/Smile_%28data_interchange_format%29), [BJSON](http://bjson.org/) or [transenc](http://sourceforge.net/p/transenc/wiki/Specification/) are examples. None of these seemed notable enough to be included.

[Sereal](https://github.com/Sereal/Sereal) would be worthwhile to include. It's more oriented towards Perl serialization than as a binary JSON replacement though, supporting such types as weak cyclic references and regular expression objects. They have their own benchmarks comparing performance to MessagePack.

There is also [CBOR](https://tools.ietf.org/html/rfc7049) which is a fork of MessagePack that attempts formal standardization, both in the specification and in supporting a registration process for extension types. It claims many implementations but does not seem to be used much, and it is [not supported by the MessagePack community](https://github.com/msgpack/msgpack/issues/180). Despite the claims of open process, the RFC added questionable features that were not agreed upon by the MessagePack community such as indefinite length types.

Other human readable serialization formats could be added as well. YAML would probably be worthwhile, although this is arguable. It's not really a serialization format so much as a data entry format; it's mostly meant to be hand-written rather than generated.

I had originally added an XML parser to this list, encoding the data types as specified by [XML-RPC](https://en.wikipedia.org/wiki/XML-RPC). I eventually removed it, mostly because it was a distraction from real data serialization formats. It's not only that the performance was awful or that the code was a nightmare. The data simply does not translate well to XML. The overhead of XML-RPC is ludicrous. XML is not a data serialization format and should not be used as such; it's a markup language for documents of mostly text.


## On Libraries

Choosing a representative set of libraries was difficult. There are dozens of JSON libraries and it's difficult to tell which are popular. For C++, obviously RapidJSON is the most common. For C, YAJL seems to be quite popular, though it does not have good performance and may no longer be maintained. Jansson is better maintained and seems cleaner and easier to use, but is unfortunately even slower than YAJL.

The reason I added quite a few JSON parsers in C is because I was looking for one that isn't painfully slow. The YAJL gen and tree APIs use individually allocated values, copy all strings, recalculate length from the null-terminator over and over. Jansson and json-parser/json-builder go even farther, making trees mutable and providing symmetry between the decoder and encoder. I was going to try [json-c](https://github.com/json-c/json-c) or [ujson4c](https://github.com/esnme/ujson4c/) next but they're just more of the same, and won't be anywhere near as fast as RapidJSON. I can't seem to find one that is written with performance in mind.

An often recommended JSON library is [jsmn](http://zserge.com/jsmn.html). I have not added it because it is not a parser. It is merely a tokenizer. Examples of using jsmn [simply ignore escape sequences](https://github.com/alisdair/jsmn-example/blob/8e8c4f11f60d6c60aa11deb83aa61c42c6bc2764/json.c#L89-L93) which is obviously incorrect. A library that can't unescape unicode sequences or parse numbers is not useful; you can't actually get the data out of it. I don't even know how you would use this library without those features, and I don't know why it doesn't provide them at least as standalone functions to run on string tokens.

The MessagePack homepage lists at least two other C++ libraries. The first is not standalone; it's part of [netLink](https://github.com/Lichtso/netLink), a networking library. The other, [mneumann/MessagePack](https://github.com/mneumann/MessagePack), looks interesting especially since it's tailored to C++11. It seems to have been written mostly to provide Ruby bindings though and does not appear to be actively maintained; it doesn't support the modern MessagePack spec (the separation of str/bin/ext types.) In addition, [msgpackalt](https://code.google.com/p/msgpackalt/) is interesting and looks like it could be extremely fast, but it was abandoned years ago and does not support the modern MessagePack spec either.

The BSON homepage currently recommends using the MongoDB C++ Legacy Driver as the primary BSON implementation for C++. Clearly from the code size results, GCC fails to strip out the non-BSON code, so this is pretty much a non-starter for using BSON in some other project. At least two projects have attempted to separate the BSON components but neither has been maintained in years. (The new MongoDB C++ driver has made a 3.0.0-rc0 prerelease only a few hours ago as of this writing. It should be added to this benchmark once it makes an official release.)


## On Schemas

There are a number of binary data serialization formats that use schemas such as [Protocol Buffers](https://developers.google.com/protocol-buffers/), [FlatBuffers](https://google.github.io/flatbuffers/), [Avro](http://avro.apache.org/), or [Thrift](https://thrift.apache.org/). These cannot parse arbitrary schemaless data such as the data generated in this test. They typically need much more setup to use; you need to create a schema (or get a schema for the data you are trying to parse), and you need to generate some code that gets compiled into your project. This means they are more limited in the languages and environments they support. It is also very difficult to hand-write data without having to build a converter based on the schema. On the flipside, they perform most validation automatically, so there is less error-checking involved in parsing data. They also eliminate all type information from the data itself, so they can be faster and smaller.

In this sense they are not really competitors to JSON or MessagePack. The advantage of schemaless formats is to be able to trivially parse data without this additional scaffolding. MessagePack is much easier to get started with, and MPack's Node API in particular makes type error-handling much easier, negating many of the advantages of these other data formats.

A benchmark comparing MessagePack to Protocol Buffers or FlatBuffers would be worthwhile. A good MessagePack comparison benchmark would make use of static type features that match the predefined schema, such as msgpack-c's C++ [Class Adaptors](https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_adaptor), MPack's [Expect API](https://github.com/ludocode/mpack/blob/master/docs/expect.md), or CMP's type readers in place of the generated code of other formats. This is outside the scope of this benchmark.


## On Traversible Formats

BSON and Binn (and some schema formats such as FlatBuffers) are designed to be traversible in-place. Every compound element states the size or offset in bytes of each of its children. The benefit of this is that you can jump past array and map values that you aren't interested in without decoding their contents. This means BSON and Binn get many of the benefits of both a tree parser and an incremental parser: data is parsed on-the-fly as it is accessed, even though it can be traversed at will. This is why [libbson-iter][libbson-iter], [mongo-cxx-obj][mongo-cxx-obj] and [binn-load][binn-load] participate in both benchmark categories.

As we can see from these benchmarks though, the benefits in parsing time are modest at best. libbson is slower than the MPack imperative parser, but somewhat faster than the MPack Node API and the msgpack-c reference implementation. It does not save much on overall decoding time even in comparison to allocating tree parsers for MessagePack.

The glaring downside, as we see from the encoding results, is that encoding is _slow_. This is because, logically, the data needs to be encoded inside-out. You have to add up the encoded bytes of all of a map or array's children before you can encode their parent. Encoding libraries for both formats are especially slow for small messages because they are not bound by memory bandwidth.

The other downside is that these formats are large. With the data in this benchmark, BSON is even larger than JSON. Binn is remarkably small for a traversible format but still larger than MessagePack. Of course this saves having to allocate a tree in memory to support random access. Applications are rarely memory-bound these days though, and the memory overhead is not very large anyway. This is because nodes in an MPack or RapidJSON tree (in-situ) do not contain copies of strings; they just point into the original data.

Moreover, since the incremental parsing speed of MessagePack is faster than BSON especially in large messages, it would actually be faster to convert MessagePack to JSON than to convert BSON to JSON. This is probably mostly due to its size, but in part also due to its additional complexity (especially with its array keys.)

Ironically, the test in this benchmark in which BSON has the largest advantage is in tree traversal of huge messages on a nearly embedded device. This is the complete opposite of BSON's intended use case.

Where BSON is meant to shine is in accessing only certain parts of the data. If you have a large message and you only want to read a few pieces of it, BSON might be appropriate. The benchmark does not test this scenario. But either way, despite the tree API it provides, BSON cannot realistically be edited in-place. If an element changes size, the entire rest of the document would need to shift forward or backward, and all of its parents need to have their size updated as well. So this only works if you are only accessing small pieces of it and you rarely change any part of it.

The best use case for BSON would seem to be when you have many gigabytes of data on disk that is very randomly accessed, arranged in chunks of uncompressed BSON of several hundred kilobytes or more, where you only need to access some small part of a given BSON chunk at a time, and where you rarely modify any existing data. Having more data than RAM means it can't all be cached, and having chunks several times the virtual memory page size means significant data can be skipped without being paged in from disk. Is this really the use case for MongoDB? I guess it seems like it, but even in this ideal scenario I'm not convinced that it's any better than just using MessagePack.

The worst use case for these formats is in two-way communication. The minor gains in decoding speed are lost by huge penalties in encoding and message size. These formats really need to be stored on disk and read far more often than written in order to be useful.

In any case, Binn certainly seems the far more interesting of the two formats. BSON data is huge, and the format has some glaring design flaws such as array keys as decimal strings and an untyped root element. Binn has a much nicer design, and seems to generate data not much bigger than MessagePack. Given the variable-width byte lengths, typed root element, proper arrays, declared width strings, maps with integer keys... It seems to me that with a bit of optimization work, Binn could be superior to BSON in every way.


## On Human Readability

The typically stated advantage of JSON is that it is human-readable, but this is misleading. JSON in the real world is usually minified and gzipped. This is necessary to improve performance and reduce bandwidth.

Minified JSON is _not human-readable_. This is the first part of the content of the small (size 2) JSON data file from this benchmark:

- `[{"vzl":[null,196,{"ahilyfiq":{"xzuqwmwgmz":null},"sxnjbwbub":null},null,true],"luuoavw":"tjezd nìrxooqeu ewòm","jzzqftf":"h\n srmp h¹sxg ÕrzÁyv v"},{"pubjoe":["ncnr pjifdfnnx tv av ldv blcakuufb Uqxtan krdhzd xcohsgxx vwa bg",{"mxpdfu":false},[false,153,134]],"nwc":"cegUiirmqtxtt","pempx":71},{"ltfqwj":null,"njac":[201,false,null,[null,266,"sr m:pefvby ouwtzzu etxe\nnjl oiamfctul ewoyjun mfef pk uj pyxEU xolrzqysr crs\\ gnyvpjozo d2rcpzl czcgbccmc iapr mrfmgky "],[false,300,true,"hqgeta npsqyvOwo rshqaa qlux"]],"ewymsyu":43,"svvg":110},null,{"mbifoa":[-979,{"mfxti":"bbrelcfvtvvngxershsbx","vd":-723},["atou rwuAn",{"thma":"hc hmoi sqkx twyrrvul vrie sifixfv"},82]],"qgmczbjc":["fcgee beqvwXxk ft jkomktpnq...`

This is not much more readable than a hexdump, and it is a waste of time to try when you could just pipe it to a formatting tool instead. So if you need to pipe the data to a formatting tool like `python -m json.tool` just to read it, you might as well pipe it to [`msgpack2json -d`](https://github.com/ludocode/msgpack-tools/). The effect is the same: you're taking something obfuscated and converting it to readable text.

For the same reason, UBJSON's [ASCII type codes](https://en.wikipedia.org/wiki/UBJSON#type) are novel, but ultimately not very useful. Viewing the data in a debugger is not a good idea either way, and if you're going to run the data through `hexdump` to view the bytes, you might as well pipe it to something that will actually make it legible.

MPack contains a [rationale](https://github.com/ludocode/mpack#why-not-just-use-json) for using a binary encoding over JSON which mostly deals with efficiency. There are other advantages as well. For example, a simpler format is a smaller attack vector and is less likely to cause security issues. A binary format is also less likely to encounter locale-specific bugs such as delimiters in decimal float parsing. The advantages of human readability are overstated and the costs are high.

Web APIs concerned with usability are starting to go the route of pretty-printing their output to make it easy to debug. GitHub is doing this for example. This is unfortunate; in my opinion we are heading in the wrong direction. Computers should not talk to computers using human languages.


## On Hashing

The benchmark tests perform hashing in order to simulate accessing the resulting data. They also subtract away this hash time from the results. The assumption here is that you're going to be using the (de-)serialized data in some way, so this should not be included in the measurement of the time spent serializing it. The [extended results][extended-results] also show the the serialization time as a factor of the hash time, giving an idea of how much slower serialization is compared to the data already being decoded in memory.

In some cases this can eliminate most of the run-time of the test. For example in the incremental parsers for large data on a Raspberry Pi, most of the time spent is in moving the data into the CPU cache. This seems reasonable. We don't want to measure the performance of using the data; only the overhead of decoding it. Hashing is about the simplest thing you could do with the data, and if it takes up most of the run-time, that's fine. That just means that using the data in any way will dwarf the time spent actually decoding it. This is a good thing: it means that our parser is approaching theoretical limits, or the speed of raw structs in memory. It makes sense that we report only this overhead as the time used by the library.


## On Compiled Code Size

If you're looking to use a small schemaless binary format and library for an embedded device where code size matters, the [size-optimized results](#size---raspberry-pi) may provide the information you are looking for. The results may not be useful though. A Raspberry Pi is more like a mid-range smartphone than an embedded device, so code size is not particularly important, and allocations are still used freely.

For example, the write tests above use a growable buffer (since that's by far the most natural way to pack data for 99% of applications.) CMP doesn't include one on its own, so a [simple growable buffer][buffer-h] is implemented in the benchmark. This is sort of unfair as a benchmark comparison for embedded devices, and it probably accounts for a significant fraction of the code size. If you're on an embedded device, you may want to encode to a fixed size buffer, which will significantly change the code size results.

Of course if you're really desperate for code size, you're probably better off coming up with your own ad-hoc binary format anyway instead of using something with dynamic type information. For example I wouldn't recommend using MPack on a Pebble smartwatch (even though I've gotten it running on one) since the code is just too big (and the SDK includes its own dictionary implementation for messaging anyway.)

Also note libbson's large code size when encoding. It has all sorts of data tables, like the [first 1000 integers as strings](https://github.com/mongodb/libbson/blob/12750146601dff97493675e4d4b85a7d5c03d6e1/src/bson/bson-keys.c#L24-L124) or [hex conversion tables](https://github.com/mongodb/libbson/blob/12750146601dff97493675e4d4b85a7d5c03d6e1/src/bson/bson-oid.c#L30-L92) which bloat its size. This is probably fine since libbson is meant for MongoDB servers, not embedded devices. And of course in the MongoDB C++ Driver, the compiler cannot strip out the non-BSON code, so we have no idea how big it really is.

Something else of note is that C++ parsers tend to be much larger than their C counterparts. The reference msgpack-c implementation seems to have two totally independent implementations in the same library, one in C and one in C++. The C++ version encodes faster while the C version parses faster, but in either case, the C++ code is more than twice the size, regardless of architecture or optimization options. Similarly, [json-parser][json-parser-lib]/[json-builder][json-builder-lib] (written in C) are significantly smaller than RapidJSON, even though they provide a mutable dynamic tree.

A significant culprit here is of course templates. C++ programmers will often talk of the performance improvements available from templates, but this is not borne out in the above results. Pervasive templating does not seem to provide the performance improvements to justify such large compiled code size, long compile times and obfuscated code. The msgpack-c C implementation even [emulates templates](https://github.com/msgpack/msgpack-c/blob/master/include/msgpack/unpack_template.h) with `#include` hacks to define templated functions using macros full of control flow statements, and it's still significantly slower than MPack.

Templates can provide performance improvements in theory. Templating as used in RapidJSON can eliminate indirect function calls to flush a buffer, or eliminate branches for encoding options. These are very minor optimizations considering the state of modern branch predictors. The flipside is that templates are guaranteed to explode code size, compile times, and the costs of code maintainance and new feature development.

What the results above don't show is the code size penalty for multiple template instantiations. The write tests above (for example) show encoding only to one output stream: a growable buffer. So the code size results reflect only a single template instantiation. In a typical application you may be writing MessagePack to a growable buffer, to a fixed memory buffer, to a file, to a deflate stream, or to a socket depending on the context. With MPack or CMP, each of these will share nearly all encoding code, so it does not take up any additional space to add output streams. But with msgpack C++, most of the library will be duplicated for each output method, so the code size doubles for every new stream.

Despite the single template instantiations, the C++ benchmarks are still larger than C. In fact the compiled msgpack C++ unpacker even manages to be larger than RapidJSON! This is despite the code that uses it being [totally unaware of templates](https://github.com/ludocode/schemaless-benchmarks/blob/master/src/msgpack/msgpack-cpp-unpack.cpp) (having no `template` keyword or `<>` angle brackets at all), and despite MessagePack not requiring unicode unescaping, decimal parsing, or any other complexities of human readability.

The developers of libraries like msgpack C++ and RapidJSON will try hard to convince you that pervasive templating is key to their performance. Don't believe them. It isn't true.


## On Languages

Currently, the only languages supported by this benchmark are C and C++. I'm not really interested in benchmarks for parsers in dynamic scripting languages like Python or Javascript. There are [other benchmarks](https://www.npmjs.com/package/msgpack-lite#benchmarks) for some of these languages already. But I would be interested in benchmarking parsers for fast competitors to C/C++ such as Rust, D, Nim and Go, as well as parsers for very different languages such as Haskell.

The problem is that the benchmark harness would need to be completely rewritten for these languages (including the hash benchmarks, object generation code, etc.) It's not enough to use C bindings because the hashing code needs to be written in the language itself so that it can be optimized properly. This will be very difficult especially for safety-oriented languages considering how object data is flattened to minimize its impact on performance.


## On String Keys

JSON, UBJSON and BSON require that object keys be strings, so the data in this test uses strings. (BSON is in fact even less efficient, treating arrays as objects and using string keys for the index of every element.) This is not the case for most binary data serialization formats however.

Binn has a map type that uses integer keys instead of strings, and MessagePack allows any type as the key for a map. Data can be encoded more efficiently in these formats if object/map keys are integers instead, and a simple C/C++ enum can be used for the keys rather than strings. Of course this negates most of the benefits of compatibility with a human-readable format, such as the ease of viewing the data for debugging purposes. But string keys are probably the most inefficient part of these formats, and eliminating them makes the resulting data much closer to a schema-conforming binary format such as Protocol Buffers. Adding a handful of enums to your code to use integer keys is a whole lot easier than writing a schema and generating source code.

What's nice about Binn and MessagePack is that it is totally optional, and you can mix and match between them even within the same document. In an indie game for example, you can use string keys where it doesn't matter (such as a savegame or hand-written game data), and use integer keys when you want the smallest messages (such as network packets.) This allows rapid prototyping and development, and the data can be transitioned gradually to a "more" binary/schema-like format by switching to integer keys wherever you want to eliminate some of the overhead.

In this benchmark, 1/8th of the values are strings, many of which are quite long. This in combination with string keys means that the vast majority the bytes in these MessagePack or Binn documents are strings, and so the overhead of this is huge. Moreover, JSON and friends require the whole document to be in UTF-8 (or some Unicode encoding), but only strings need to be in UTF-8 in other formats. UTF-8 validation is very important for values, but may be unnecessary for keys since they are usually just matched against short ASCII strings in a key/value lookup, so if a key is invalid UTF-8, it simply won't match anything. The overhead of UTF-8 validation (or in-situ UTF-8 unescaping in the case of RapidJSON) is quite large, and almost all of this overhead can be eliminated by skipping UTF-8 validation of keys and transitioning to integer keys instead.


## On String Unescaping and Validation

The benchmarks are designed to test RapidJSON parsing in-situ since this is the recommended way to use it. In order to support this fairly, all parsing tests (including hash-object) perform a complete copy of the input data on every iteration, even if they don't modify it (and indeed, RapidJSON is the only library that modifies it.)

The [extended results][extended-results] show just how powerful the in-situ optimization is for RapidJSON. Parsing without it is about 50% slower. This is because every string needs to be allocated separately so it can be written unescaped. If you're using RapidJSON, you should consider enabling the in-situ optimization.

Of course, even better than parsing in-situ is using a format that doesn't require escape sequences in the first place. Strings in most binary formats are simply UTF-8, so no conversion is necessary, and validation can be performed on a read-only data buffer. (BSON uses null-terminated UTF-8 so it cannot handle NUL bytes in strings without using something like Modified UTF-8, which would fail in a proper UTF-8 validator. This is not a problem for Binn or MessagePack.)

The MPack UTF-8 validating parsers in the extended results (which check UTF-8 in both keys and values) have around 40%-100% overhead over the non-UTF-8 parsers in some tests. The UTF-8 checks in MPack are currently not very well optimized. The validator is very simple, and validation is performed in a separate pass from data parsing. An option on the Tree parser to perform UTF-8 checks during parsing might provide significant speed improvements. (This may become the default in MPack in the future since the MessagePack specification requires UTF-8 in strings.)


## On Mutable Dynamic Trees

Fast tree parsers such as MPack and RapidJSON (in-situ) parse a document into an immutable tree which contains pointers to strings in the original data. In a sense the tree is really just an index over the data which allows random access (similar to what BSON and Binn have built-in as part of the format.) This provides a very small memory overhead and a convenient API to access the data in any order while still providing most of the speed of parsing incrementally.

Jansson on the other hand parses the data into a mutable tree. Every string is copied, every node is individually allocated and reference-counted, every object's entries are re-ordered in hashtables. In this benchmark, Jansson placed dead last in nearly every speed test in which it participated.

Of course this isn't really a good test of Jansson's capabilities. Jansson is one of the few C/C++ libraries that can parse a JSON file, mutate it in some destructive way, and then write it back out again without having to adapt it to some other data structure. But I have to question the usefulness of this in a statically typed language. It seems to me that the goal of a parser in a statically typed language should be to reduce it to static types as early as possible, so as to provide type-checking and error-checking as early as possible and avoid leaking this into the rest of your application. Jansson does not provide any additional safety or error-propagating capabilities such as those that MPack provides, so you still have to perform type checks any time you access anything.

It really seems to me that a dynamic mutable tree should be totally separate from serialization. Given how compatible the different formats are in this benchmark, a mutable tree such as that provided by Jansson could be implemented as a separate library mostly independent of data format, and this could easily be serialized with an incremental encoder and decoder in any number of serialization formats. This could be the basis for a [pandoc](http://pandoc.org/)-style converter for schemaless data formats, and would be much more useful than forcing such a mutable tree as the interface to serialization.


## On Compilers

GCC 5.3.0 is used in all of the above tests. GCC appears to give better results than Clang in every test category on all platforms.

In case you are curious, below is a table comparing GCC 5.3.0 to Clang 3.7.0 for a single test. A Tree parser should provide a decent compiler optimization test, so the [mpack-node.c][mpack-node] benchmark is used. Shown are the total time and total code size (no hash subtraction is performed) for the "Small Data" test for MPack 0.8.1:

| [mpack-node.c][mpack-node] | Optimization<br>Options | Desktop<br>x86\_64<br>Time<br>(μs) | Desktop<br>x86\_64<br>Code Size<br>(bytes) | Raspberry Pi<br>ARMv6<br>Time<br>(μs) | Raspberry Pi<br>ARMv6<br>Code Size<br>(bytes) |
|:------------|:------------|----------:|------:|-----------:|------:|
| GCC 5.3.0   | `-O3 -flto` |  8.630303 | 25640 | 103.776553 | 22716 |
| Clang 3.7.0 | `-O3 -flto` |  9.094826 | 20984 | 112.694547 | 17012 |
| GCC 5.3.0   | `-Os -flto` | 11.751750 | 15256 | 134.381949 | 12680 |
| Clang 3.7.0 | `-Os -flto` |  9.885979 | 18104 | 116.988308 | 15748 |

GCC wins on speed in speed-optimized builds and on size in size-optimized builds on both x86\_64 and ARMv6. (It is also slower/larger in the complementary category in all cases. Its optimizations in either direction could be considered more "extreme" than Clang, or perhaps simply more effective.)

The usability of different compilers for day-to-day development is a more interesting topic, but it's unrelated to this benchmark.


## On Memory Allocators

The benchmark harness [pre-fragments memory][benchmark-c] to simulate a more realistic environment, where the library is embedded into a larger application. It performs a large number of allocations of random sizes, then shuffles them and frees half of them before starting the test.

The benchmarks also use the default system allocator, which on Arch Linux is ptmalloc in glibc. I did a bit of testing with jemalloc to see whether it improves benchmark performance over ptmalloc. It has negligible effect on libraries that only perform large page allocations and do not individually allocate nodes and strings (like MPack.) It can have a significant effect on libraries that perform lots of small allocations though, in some cases reducing the time per iteration by up to 15%. The pre-fragment step also has very little effect on jemalloc, whereas small changes to it can have visible effects on results with ptmalloc.
  
In any case, although some tests are faster with jemalloc, the order of results is nearly identical as compared to ptmalloc with the pre-fragment step. Most applications do not customize the allocator so the results are probably more relevant with ptmalloc.
