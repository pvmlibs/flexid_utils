--TEST--
Sparx128 cipher test
--FILE--
<?php
        $key = \pack(
            'v16',
            0x0011,
            0x2233,
            0x4455,
            0x6677,
            0x8899,
            0xAABB,
            0xCCDD,
            0xEEFF,
            0xFFEE,
            0xDDCC,
            0xBBAA,
            0x9988,
            0x7766,
            0x5544,
            0x3322,
            0x1100,
        );
        $plaintext = \pack('v8', 0x0123, 0x4567, 0x89AB, 0xCDEF, 0xFEDC, 0xBA98, 0x7654, 0x3210);

        $cipher = new \FlexId\Sparx128($key);

        $unpacked = \unpack('Q2', $plaintext);
        var_dump($unpacked);

        $encrypted = $cipher->encrypt($unpacked[1], $unpacked[2]);
        var_dump($encrypted);
?>
--EXPECT--
array(2) {
  [1]=>
  int(-3607513407803686621)
  [2]=>
  int(3607513407803686620)
}
array(2) {
  [0]=>
  int(7846982249533092648)
  [1]=>
  int(-4025966620788378927)
}