--TEST--
php_profiler_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('php_profiler')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = php_profiler_test1();

var_dump($ret);
?>
--EXPECT--
The extension php_profiler is loaded and working!
NULL
