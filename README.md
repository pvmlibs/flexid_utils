# Flexid utility tools

This extension provides tools for:
- encoding/decoding number to/form string with custom alphabet
- encrypt/decrypt numbers
- fast hmac algorithms optimized for number input

## Requirements

1. 64-bit system
2. PHP >= 8.1

## Installation with PIE

There are prebuild packages for PHP 8.1-8.6 for x86, ARM, zts, nts, musl and gnu, so you can install without building:
```shell
pie install --no-interaction pvmlibs/flexid_utils
```
For any other platform (Windows / Mac) you need to build it from source.

## Installation from sources

```shell
git clone https://github.com/pvmlibs/flexid_utils.git
cd flexid_utils
phpize
./configure CFLAGS="-O2 -g0"
make && make test
make install
```

## Usage
You probably want to use it with package [pvmlibs/flexid](https://packagist.org/packages/pvmlibs/flexid), which utilizes
this extension for faster processing.

You can use it also directly. See [stubs](flexid_utils.stub.php) for available classes.
