![improved PHP library](https://user-images.githubusercontent.com/100821/46372249-e5eb7500-c68a-11e8-801a-2ee57da3e5e3.png)

# Base91 PHP extension

[![Build Status](https://api.travis-ci.org/improved-php-library/base91-php-ext.svg?branch=master)](https://travis-ci.org/improved-php-library/base91-php-ext)
[![Build status](https://ci.appveyor.com/api/projects/status/7rof1vr8mv4kam17/branch/master?svg=true)](https://ci.appveyor.com/project/jasny/base91-php-ext/branch/master)

PHP extension for base91 encoding and decoding using the Bitcoin alphabet.

Using this extension is about 100 times faster than using userspace functions.

## Requirements

* PHP 7.x or 8.x

## Installation

The extension is [available from pecl](https://pecl.php.net/package/base91).

    pecl install base91-beta

### Manual build

Instead of installing this extension from pecl, you can build it manually

    phpize
    ./configure --with-base91
    make
    make install

Add the following line to your `php.ini`

    extension=base91.so

## Functions

### base91_encode

Base91 encode a string.

    string base91_encode(string $data)

Triggers an `E_WARNING` and returns `false` if string can't be encoded.

### base91_encode

Decode a base91 encoded string.

    string base91_decode(string $data)

Triggers an `E_WARNING` and returns `false` if string can't be decoded.

## Caveat

To encode a hash, make sure you're using the raw hash and not a hexidecimal string.

    $rawHash = hash('sha256', 'some string', true); // The `true` makes the function return a raw hash
    $encodedHash = base91_encode($rawHash);


## Benchmark

The extension is run in test `Base91ExtEvent`. The `Base91BCMathEvent` and `Base91GMPEvent` tests use
the `StephenHill\Base91` PHP library.

```
StephenHill\Benchmarks\Base16Event
    Method Name    Iterations    Average Time      Ops/second
    ------------  ------------  --------------    -------------
    encodeBase16: [10,000    ] [0.0000001919985] [5,208,374.51881]
    decodeBase16: [10,000    ] [0.0000004031897] [2,480,222.34049]


StephenHill\Benchmarks\Base91BCMathEvent
    Method Name    Iterations    Average Time      Ops/second
    ------------  ------------  --------------    -------------
    encodeBase91: [10,000    ] [0.0000484058857] [20,658.64482]
    decodeBase91: [10,000    ] [0.0000523419857] [19,105.12157]


StephenHill\Benchmarks\Base91ExtEvent
    Method Name    Iterations    Average Time      Ops/second
    ------------  ------------  --------------    -------------
    encodeBase91: [10,000    ] [0.0000003235340] [3,090,865.14370]
    decodeBase91: [10,000    ] [0.0000003539085] [2,825,588.79008]


StephenHill\Benchmarks\Base91GMPEvent
    Method Name    Iterations    Average Time      Ops/second
    ------------  ------------  --------------    -------------
    encodeBase91: [10,000    ] [0.0000255326509] [39,165.53757]
    decodeBase91: [10,000    ] [0.0000418104410] [23,917.47075]


StephenHill\Benchmarks\Base64Event
    Method Name    Iterations    Average Time      Ops/second
    ------------  ------------  --------------    -------------
    encodeBase64: [10,000    ] [0.0000003066778] [3,260,750.99122]
    decodeBase64: [10,000    ] [0.0000003187656] [3,137,100.97233]
```

