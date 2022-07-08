--TEST--
Test base91_decode
--SKIPIF--
<?php if (!extension_loaded("base91")) print "skip"; ?>
--FILE--
<?php
var_dump(base91_decode("3~d[Ik6gBXR+GQ"));
var_dump(bin2hex(base91_decode("111")));
var_dump(bin2hex(base91_decode("0^9|y2PTjpAH^NvilxpXa~/KN#?_qqo0<V8zjSP")));

?>
--EXPECT--
string(11) "Hello World"
string(6) "000000"
string(64) "2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae"

