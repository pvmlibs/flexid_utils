--TEST--
Sparx64 cipher test
--FILE--
<?php
$key = \pack('v8', 0x0011, 0x2233, 0x4455, 0x6677, 0x8899, 0xAABB, 0xCCDD, 0xEEFF);
$plaintext = \pack('v4', 0x0123, 0x4567, 0x89AB, 0xCDEF);

$cipher = new \FlexId\Sparx64($key);

$unpacked = \unpack('Q', $plaintext);
var_dump($unpacked);
var_dump($cipher->encrypt($unpacked[1]));
?>
--EXPECT--
array(1) {
  [1]=>
  int(-3607513407803686621)
}
int(6888257785890483134)