<?php
//die();
error_reporting(0);
while(true) 
{
	sleep(1);
	if (file_exists('!restart!')) 
	{
		$hnd = popen("restart.bat", "r");
		sleep(1);
		pclose($hnd);
		unlink('!restart!');
		echo "Restarted at ".date("D, j M Y G:i:s")."\r\n";
	}
	if (file_exists('!stop!')) 
	{
		$hnd = popen("stop.bat", "r");
		sleep(1);
		pclose($hnd);
		unlink('!stop!');
		echo "Stopped at ".date("D, j M Y G:i:s")."\r\n";
	}
}
?>