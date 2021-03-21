--TEST--
Check if php_profiler is loaded
--SKIPIF--
<?php
if (!extension_loaded('php_profiler')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "php_profiler" is available';
?>
--EXPECT--
The extension "php_profiler" is available
