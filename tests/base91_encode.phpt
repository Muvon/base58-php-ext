--TEST--
Test base91_encode
--SKIPIF--
<?php if (!extension_loaded("base91")) print "skip"; ?>
--FILE--
<?php
var_dump(base91_encode("Hello World"));
var_dump(base91_encode("\0\0\0"));
var_dump(base91_encode(hash('sha256', 'foo', true)));

?>
--EXPECT--
string(14) "3~d[Ik6gBXR+GQ"
string(3) "111"
string(39) "0^9|y2PTjpAH^NvilxpXa~/KN#?_qqo0<V8zjSP"

