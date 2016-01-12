
# Extended Results

This contains the extended benchmarking results. All data formats and libraries supported by the benchmark are included here, and several configurations are provided for some libraries. For example ordered/unordered for Jansson, normal/in-situ for RapidJSON, unoptimized/type-optimized for ubj, etc.

Standard deviation is shown for the remaining five iterations after dropping highest and lowest of seven total iterations. Standard deviation is correctly propagated in hash subtraction and overhead calculation.

- [Speed - Desktop PC](#speed---desktop-pc)
  - [Small Data](#small-data): [Write](#write-test-1), [Tree](#tree-test-1), [Incremental](#incremental-parse-test-1)
  - [Large Data](#large-data): [Write](#write-test-2), [Tree](#tree-test-2), [Incremental](#incremental-parse-test-2)
- [Speed - Chromebook](#speed---chromebook)
  - [Small Data](#small-data-1): [Write](#write-test-3), [Tree](#tree-test-3), [Incremental](#incremental-parse-test-3)
  - [Large Data](#large-data-1): [Write](#write-test-4), [Tree](#tree-test-4), [Incremental](#incremental-parse-test-4)
- [Speed - Raspberry Pi](#speed---raspberry-pi)
  - [Small Data](#small-data-2): [Write](#write-test-5), [Tree](#tree-test-5), [Incremental](#incremental-parse-test-5)
  - [Large Data](#large-data-2): [Write](#write-test-6), [Tree](#tree-test-6), [Incremental](#incremental-parse-test-6)
- [Size - Raspberry Pi](#size---raspberry-pi)
  - [Small Data](#small-data-3): [Write](#write-test-7), [Tree](#tree-test-7), [Incremental](#incremental-parse-test-7)

[rapidjson]: http://rapidjson.org/
[msgpack]: https://github.com/msgpack/msgpack-c
[libbson]: https://github.com/mongodb/libbson
[mongo-cxx]: https://github.com/mongodb/mongo-cxx-driver
[binn]: https://github.com/liteserver/binn
[json-parser-lib]: https://github.com/udp/json-parser
[jansson]: http://www.digip.org/jansson/
[cmp]: https://github.com/camgunz/cmp
[yajl]: http://lloyd.github.io/yajl/
[json-builder-lib]: https://github.com/udp/json-builder
[ubj]: https://github.com/Steve132/ubj
[mpack]: https://github.com/ludocode/mpack

[mpack-node]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-node.c
[mpack-utf8-node]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-node.c
[yajl-parse]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/yajl/yajl-parse.c
[hash-data]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/hash/hash-data.c
[cmp-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/cmp/cmp-read.c
[ubj-opt-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/ubj/ubj-read.c
[msgpack-c-pack]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/msgpack/msgpack-c-pack.c
[rapidjson-insitu-sax]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-sax.cpp
[json-builder]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/udp-json/json-builder.c
[rapidjson-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-write.cpp
[libbson-append]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/libbson/libbson-append.c
[yajl-tree]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/yajl/yajl-tree.c
[jansson-ordered-load]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/jansson/jansson-load.c
[libbson-iter]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/libbson/libbson-iter.c
[binn-load]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/binn/binn-load.c
[cmp-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/cmp/cmp-write.c
[mongo-cxx-builder]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mongo-cxx/mongo-cxx-builder.cpp
[rapidjson-sax]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-sax.cpp
[msgpack-cpp-unpack]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/msgpack/msgpack-cpp-unpack.cpp
[rapidjson-insitu-dom]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-dom.cpp
[msgpack-cpp-pack]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/msgpack/msgpack-cpp-pack.cpp
[hash-object]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/hash/hash-object.c
[mpack-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-read.c
[binn-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/binn/binn-write.c
[mpack-tracking-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-write.c
[yajl-gen]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/yajl/yajl-gen.c
[ubj-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/ubj/ubj-write.c
[mpack-utf8-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-read.c
[json-parser]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/udp-json/json-parser.c
[ubj-opt-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/ubj/ubj-write.c
[jansson-dump]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/jansson/jansson-dump.c
[mpack-write]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-write.c
[mpack-tracking-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mpack/mpack-read.c
[jansson-load]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/jansson/jansson-load.c
[mongo-cxx-obj]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/mongo-cxx/mongo-cxx-obj.cpp
[rapidjson-dom]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/rapidjson/rapidjson-dom.cpp
[ubj-read]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/ubj/ubj-read.c
[jansson-ordered-dump]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/jansson/jansson-dump.c
[msgpack-c-unpack]: https://github.com/ludocode/schemaless-benchmarks/blob/master/src/msgpack/msgpack-c-unpack.c


## Speed - Desktop PC

These results are for a speed-optimized build (`-O3 -flto`) on a typical, somewhat outdated desktop PC. The tests are run on Arch Linux in recovery mode.

CPU model: AMD Phenom(tm) II X6 1090T Processor  
Bogomips: 6402.44
Tue Jan 12 03:04:08 EST 2016

### Small Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 2.26 ± 0.09 | 19336 | subtracted from Write tests |
| [hash-object.c][hash-object] | 3.61 ± 0.01 | 20016 | subtracted from Tree and Incremental tests |


_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including object generation code (which is included in all tests.)_



#### Write Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-write] | MessagePack | 2.13 ± 0.09 | 4088 | 1.94 ± 0.08 | 1a9c6681 |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-pack] | MessagePack | 2.77 ± 0.09 | 8616 | 2.22 ± 0.09 | 1a9c6681 |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-write] | MessagePack | 4.89 ± 0.09 | 5368 | 3.16 ± 0.13 | 1a9c6681 |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-pack] | MessagePack | 6.42 ± 0.11 | 4088 | 3.83 ± 0.15 | 1a9c6681 |
| [CMP][cmp] (v14) [(C)][cmp-write] | MessagePack | 7.33 ± 0.10 | 3232 | 4.24 ± 0.17 | 1a9c6681 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-builder] | BSON | 10.21 ± 0.10 | 180952 | 5.51 ± 0.22 | 4e02a1f9 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-write] | UBJSON | 10.41 ± 0.09 | 3352 | 5.60 ± 0.22 | c1a19bb7 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-write] | JSON | 12.26 ± 0.09 | 16904 | 6.41 ± 0.25 | e4495ccf |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-write] | UBJSON | 13.59 ± 0.09 | 4184 | 7.00 ± 0.28 | 37f97981 |
| [libbson][libbson] (1.3.0) [(C)][libbson-append] | BSON | 14.47 ± 0.11 | 21112 | 7.39 ± 0.29 | 4e02a1f9 |
| [Binn][binn] (b3e2c27) [(C)][binn-write] | Binn | 17.48 ± 0.23 | 8216 | 8.72 ± 0.36 | d7327409 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-gen] | JSON | 37.86 ± 0.29 | 6752 | 17.72 ± 0.71 | 4694ca58 |
| [json-builder][json-builder-lib] (19c739f) [(C)][json-builder] | JSON | 51.64 ± 1.90 | 4560 | 23.80 ± 1.26 | 75d4861f |
| [Jansson][jansson] (2.7) [(C)][jansson-dump] | JSON | 63.40 ± 2.11 | 12528 | 28.99 ± 1.48 | 586d4446 |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-dump] | JSON | 74.04 ± 2.83 | 12704 | 33.69 ± 1.83 | 97b6e2b2 |



_The Time and Code Size columns show the net result after subtracting the hash-data time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-data. In all three columns, lower is better._



#### Tree Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 4.45 ± 0.01 | 3496 | 2.23 ± 0.00 | 0075ff81 |
| [MPack][mpack] (0.8.1) [(C)][mpack-node] | MessagePack | 4.99 ± 0.01 | 5624 | 2.38 ± 0.00 | 0075ff81 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 5.99 ± 0.04 | 180280 | 2.66 ± 0.01 | 0075ff81 |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-unpack] | MessagePack | 6.47 ± 0.03 | 5200 | 2.79 ± 0.01 | 0075ff81 |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 7.60 ± 0.02 | 4944 | 3.11 ± 0.01 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-node] | MessagePack | 8.44 ± 0.01 | 6200 | 3.34 ± 0.01 | 0075ff81 |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-unpack] | MessagePack | 9.40 ± 0.03 | 19152 | 3.60 ± 0.01 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-dom] | JSON | 10.58 ± 0.03 | 14600 | 3.93 ± 0.01 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-dom] | JSON | 15.18 ± 0.05 | 15800 | 5.20 ± 0.02 | 0075ff81 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-read] | UBJSON | 45.62 ± 1.31 | 10072 | 13.63 ± 0.36 | 0075ff81 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-read] | UBJSON | 51.78 ± 0.95 | 10072 | 15.34 ± 0.26 | 0075ff81 |
| [json-parser][json-parser-lib] (7053321) [(C)][json-parser] | JSON | 67.89 ± 1.73 | 8288 | 19.80 ± 0.48 | 0075ff81 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-tree] | JSON | 99.31 ± 3.10 | 22592 | 28.50 ± 0.86 | 0075ff81 |
| [Jansson][jansson] (2.7) [(C)][jansson-load] | JSON | 105.18 ± 1.39 | 14440 | 30.12 ± 0.39 | 38b31ddb |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-load] | JSON | 109.69 ± 2.63 | 14792 | 31.37 ± 0.73 | 0075ff81 |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._



#### Incremental Parse Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-read] | MessagePack | 3.20 ± 0.01 | 5336 | 1.89 ± 0.00 | 0075ff81 |
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 4.45 ± 0.01 | 3496 | 2.23 ± 0.00 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-read] | MessagePack | 5.10 ± 0.02 | 7552 | 2.41 ± 0.01 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-sax] | JSON | 5.57 ± 0.02 | 6504 | 2.54 ± 0.01 | 0075ff81 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 5.99 ± 0.04 | 180280 | 2.66 ± 0.01 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-read] | MessagePack | 6.53 ± 0.02 | 5464 | 2.81 ± 0.01 | 0075ff81 |
| [CMP][cmp] (v14) [(C)][cmp-read] | MessagePack | 7.43 ± 0.06 | 3440 | 3.06 ± 0.02 | 0075ff81 |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 7.60 ± 0.02 | 4944 | 3.11 ± 0.01 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-sax] | JSON | 9.70 ± 0.02 | 8688 | 3.68 ± 0.01 | 0075ff81 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-parse] | JSON | 19.86 ± 0.06 | 12512 | 6.50 ± 0.02 | 0075ff81 |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._


### Large Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 144.28 ± 9.17 | 19336 | subtracted from Write tests |
| [hash-object.c][hash-object] | 353.45 ± 1.52 | 20016 | subtracted from Tree and Incremental tests |


_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including object generation code (which is included in all tests.)_



#### Write Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-write] | MessagePack | 307.27 ± 9.20 | 4088 | 3.13 ± 0.20 | b4ac134d |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-pack] | MessagePack | 347.14 ± 9.24 | 8616 | 3.41 ± 0.22 | b4ac134d |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-write] | MessagePack | 475.85 ± 9.22 | 5368 | 4.30 ± 0.27 | b4ac134d |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-pack] | MessagePack | 544.24 ± 11.32 | 4088 | 4.77 ± 0.31 | b4ac134d |
| [CMP][cmp] (v14) [(C)][cmp-write] | MessagePack | 660.19 ± 11.78 | 3232 | 5.58 ± 0.36 | b4ac134d |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-builder] | BSON | 715.26 ± 9.24 | 180952 | 5.96 ± 0.38 | 14d7b3b5 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-write] | UBJSON | 881.34 ± 9.26 | 3352 | 7.11 ± 0.45 | 12ead223 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-write] | UBJSON | 1007.45 ± 9.25 | 4184 | 7.98 ± 0.51 | 8f23e492 |
| [libbson][libbson] (1.3.0) [(C)][libbson-append] | BSON | 1048.95 ± 9.39 | 21112 | 8.27 ± 0.53 | 14d7b3b5 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-write] | JSON | 1097.71 ± 9.21 | 16904 | 8.61 ± 0.55 | a1ab8d87 |
| [Binn][binn] (b3e2c27) [(C)][binn-write] | Binn | 1687.91 ± 11.73 | 8216 | 12.70 ± 0.81 | 07910350 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-gen] | JSON | 3125.41 ± 26.25 | 6752 | 22.66 ± 1.45 | 11ddf727 |
| [json-builder][json-builder-lib] (19c739f) [(C)][json-builder] | JSON | 5032.13 ± 10.81 | 4560 | 35.88 ± 2.28 | 9061166e |
| [Jansson][jansson] (2.7) [(C)][jansson-dump] | JSON | 6540.87 ± 34.64 | 12528 | 46.33 ± 2.96 | 436d7f85 |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-dump] | JSON | 7152.87 ± 132.34 | 12704 | 50.58 ± 3.34 | 03e30c6f |



_The Time and Code Size columns show the net result after subtracting the hash-data time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-data. In all three columns, lower is better._



#### Tree Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 301.90 ± 2.20 | 3496 | 1.85 ± 0.01 | 91b5d7ef |
| [MPack][mpack] (0.8.1) [(C)][mpack-node] | MessagePack | 414.16 ± 2.34 | 5624 | 2.17 ± 0.01 | 91b5d7ef |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 453.91 ± 4.99 | 180280 | 2.28 ± 0.02 | 91b5d7ef |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 597.67 ± 2.91 | 4944 | 2.69 ± 0.01 | 91b5d7ef |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-unpack] | MessagePack | 750.87 ± 2.27 | 5200 | 3.12 ± 0.01 | 91b5d7ef |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-node] | MessagePack | 751.61 ± 2.04 | 6200 | 3.13 ± 0.01 | 91b5d7ef |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-unpack] | MessagePack | 1004.67 ± 2.95 | 19152 | 3.84 ± 0.02 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-dom] | JSON | 1129.46 ± 1.84 | 14600 | 4.20 ± 0.02 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-dom] | JSON | 1694.28 ± 2.37 | 15800 | 5.79 ± 0.03 | 91b5d7ef |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-read] | UBJSON | 3564.19 ± 24.68 | 10072 | 11.08 ± 0.08 | 91b5d7ef |
| [ubj][ubj] (f4d85c4) [(C)][ubj-read] | UBJSON | 4340.63 ± 23.33 | 10072 | 13.28 ± 0.09 | 91b5d7ef |
| [json-parser][json-parser-lib] (7053321) [(C)][json-parser] | JSON | 6437.37 ± 31.57 | 8288 | 19.21 ± 0.12 | 91b5d7ef |
| [YAJL][yajl] (2.1.0) [(C)][yajl-tree] | JSON | 8236.40 ± 73.39 | 22592 | 24.30 ± 0.23 | 91b5d7ef |
| [Jansson][jansson] (2.7) [(C)][jansson-load] | JSON | 9529.31 ± 36.55 | 14440 | 27.96 ± 0.16 | 5b6ba465 |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-load] | JSON | 9815.17 ± 28.36 | 14792 | 28.77 ± 0.15 | 91b5d7ef |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._



#### Incremental Parse Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-read] | MessagePack | 202.09 ± 2.43 | 5336 | 1.57 ± 0.01 | 91b5d7ef |
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 301.90 ± 2.20 | 3496 | 1.85 ± 0.01 | 91b5d7ef |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-read] | MessagePack | 319.70 ± 1.90 | 7552 | 1.90 ± 0.01 | 91b5d7ef |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 453.91 ± 4.99 | 180280 | 2.28 ± 0.02 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-sax] | JSON | 532.73 ± 2.11 | 6504 | 2.51 ± 0.01 | 91b5d7ef |
| [CMP][cmp] (v14) [(C)][cmp-read] | MessagePack | 533.46 ± 3.23 | 3440 | 2.51 ± 0.01 | 91b5d7ef |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-read] | MessagePack | 565.73 ± 2.36 | 5464 | 2.60 ± 0.01 | 91b5d7ef |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 597.67 ± 2.91 | 4944 | 2.69 ± 0.01 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-sax] | JSON | 914.88 ± 2.29 | 8688 | 3.59 ± 0.02 | 91b5d7ef |
| [YAJL][yajl] (2.1.0) [(C)][yajl-parse] | JSON | 1589.58 ± 3.31 | 12512 | 5.50 ± 0.03 | 91b5d7ef |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._



## Speed - Chromebook

These results are for an Acer C720 Chromebook. The tests are run on Arch Linux in recovery mode.


CPU model: Intel(R) Celeron(R) 2957U @ 1.40GHz  
Architecture: x86\_64  
Bogomips: 2794.95

### Small Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 3.44 ± 0.00 | 19336 | subtracted from Write tests |
| [hash-object.c][hash-object] | 3.88 ± 0.01 | 20016 | subtracted from Tree and Incremental tests |


_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including object generation code (which is included in all tests.)_



#### Write Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-write] | MessagePack | 2.19 ± 0.09 | 4088 | 1.64 ± 0.03 | 1a9c6681 |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-pack] | MessagePack | 3.44 ± 0.05 | 8616 | 2.00 ± 0.01 | 1a9c6681 |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-write] | MessagePack | 7.93 ± 0.15 | 5368 | 3.31 ± 0.04 | 1a9c6681 |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-pack] | MessagePack | 11.90 ± 0.13 | 4088 | 4.46 ± 0.04 | 1a9c6681 |
| [CMP][cmp] (v14) [(C)][cmp-write] | MessagePack | 14.01 ± 0.06 | 3232 | 5.08 ± 0.02 | 1a9c6681 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-builder] | BSON | 18.01 ± 0.10 | 180952 | 6.24 ± 0.03 | 4e02a1f9 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-write] | JSON | 18.50 ± 0.11 | 16904 | 6.38 ± 0.03 | e4495ccf |
| [ubj][ubj] (f4d85c4) [(C)][ubj-write] | UBJSON | 20.58 ± 0.06 | 3352 | 6.99 ± 0.02 | c1a19bb7 |
| [libbson][libbson] (1.3.0) [(C)][libbson-append] | BSON | 24.80 ± 0.10 | 21112 | 8.22 ± 0.03 | 4e02a1f9 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-write] | UBJSON | 25.03 ± 0.06 | 4184 | 8.29 ± 0.02 | 37f97981 |
| [Binn][binn] (b3e2c27) [(C)][binn-write] | Binn | 30.28 ± 0.29 | 8216 | 9.81 ± 0.08 | d7327409 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-gen] | JSON | 67.86 ± 0.47 | 6752 | 20.75 ± 0.14 | 4694ca58 |
| [json-builder][json-builder-lib] (19c739f) [(C)][json-builder] | JSON | 91.01 ± 1.20 | 4560 | 27.49 ± 0.35 | 75d4861f |
| [Jansson][jansson] (2.7) [(C)][jansson-dump] | JSON | 107.06 ± 0.25 | 12528 | 32.16 ± 0.07 | c6e55bf0 |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-dump] | JSON | 128.56 ± 2.45 | 12704 | 38.42 ± 0.71 | 97b6e2b2 |



_The Time and Code Size columns show the net result after subtracting the hash-data time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-data. In all three columns, lower is better._



#### Tree Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 4.49 ± 0.06 | 3496 | 2.16 ± 0.02 | 0075ff81 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 6.95 ± 0.07 | 180280 | 2.79 ± 0.02 | 0075ff81 |
| [MPack][mpack] (0.8.1) [(C)][mpack-node] | MessagePack | 8.93 ± 0.07 | 5624 | 3.30 ± 0.02 | 0075ff81 |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-unpack] | MessagePack | 10.34 ± 0.03 | 5200 | 3.66 ± 0.01 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-node] | MessagePack | 12.60 ± 0.03 | 6200 | 4.25 ± 0.01 | 0075ff81 |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-unpack] | MessagePack | 13.15 ± 0.03 | 19152 | 4.39 ± 0.01 | 0075ff81 |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 14.19 ± 0.02 | 4944 | 4.66 ± 0.01 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-dom] | JSON | 14.37 ± 0.05 | 14600 | 4.70 ± 0.02 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-dom] | JSON | 21.09 ± 0.08 | 15800 | 6.43 ± 0.03 | 0075ff81 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-read] | UBJSON | 72.80 ± 1.38 | 10072 | 19.76 ± 0.36 | 0075ff81 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-read] | UBJSON | 79.54 ± 0.37 | 10072 | 21.50 ± 0.11 | 0075ff81 |
| [json-parser][json-parser-lib] (7053321) [(C)][json-parser] | JSON | 120.19 ± 1.01 | 8288 | 31.98 ± 0.28 | 0075ff81 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-tree] | JSON | 165.11 ± 2.09 | 22592 | 43.55 ± 0.55 | 0075ff81 |
| [Jansson][jansson] (2.7) [(C)][jansson-load] | JSON | 179.48 ± 2.96 | 14440 | 47.26 ± 0.77 | 42bf8b2f |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-load] | JSON | 186.65 ± 1.58 | 14792 | 49.11 ± 0.43 | 0075ff81 |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._



#### Incremental Parse Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 4.49 ± 0.06 | 3496 | 2.16 ± 0.02 | 0075ff81 |
| [MPack][mpack] (0.8.1) [(C)][mpack-read] | MessagePack | 5.62 ± 0.02 | 5336 | 2.45 ± 0.01 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-sax] | JSON | 6.76 ± 0.02 | 6504 | 2.74 ± 0.01 | 0075ff81 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 6.95 ± 0.07 | 180280 | 2.79 ± 0.02 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-read] | MessagePack | 7.96 ± 0.05 | 7552 | 3.05 ± 0.02 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-read] | MessagePack | 8.15 ± 0.04 | 5464 | 3.10 ± 0.01 | 0075ff81 |
| [CMP][cmp] (v14) [(C)][cmp-read] | MessagePack | 11.55 ± 0.09 | 3440 | 3.98 ± 0.03 | 0075ff81 |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 14.19 ± 0.02 | 4944 | 4.66 ± 0.01 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-sax] | JSON | 14.53 ± 0.03 | 8688 | 4.75 ± 0.02 | 0075ff81 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-parse] | JSON | 34.08 ± 0.25 | 12512 | 9.78 ± 0.07 | 0075ff81 |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._


### Large Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 220.20 ± 0.00 | 19336 | subtracted from Write tests |
| [hash-object.c][hash-object] | 888.28 ± 0.22 | 20016 | subtracted from Tree and Incremental tests |


_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including object generation code (which is included in all tests.)_



#### Write Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-write] | MessagePack | 856.44 ± 1.19 | 4088 | 4.89 ± 0.01 | b4ac134d |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-pack] | MessagePack | 944.99 ± 1.73 | 8616 | 5.29 ± 0.01 | b4ac134d |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-write] | MessagePack | 1114.94 ± 0.89 | 5368 | 6.06 ± 0.00 | b4ac134d |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-pack] | MessagePack | 1338.33 ± 0.96 | 4088 | 7.08 ± 0.00 | b4ac134d |
| [CMP][cmp] (v14) [(C)][cmp-write] | MessagePack | 1463.82 ± 1.64 | 3232 | 7.65 ± 0.01 | b4ac134d |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-builder] | BSON | 1528.28 ± 1.98 | 180952 | 7.94 ± 0.01 | 14d7b3b5 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-write] | UBJSON | 1844.43 ± 2.97 | 3352 | 9.38 ± 0.01 | 12ead223 |
| [libbson][libbson] (1.3.0) [(C)][libbson-append] | BSON | 2003.89 ± 6.16 | 21112 | 10.10 ± 0.03 | 14d7b3b5 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-write] | UBJSON | 2117.18 ± 2.66 | 4184 | 10.61 ± 0.01 | 8f23e492 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-write] | JSON | 2136.34 ± 1.11 | 16904 | 10.70 ± 0.01 | a1ab8d87 |
| [Binn][binn] (b3e2c27) [(C)][binn-write] | Binn | 3492.20 ± 7.97 | 8216 | 16.86 ± 0.04 | 07910350 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-gen] | JSON | 6091.83 ± 25.31 | 6752 | 28.66 ± 0.11 | 11ddf727 |
| [json-builder][json-builder-lib] (19c739f) [(C)][json-builder] | JSON | 11446.17 ± 489.65 | 4560 | 52.98 ± 2.22 | 9061166e |
| [Jansson][jansson] (2.7) [(C)][jansson-dump] | JSON | 12823.71 ± 195.06 | 12528 | 59.24 ± 0.89 | 71b6803d |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-dump] | JSON | 14258.16 ± 222.04 | 12704 | 65.75 ± 1.01 | 03e30c6f |



_The Time and Code Size columns show the net result after subtracting the hash-data time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-data. In all three columns, lower is better._



#### Tree Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 512.67 ± 1.18 | 3496 | 1.58 ± 0.00 | 91b5d7ef |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 618.37 ± 1.92 | 180280 | 1.70 ± 0.00 | 91b5d7ef |
| [MPack][mpack] (0.8.1) [(C)][mpack-node] | MessagePack | 819.70 ± 0.98 | 5624 | 1.92 ± 0.00 | 91b5d7ef |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-unpack] | MessagePack | 1264.30 ± 2.03 | 5200 | 2.42 ± 0.00 | 91b5d7ef |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-node] | MessagePack | 1319.61 ± 1.94 | 6200 | 2.49 ± 0.00 | 91b5d7ef |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 1320.63 ± 0.83 | 4944 | 2.49 ± 0.00 | 91b5d7ef |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-unpack] | MessagePack | 1852.49 ± 1.76 | 19152 | 3.09 ± 0.00 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-dom] | JSON | 2135.78 ± 3.23 | 14600 | 3.40 ± 0.00 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-dom] | JSON | 3148.26 ± 8.94 | 15800 | 4.54 ± 0.01 | 91b5d7ef |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-read] | UBJSON | 6605.73 ± 14.05 | 10072 | 8.44 ± 0.02 | 91b5d7ef |
| [ubj][ubj] (f4d85c4) [(C)][ubj-read] | UBJSON | 8497.19 ± 89.18 | 10072 | 10.57 ± 0.10 | 91b5d7ef |
| [json-parser][json-parser-lib] (7053321) [(C)][json-parser] | JSON | 12027.21 ± 29.90 | 8288 | 14.54 ± 0.03 | 91b5d7ef |
| [YAJL][yajl] (2.1.0) [(C)][yajl-tree] | JSON | 17193.97 ± 21.95 | 22592 | 20.36 ± 0.03 | 91b5d7ef |
| [Jansson][jansson] (2.7) [(C)][jansson-load] | JSON | 18217.32 ± 19.85 | 14440 | 21.51 ± 0.02 | 7eeb34f1 |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-load] | JSON | 19240.90 ± 409.69 | 14792 | 22.66 ± 0.46 | 91b5d7ef |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._



#### Incremental Parse Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-read] | MessagePack | 407.82 ± 1.17 | 5336 | 1.46 ± 0.00 | 91b5d7ef |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-read] | MessagePack | 495.80 ± 0.49 | 7552 | 1.56 ± 0.00 | 91b5d7ef |
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 512.67 ± 1.18 | 3496 | 1.58 ± 0.00 | 91b5d7ef |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 618.37 ± 1.92 | 180280 | 1.70 ± 0.00 | 91b5d7ef |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-read] | MessagePack | 844.49 ± 3.80 | 5464 | 1.95 ± 0.00 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-sax] | JSON | 883.71 ± 1.44 | 6504 | 1.99 ± 0.00 | 91b5d7ef |
| [CMP][cmp] (v14) [(C)][cmp-read] | MessagePack | 899.51 ± 1.34 | 3440 | 2.01 ± 0.00 | 91b5d7ef |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 1320.63 ± 0.83 | 4944 | 2.49 ± 0.00 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-sax] | JSON | 1699.81 ± 7.35 | 8688 | 2.91 ± 0.01 | 91b5d7ef |
| [YAJL][yajl] (2.1.0) [(C)][yajl-parse] | JSON | 2822.65 ± 6.66 | 12512 | 4.18 ± 0.01 | 91b5d7ef |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._


## Speed - Raspberry Pi

These results are for a speed-optimized build (`-O3 -flto`) on a first-generation Raspberry Pi, running a fresh install of Arch Linux ARM.

CPU model: ARMv6-compatible processor rev 7 (v6l)  
Hardware Revision: 000f (Model B rev2, 512 MB RAM)


### Small Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 30.31 ± 0.03 | 16112 | subtracted from Write tests |
| [hash-object.c][hash-object] | 36.07 ± 0.15 | 17104 | subtracted from Tree and Incremental tests |


_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including object generation code (which is included in all tests.)_



#### Write Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-write] | MessagePack | 38.31 ± 0.18 | 4032 | 2.26 ± 0.01 | 1a9c6681 |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-pack] | MessagePack | 38.36 ± 0.41 | 7628 | 2.27 ± 0.01 | 1a9c6681 |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-pack] | MessagePack | 69.21 ± 0.37 | 4384 | 3.28 ± 0.01 | 1a9c6681 |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-write] | MessagePack | 81.95 ± 0.37 | 5440 | 3.70 ± 0.01 | 1a9c6681 |
| [CMP][cmp] (v14) [(C)][cmp-write] | MessagePack | 89.75 ± 0.11 | 3396 | 3.96 ± 0.01 | 1a9c6681 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-write] | UBJSON | 159.36 ± 0.38 | 3664 | 6.26 ± 0.01 | c1a19bb7 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-builder] | BSON | 188.51 ± 0.26 | 160444 | 7.22 ± 0.01 | 4e02a1f9 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-write] | UBJSON | 194.92 ± 0.29 | 4448 | 7.43 ± 0.01 | 37f97981 |
| [libbson][libbson] (1.3.0) [(C)][libbson-append] | BSON | 229.45 ± 0.41 | 17604 | 8.57 ± 0.02 | 4e02a1f9 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-write] | JSON | 231.33 ± 1.10 | 18124 | 8.63 ± 0.04 | e4495ccf |
| [Binn][binn] (b3e2c27) [(C)][binn-write] | Binn | 508.11 ± 8.82 | 8688 | 17.76 ± 0.29 | d7327409 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-gen] | JSON | 752.41 ± 12.40 | 6840 | 25.82 ± 0.41 | 4694ca58 |
| [json-builder][json-builder-lib] (19c739f) [(C)][json-builder] | JSON | 1539.33 ± 11.29 | 4392 | 51.78 ± 0.38 | 75d4861f |
| [Jansson][jansson] (2.7) [(C)][jansson-dump] | JSON | 1789.29 ± 12.16 | 14884 | 60.03 ± 0.40 | eb946daf |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-dump] | JSON | 2089.40 ± 21.09 | 15344 | 69.93 ± 0.70 | 97b6e2b2 |



_The Time and Code Size columns show the net result after subtracting the hash-data time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-data. In all three columns, lower is better._



#### Tree Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 63.48 ± 0.36 | 3876 | 2.76 ± 0.01 | 0075ff81 |
| [MPack][mpack] (0.8.1) [(C)][mpack-node] | MessagePack | 69.05 ± 0.55 | 5612 | 2.91 ± 0.02 | 0075ff81 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 84.01 ± 0.38 | 159648 | 3.33 ± 0.02 | 0075ff81 |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-unpack] | MessagePack | 87.54 ± 0.37 | 5884 | 3.43 ± 0.02 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-node] | MessagePack | 99.39 ± 0.44 | 6212 | 3.76 ± 0.02 | 0075ff81 |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 141.18 ± 0.70 | 4496 | 4.91 ± 0.03 | 0075ff81 |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-unpack] | MessagePack | 142.66 ± 0.50 | 17300 | 4.96 ± 0.02 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-dom] | JSON | 170.34 ± 0.38 | 13684 | 5.72 ± 0.03 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-dom] | JSON | 245.59 ± 0.89 | 18440 | 7.81 ± 0.04 | 0075ff81 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-read] | UBJSON | 1137.63 ± 5.15 | 8836 | 32.54 ± 0.19 | 0075ff81 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-read] | UBJSON | 1491.61 ± 23.82 | 8844 | 42.36 ± 0.68 | 0075ff81 |
| [json-parser][json-parser-lib] (7053321) [(C)][json-parser] | JSON | 1751.27 ± 9.72 | 7316 | 49.56 ± 0.34 | 0075ff81 |
| [Jansson][jansson] (2.7) [(C)][jansson-load] | JSON | 2604.03 ± 31.41 | 16848 | 73.20 ± 0.92 | e5cef7b9 |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-load] | JSON | 2795.44 ± 87.16 | 17084 | 78.51 ± 2.44 | 0075ff81 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-tree] | JSON | 3416.81 ± 26.22 | 18632 | 95.74 ± 0.82 | 0075ff81 |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._



#### Incremental Parse Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-read] | MessagePack | 29.45 ± 0.29 | 5460 | 1.82 ± 0.01 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-read] | MessagePack | 57.81 ± 0.23 | 5460 | 2.60 ± 0.01 | 0075ff81 |
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 63.48 ± 0.36 | 3876 | 2.76 ± 0.01 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-read] | MessagePack | 66.11 ± 0.35 | 6772 | 2.83 ± 0.01 | 0075ff81 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 84.01 ± 0.38 | 159648 | 3.33 ± 0.02 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-sax] | JSON | 99.28 ± 0.26 | 6368 | 3.75 ± 0.02 | 0075ff81 |
| [CMP][cmp] (v14) [(C)][cmp-read] | MessagePack | 100.14 ± 0.22 | 3316 | 3.78 ± 0.02 | 0075ff81 |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 141.18 ± 0.70 | 4496 | 4.91 ± 0.03 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-sax] | JSON | 145.68 ± 0.27 | 11416 | 5.04 ± 0.02 | 0075ff81 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-parse] | JSON | 349.72 ± 2.62 | 11820 | 10.70 ± 0.08 | 0075ff81 |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._


### Large Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 3956.91 ± 0.33 | 16112 | subtracted from Write tests |
| [hash-object.c][hash-object] | 6161.16 ± 1.20 | 17104 | subtracted from Tree and Incremental tests |


_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including object generation code (which is included in all tests.)_



#### Write Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-write] | MessagePack | 5610.92 ± 9.58 | 4032 | 2.42 ± 0.00 | b4ac134d |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-pack] | MessagePack | 6098.33 ± 8.50 | 7628 | 2.54 ± 0.00 | b4ac134d |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-pack] | MessagePack | 8142.64 ± 7.78 | 4384 | 3.06 ± 0.00 | b4ac134d |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-write] | MessagePack | 8241.74 ± 9.81 | 5440 | 3.08 ± 0.00 | b4ac134d |
| [CMP][cmp] (v14) [(C)][cmp-write] | MessagePack | 9206.05 ± 11.85 | 3396 | 3.33 ± 0.00 | b4ac134d |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-write] | JSON | 15235.80 ± 8.42 | 18124 | 4.85 ± 0.00 | a1ab8d87 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-write] | UBJSON | 15446.15 ± 62.05 | 3664 | 4.90 ± 0.02 | 12ead223 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-builder] | BSON | 15984.76 ± 16.45 | 160444 | 5.04 ± 0.00 | 14d7b3b5 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-write] | UBJSON | 16971.06 ± 58.26 | 4448 | 5.29 ± 0.01 | 8f23e492 |
| [libbson][libbson] (1.3.0) [(C)][libbson-append] | BSON | 18287.50 ± 31.43 | 17604 | 5.62 ± 0.01 | 14d7b3b5 |
| [Binn][binn] (b3e2c27) [(C)][binn-write] | Binn | 39203.27 ± 309.77 | 8688 | 10.91 ± 0.08 | 07910350 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-gen] | JSON | 61838.05 ± 1453.54 | 6840 | 16.63 ± 0.37 | 11ddf727 |
| [json-builder][json-builder-lib] (19c739f) [(C)][json-builder] | JSON | 145811.52 ± 384.95 | 4392 | 37.85 ± 0.10 | 9061166e |
| [Jansson][jansson] (2.7) [(C)][jansson-dump] | JSON | 151609.98 ± 828.70 | 14884 | 39.32 ± 0.21 | e766b568 |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-dump] | JSON | 166935.57 ± 2095.52 | 15344 | 43.19 ± 0.53 | 03e30c6f |



_The Time and Code Size columns show the net result after subtracting the hash-data time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-data. In all three columns, lower is better._



#### Tree Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 2745.93 ± 1.83 | 3876 | 1.45 ± 0.00 | 91b5d7ef |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 4315.50 ± 2.12 | 159648 | 1.70 ± 0.00 | 91b5d7ef |
| [MPack][mpack] (0.8.1) [(C)][mpack-node] | MessagePack | 6316.98 ± 33.60 | 5612 | 2.03 ± 0.01 | 91b5d7ef |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 7053.49 ± 20.29 | 4496 | 2.14 ± 0.00 | 91b5d7ef |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-node] | MessagePack | 9447.64 ± 23.81 | 6212 | 2.53 ± 0.00 | 91b5d7ef |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-unpack] | MessagePack | 10287.89 ± 25.04 | 5884 | 2.67 ± 0.00 | 91b5d7ef |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-unpack] | MessagePack | 13581.87 ± 16.42 | 17300 | 3.20 ± 0.00 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-dom] | JSON | 17504.30 ± 18.48 | 13684 | 3.84 ± 0.00 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-dom] | JSON | 24390.81 ± 26.50 | 18440 | 4.96 ± 0.00 | 91b5d7ef |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-read] | UBJSON | 71491.44 ± 183.08 | 8836 | 12.60 ± 0.03 | 91b5d7ef |
| [ubj][ubj] (f4d85c4) [(C)][ubj-read] | UBJSON | 94270.72 ± 712.07 | 8844 | 16.30 ± 0.12 | 91b5d7ef |
| [json-parser][json-parser-lib] (7053321) [(C)][json-parser] | JSON | 131013.22 ± 236.49 | 7316 | 22.26 ± 0.04 | 91b5d7ef |
| [Jansson][jansson] (2.7) [(C)][jansson-load] | JSON | 187014.94 ± 4164.80 | 16848 | 31.35 ± 0.68 | 2a98f785 |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-load] | JSON | 197942.32 ± 6581.88 | 17084 | 33.13 ± 1.07 | 91b5d7ef |
| [YAJL][yajl] (2.1.0) [(C)][yajl-tree] | JSON | 227495.40 ± 4909.91 | 18632 | 37.92 ± 0.80 | 91b5d7ef |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._



#### Incremental Parse Test

| Library | Format | Time<br>(μs) ▲ | Code Size<br>(bytes) | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [MPack][mpack] (0.8.1) [(C)][mpack-read] | MessagePack | 361.43 ± 35.24 | 5460 | 1.06 ± 0.01 | 91b5d7ef |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-read] | MessagePack | 2627.60 ± 38.45 | 6772 | 1.43 ± 0.01 | 91b5d7ef |
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 2745.93 ± 1.83 | 3876 | 1.45 ± 0.00 | 91b5d7ef |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-read] | MessagePack | 3335.67 ± 34.95 | 5460 | 1.54 ± 0.01 | 91b5d7ef |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 4315.50 ± 2.12 | 159648 | 1.70 ± 0.00 | 91b5d7ef |
| [CMP][cmp] (v14) [(C)][cmp-read] | MessagePack | 5713.58 ± 27.30 | 3316 | 1.93 ± 0.00 | 91b5d7ef |
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 7053.49 ± 20.29 | 4496 | 2.14 ± 0.00 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-sax] | JSON | 7387.73 ± 12.12 | 6368 | 2.20 ± 0.00 | 91b5d7ef |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-sax] | JSON | 11792.55 ± 5.21 | 11416 | 2.91 ± 0.00 | 91b5d7ef |
| [YAJL][yajl] (2.1.0) [(C)][yajl-parse] | JSON | 24252.07 ± 234.90 | 11820 | 4.94 ± 0.04 | 91b5d7ef |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._


## Size - Raspberry Pi

These results are for a size-optimized build (`-Os -flto`) on a first-generation Raspberry Pi, running a fresh install of Arch Linux ARM.

For brevity we are only showing the results for a Raspberry Pi with small data, since you probably aren't worried about size optimization on desktops and servers, or parsing large data files on a Raspberry Pi.

The tables are sorted by code size.


### Small Data


#### Hash Comparisons

| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |
|----|---:|---:|----|
| [hash-data.c][hash-data] | 37.29 ± 0.03 | 9036 | subtracted from Write tests |
| [hash-object.c][hash-object] | 43.94 ± 0.34 | 9488 | subtracted from Tree and Incremental tests |


_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including object generation code (which is included in all tests.)_



#### Write Test

| Library | Format | Time<br>(μs) | Code Size<br>(bytes) ▲ | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-pack] | MessagePack | 67.35 ± 0.19 | 1528 | 2.81 ± 0.01 | 1a9c6681 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-write] | UBJSON | 189.24 ± 0.50 | 1776 | 6.07 ± 0.01 | c1a19bb7 |
| [CMP][cmp] (v14) [(C)][cmp-write] | MessagePack | 100.66 ± 0.26 | 2324 | 3.70 ± 0.01 | 1a9c6681 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-write] | UBJSON | 240.83 ± 0.15 | 2352 | 7.46 ± 0.01 | 37f97981 |
| [MPack][mpack] (0.8.1) [(C)][mpack-write] | MessagePack | 38.90 ± 0.21 | 3124 | 2.04 ± 0.01 | 1a9c6681 |
| [json-builder][json-builder-lib] (19c739f) [(C)][json-builder] | JSON | 1551.66 ± 12.47 | 3176 | 42.61 ± 0.34 | 75d4861f |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-pack] | MessagePack | 68.38 ± 0.21 | 3324 | 2.83 ± 0.01 | 1a9c6681 |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-write] | MessagePack | 107.05 ± 0.42 | 3960 | 3.87 ± 0.01 | 1a9c6681 |
| [Binn][binn] (b3e2c27) [(C)][binn-write] | Binn | 516.18 ± 5.71 | 4432 | 14.84 ± 0.15 | d7327409 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-gen] | JSON | 757.12 ± 18.04 | 6012 | 21.30 ± 0.48 | 4694ca58 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-write] | JSON | 345.26 ± 0.74 | 8328 | 10.26 ± 0.02 | e4495ccf |
| [Jansson][jansson] (2.7) [(C)][jansson-dump] | JSON | 1883.20 ± 17.47 | 10576 | 51.50 ± 0.47 | ab18ef6f |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-dump] | JSON | 2207.44 ± 29.75 | 10876 | 60.19 ± 0.80 | 97b6e2b2 |
| [libbson][libbson] (1.3.0) [(C)][libbson-append] | BSON | 264.93 ± 1.11 | 14192 | 8.10 ± 0.03 | 4e02a1f9 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-builder] | BSON | 270.70 ± 1.23 | 160500 | 8.26 ± 0.03 | 4e02a1f9 |



_The Time and Code Size columns show the net result after subtracting the hash-data time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-data. In all three columns, lower is better._



#### Tree Test

| Library | Format | Time<br>(μs) | Code Size<br>(bytes) ▲ | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 175.29 ± 0.44 | 3008 | 4.99 ± 0.04 | 0075ff81 |
| [MPack][mpack] (0.8.1) [(C)][mpack-node] | MessagePack | 90.79 ± 0.43 | 3192 | 3.07 ± 0.02 | 0075ff81 |
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 84.31 ± 0.42 | 3252 | 2.92 ± 0.02 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-node] | MessagePack | 127.18 ± 0.89 | 3748 | 3.89 ± 0.04 | 0075ff81 |
| [ubj][ubj] (f4d85c4) \[optimized] [(C)][ubj-opt-read] | UBJSON | 1241.77 ± 15.03 | 3844 | 29.26 ± 0.41 | 0075ff81 |
| [ubj][ubj] (f4d85c4) [(C)][ubj-read] | UBJSON | 1637.14 ± 25.18 | 3852 | 38.26 ± 0.64 | 0075ff81 |
| [msgpack C][msgpack] (1.3.0) [(C)][msgpack-c-unpack] | MessagePack | 107.23 ± 0.57 | 3948 | 3.44 ± 0.03 | 0075ff81 |
| [json-parser][json-parser-lib] (7053321) [(C)][json-parser] | JSON | 1772.10 ± 2.94 | 6552 | 41.33 ± 0.32 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-dom] | JSON | 504.28 ± 0.71 | 9076 | 12.48 ± 0.10 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-dom] | JSON | 322.89 ± 0.74 | 9236 | 8.35 ± 0.07 | 0075ff81 |
| [msgpack C++][msgpack] (1.3.0) [(C++)][msgpack-cpp-unpack] | MessagePack | 175.38 ± 0.55 | 11044 | 4.99 ± 0.04 | 0075ff81 |
| [Jansson][jansson] (2.7) [(C)][jansson-load] | JSON | 2822.15 ± 51.68 | 13476 | 65.23 ± 1.28 | 8eaff739 |
| [Jansson][jansson] (2.7) \[ordered] [(C)][jansson-ordered-load] | JSON | 2895.82 ± 8.30 | 13680 | 66.90 ± 0.55 | 0075ff81 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-tree] | JSON | 3339.01 ± 72.53 | 13972 | 76.99 ± 1.75 | 0075ff81 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 89.95 ± 0.40 | 161096 | 3.05 ± 0.02 | 0075ff81 |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._



#### Incremental Parse Test

| Library | Format | Time<br>(μs) | Code Size<br>(bytes) ▲ | Time<br>Overhead | Hash |
|----|----|---:|---:|---:|---:|
| [Binn][binn] (b3e2c27) [(C)][binn-load] | Binn | 175.29 ± 0.44 | 3008 | 4.99 ± 0.04 | 0075ff81 |
| [CMP][cmp] (v14) [(C)][cmp-read] | MessagePack | 107.18 ± 0.34 | 3192 | 3.44 ± 0.03 | 0075ff81 |
| [MPack][mpack] (0.8.1) [(C)][mpack-read] | MessagePack | 87.52 ± 0.38 | 3208 | 2.99 ± 0.02 | 0075ff81 |
| [libbson][libbson] (1.3.0) [(C)][libbson-iter] | BSON | 84.31 ± 0.42 | 3252 | 2.92 ± 0.02 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[UTF-8] [(C)][mpack-utf8-read] | MessagePack | 122.76 ± 0.44 | 3544 | 3.79 ± 0.03 | 0075ff81 |
| [MPack][mpack] (0.8.1) \[tracking] [(C)][mpack-tracking-read] | MessagePack | 155.88 ± 0.42 | 3924 | 4.55 ± 0.04 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) \[in-situ] [(C++)][rapidjson-insitu-sax] | JSON | 207.46 ± 0.36 | 4284 | 5.72 ± 0.04 | 0075ff81 |
| [RapidJSON][rapidjson] (1.0.2) [(C++)][rapidjson-sax] | JSON | 359.65 ± 0.45 | 4920 | 9.19 ± 0.07 | 0075ff81 |
| [YAJL][yajl] (2.1.0) [(C)][yajl-parse] | JSON | 376.31 ± 2.85 | 10180 | 9.56 ± 0.10 | 0075ff81 |
| [MongoDB Legacy][mongo-cxx] (1.1.0) [(C++)][mongo-cxx-obj] | BSON | 89.95 ± 0.40 | 161096 | 3.05 ± 0.02 | 0075ff81 |



_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._


