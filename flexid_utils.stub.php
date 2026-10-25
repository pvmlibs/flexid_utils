<?php

/** @generate-class-entries */
namespace FlexId;

/**
 * Fast keyed hash siphash-2-4, optimized for input with additional number.
 */
class Siphash
{
    /**
     * @param string $key 16-byte secret key
     * @throws \ValueError
     */
    public function __construct($key) {}

    /**
     * @param string $data message to hash
     * @param int|null $num additional number to hash (hash(number || $data))
     * @return int 64 bit output hash as number
     */
    public function hash($data, $num = null): int {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}

/**
 * Strong and fast cryptographic keyed hash.
 */
class Blake3
{
    /**
     * @param string $key 32-byte secret key
     * @throws \ValueError
     */
    public function __construct($key) {}

    /**
     * @param string $data message to hash
     * @param int|null $num additional number to hash (hash(number || $data))
     * @return list<int> list of up to $output_parts (1-4) 64-bit data chunks as numbers
     * @throws \ValueError
     */
    public function hash($data, $num = null, int $output_parts = 1): array {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}

/**
 * Sparx cipher, operates on 64-bit block (1 number) with 128-bit key, produces 64-bit output (1 number)
 */
class Sparx64
{
    /**
     * @param string $key 16-byte secret key
     * @throws \ValueError
     */
    public function __construct($key) {}
    public function encrypt(int $num): int {}
    public function decrypt(int $data): int {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}

/**
 * Sparx cipher, operates on 128-bit block (2 numbers) with 256-bit key, produces 128-bit output (2 numbers)
 */
class Sparx128
{
    /**
     * @param string $key 32-byte secret key
     * @throws \ValueError
     */
    public function __construct($key) {}
    /**
     * @return list<int> 2 numbers output
     */
    public function encrypt(int $hipart, int $lowpart): array {}
    /**
     * @return list<int> 2 numbers output
     */
    public function decrypt(int $hipart, int $lowpart): array {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}

/**
 * Transforms number to/form string with custom alphabet
 */
class Encoder
{
    /**
     * @param string $alphabet Custom alphabet (2-94 max length)
     * @param string $separator separator used in doubleEncode/doubleDecode, if first encoded number is less than getMaxEncodedLength()
     * @throws \ValueError
     */
    public function __construct($alphabet, $separator = ' ') {}
    public function encode(int $num): string {}
    /**
     * @throws \ValueError
     */
    public function decode(string $input): int {}

    /**
     * Encodes 2 numbers
     * @param int $hipart
     * @param int $lowpart
     * @return string output can take up to 2 * getMaxEncodedLength()
     */
    public function doubleEncode($hipart, $lowpart): string {}
    /**
     * @return list<int> returns 2 numbers (encoded as $hipart, $lowpart)
     * @throws \ValueError
     */
    public function doubleDecode(string $input): array {}

    /**
     * @return int Max length of encoded number
     */
    public function getMaxEncodedLength(): int {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}

/**
 * Transforms positive number to/form string.
 * This is intended to get short, random look strings from numbers, even for sequential ones.
 * Primary goal is to get short output, so this should not be treated as secure!
 * Use encryption if you need strong confidentiality.
 */
class HashEncoder
{
    /**
     * @param string $alphabet Alphabet (9-94 max length, without power of 2).
     *                                  This should be treated as secret to make reversing more difficult.
     * @param int $offset number added to $num when encoding and subtracted after decoding
     * @throws \ValueError
     */
    public function __construct($alphabet, $offset = 0) {}

    /**
     * @param int $num only positive numbers are allowed
     */
    public function encode($num): string {}
    /**
     * @throws \ValueError
     */
    public function decode(string $input): int {}
    /**
     * @return int Max length of encoded number
     */
    public function getMaxEncodedLength(): int {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}
