#!/usr/bin/env python3

# Copyright (c) 2015 Nicholas Fraser
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import csv, sys
from functools import reduce
from math import sqrt

benchmarks_url = 'https://github.com/ludocode/schemaless-benchmarks/blob/master/'

urlrefs = {
    "mpack": "https://github.com/ludocode/mpack",
    "cmp": "https://github.com/camgunz/cmp",
    "msgpack": "https://github.com/msgpack/msgpack-c",
    "rapidjson": "http://rapidjson.org/",
    "yajl": "http://lloyd.github.io/yajl/",
    "libbson": "https://github.com/mongodb/libbson",
    "binn": "https://github.com/liteserver/binn",
    "jansson": "http://www.digip.org/jansson/",
    "json-parser-lib": "https://github.com/udp/json-parser",
    "json-builder-lib": "https://github.com/udp/json-builder",
    "ubj": "https://github.com/Steve132/ubj",
    "mongo-cxx": "https://github.com/mongodb/mongo-cxx-driver",
}

info = { # fullname, urlref, config
    "mpack-": ["MPack", "mpack", ""],
    "mpack-tracking-": ["MPack", "mpack", " \\[tracking]"],
    "mpack-utf8-": ["MPack", "mpack", " \\[UTF-8]"],
    "cmp-": ["CMP", "cmp", ""],
    "msgpack-c-": ["msgpack C", "msgpack", ""],
    "msgpack-cpp-": ["msgpack C++", "msgpack", ""],
    "rapidjson-": ["RapidJSON", "rapidjson", ""],
    "rapidjson-insitu-": ["RapidJSON", "rapidjson", " \\[in-situ]"],
    "yajl-": ["YAJL", "yajl", ""],
    "libbson-": ["libbson", "libbson", ""],
    "binn-": ["Binn", "binn", ""],
    "jansson-": ["Jansson", "jansson", ""],
    "jansson-ordered-": ["Jansson", "jansson", " \\[ordered]"],
    "json-parser": ["json-parser", "json-parser-lib", ""],
    "json-builder": ["json-builder", "json-builder-lib", ""],
    "ubj-": ["ubj", "ubj", ""],
    "ubj-opt-": ["ubj", "ubj", " \\[optimized]"],
    "mongo-cxx-": ["MongoDB Legacy", "mongo-cxx", ""],
}

size_results = len(sys.argv) > 1 and sys.argv[1] == "size"
extended     = len(sys.argv) > 2 and sys.argv[2] == "extended"

show_overhead = extended
show_language = extended
show_hash = extended
show_stdev = extended
show_benchmark = not extended

if show_overhead:
    hash_footnote = """
_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including object generation code (which is included in all tests.)_
"""
    write_footnote = """
_The Time and Code Size columns show the net result after subtracting the hash-data time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-data. In all three columns, lower is better._
"""
    read_footnote = """
_The Time and Code Size columns show the net result after subtracting the hash-object time and size. The Time Overhead column shows the total time of the benchmark divided by the total time of hash-object. In all three columns, lower is better._
"""
else:
    hash_footnote = """
_The Time column shows the total time taken to hash the expected output of the library in the test (the expected objects for Tree and Incremental tests, or a chunk of bytes roughly the size of encoded data for the Write tests.) The Code Size column shows the total code size of the benchmark harness, including hashing and object generation code (which is included in all tests.)_
"""
    write_footnote = """
_The Time and Code Size columns show the net result after subtracting the hash-data time and size. In both columns, lower is better._
"""
    read_footnote = """
_The Time and Code Size columns show the net result after subtracting the hash-object time and size. In both columns, lower is better._
"""

csvname = 'results.csv'

NAME, LANGUAGE, VERSION, FILE, FORMAT, OBJECT_SIZE, TIME, BINARY_SIZE, SIZE_OPTIMIZED, HASH = range(10)

# data[size][name]
data = {}
for i in range(1,6):
    data[i] = {}

# collect data in csv
with open(csvname) as csvfile:
    reader = csv.reader(csvfile)
    for row in reader:
        if not size_results == int(row[SIZE_OPTIMIZED]):
            continue

        sizedata = data[int(row[OBJECT_SIZE])]
        name = row[NAME]

        # add the row once, but replace the time with a list containing all times found
        if name in sizedata:
            if sizedata[name][BINARY_SIZE] != row[BINARY_SIZE]:
                raise Exception("row code size does not match! did you 'make clean'?\nnew row: " +
                        str(row) + "\nexisting row: " + str(sizedata[name]))
            sizedata[name][TIME].append(float(row[TIME]))
        else:
            row[TIME] = [float(row[TIME])]
            sizedata[name] = row

def printheader(test):
    print()
    print(test)
    print()
    header = '| Library |'
    divider = '|----|'
    if show_benchmark:
        header += ' Benchmark |'
        divider += '----|'

    header += ' Format | Time<br>(μs)%s | Code Size<br>(bytes)%s |'
    header = header % (size_results and ("", " ▲") or (" ▲", ""))
    divider += '----|---:|---:|'

    if show_overhead:
        header += ' Time<br>Overhead |'
        divider += '---:|'
    if show_hash:
        header += ' Hash |'
        divider += '---:|'

    print(header)
    print(divider)

def rowtime(row):

    # copy list
    times = list(row[TIME])

    # drop highest and lowest
    if len(times) > 6:
        times.remove(max(times))
        times.remove(min(times))

    # calculate mean and stdev
    count = len(times)
    mean = sum(times) / count
    if count > 1:
        sumsqr = reduce(lambda x, y: x + pow(y - mean, 2), times, 0)
        stdev = sqrt(sumsqr / (count - 1))
    else:
        stdev = mean

    return mean, stdev

def rowstring(net, stdev):
    if show_stdev:
        ## pad the stdev with U+2007 FIGURE SPACE to better align results
        #stdevstr = '%.2f' % stdev
        #return '%.2f %s± %s' % (net, '\u2007' * (5 - len(stdevstr)), stdevstr)
        return '%.2f ± %.2f' % (net, stdev)
    return '%.2f' % net

def rowtimestr(row, sub):
    time, timedev = rowtime(row)
    subtime, subdev = rowtime(sub)
    net = time - subtime
    stdev = sqrt(pow(timedev, 2) + pow(subdev, 2))
    return rowstring(net, stdev)

def rowoverhead(row, sub):
    time, timedev = rowtime(row)
    subtime, subdev = rowtime(sub)
    overhead = time / subtime
    stdev = overhead * sqrt(pow(timedev / time, 2) + pow(subdev / subtime, 2))
    return rowstring(overhead, stdev)

def addrow(rows, sizedata, name, write):
    if name not in sizedata:
        return
    row = sizedata[name]
    sub = sizedata[write and "hash-data" or "hash-object"]

    size = int(row[BINARY_SIZE]) - int(sub[BINARY_SIZE])

    time = rowtime(row)[0]
    timestr = rowtimestr(row, sub)
    overheadstr = rowoverhead(row, sub)

    filename = row[FILE].split('/')[-1]
    version = row[VERSION]

    fullname = row[NAME]
    urlref = ""
    config = ""
    match = 0
    for key, value in info.items():
        if row[NAME].startswith(key) and len(key) > match:
            match = len(key)
            fullname = value[0]
            urlref = value[1]
            config = value[2]

    language = ""
    if show_language:
        language = row[LANGUAGE]

    if show_benchmark:
        p = '| [%s][%s] (%s)%s | [%s][%s]%s |' % (fullname, urlref, version, config, filename, name, show_language and (" " + language) or "")
    else:
        p = '| [%s][%s] (%s)%s [(%s)][%s] |' % (fullname, urlref, version, config, language, name)

    p += ' %s | %s | %i |' % \
            (row[FORMAT], timestr, size)
    if show_overhead:
        p += ' %s |' % overheadstr
    if show_hash:
        p += ' %s |' % row[HASH]
    rows.append([size_results and size or time, p])

def printrows(rows):
    for row in sorted(rows):
        print(row[1])
    print()

def printhashrow(sizedata, name, desc, purpose):
    row = sizedata[name]
    time = rowstring(*rowtime(row))
    size = int(row[BINARY_SIZE])
    filename = row[FILE].split('/')[-1]
    print('| [%s][%s] | %s | %i | %s |' % (filename, name, time, size, purpose))

# print link refs
print()
for name, url in urlrefs.items():
    print("[%s]: %s" % (name, url))
print()
printed = []
for size in range(1,6):
    sizedata = data[size]
    if len(sizedata) == 0:
        continue
    for name, row in sizedata.items():
        if name in printed:
            continue
        printed.append(name)
        print("[%s]: %s" % (name, benchmarks_url + row[FILE]))
print()

for size in range(1,6):
    sizedata = data[size]
    if len(sizedata) == 0:
        continue

    if size == 1:
        print("## Smallest Data")
    if size == 2:
        print("## Small Data")
    if size == 3:
        print("## Medium Data")
    if size == 4:
        print("## Large Data")
    if size == 5:
        print("## Largest Data")
    print()

    print()
    print("### Hash Comparisons")
    print()
    print('| Benchmark | Time<br>(μs) | Code Size<br>(bytes) | Comparison |')
    print('|----|---:|---:|----|')
    printhashrow(sizedata, 'hash-data', 'Data Hash', 'subtracted from Write tests')
    printhashrow(sizedata, 'hash-object', 'Object Hash', 'subtracted from Tree and Incremental tests')
    print()
    print(hash_footnote)
    print()

    rows = []
    addrow(rows, sizedata, 'mpack-write', True)
    addrow(rows, sizedata, 'cmp-write', True)
    addrow(rows, sizedata, 'msgpack-cpp-pack', True)
    addrow(rows, sizedata, 'msgpack-c-pack', True)
    addrow(rows, sizedata, 'rapidjson-write', True)
    addrow(rows, sizedata, 'yajl-gen', True)
    addrow(rows, sizedata, 'jansson-dump', True)
    addrow(rows, sizedata, 'libbson-append', True)
    addrow(rows, sizedata, 'mongo-cxx-builder', True)
    if extended:
        addrow(rows, sizedata, 'binn-write', True)
        addrow(rows, sizedata, 'json-builder', True)
        addrow(rows, sizedata, 'ubj-write', True)
        addrow(rows, sizedata, 'ubj-opt-write', True)
        addrow(rows, sizedata, 'mpack-tracking-write', True)
        addrow(rows, sizedata, 'jansson-ordered-dump', True)
    if len(rows) > 0:
        printheader('### Write Test')
        printrows(rows)
        print()
        print(write_footnote)
        print()

    rows = []
    addrow(rows, sizedata, 'mpack-node', False)
    addrow(rows, sizedata, 'msgpack-c-unpack', False)
    addrow(rows, sizedata, 'msgpack-cpp-unpack', False)
    addrow(rows, sizedata, 'rapidjson-insitu-dom', False)
    addrow(rows, sizedata, 'yajl-tree', False)
    addrow(rows, sizedata, 'jansson-load', False)
    addrow(rows, sizedata, 'libbson-iter', False)
    addrow(rows, sizedata, 'mongo-cxx-obj', False)
    if extended:
        addrow(rows, sizedata, 'binn-load', False)
        addrow(rows, sizedata, 'json-parser', False)
        addrow(rows, sizedata, 'mpack-utf8-node', False)
        addrow(rows, sizedata, 'rapidjson-dom', False)
        addrow(rows, sizedata, 'jansson-ordered-load', False)
        addrow(rows, sizedata, 'ubj-read', False)
        addrow(rows, sizedata, 'ubj-opt-read', False)
    if len(rows) > 0:
        printheader('### Tree Test')
        printrows(rows)
        print()
        print(read_footnote)
        print()

    rows = []
    addrow(rows, sizedata, 'mpack-read', False)
    addrow(rows, sizedata, 'cmp-read', False)
    addrow(rows, sizedata, 'rapidjson-insitu-sax', False)
    addrow(rows, sizedata, 'yajl-parse', False)
    addrow(rows, sizedata, 'libbson-iter', False)
    addrow(rows, sizedata, 'mongo-cxx-obj', False)
    if extended:
        addrow(rows, sizedata, 'binn-load', False)
        addrow(rows, sizedata, 'mpack-utf8-read', False)
        addrow(rows, sizedata, 'mpack-tracking-read', False)
        addrow(rows, sizedata, 'rapidjson-sax', False)
    if len(rows) > 0:
        printheader('### Incremental Parse Test')
        printrows(rows)
        print()
        print(read_footnote)
        print()

