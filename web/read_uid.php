<?php
/*
Copyright Matthieu Cattin 2014

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
?>

<!DOCTYPE HTML>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>MokAddict add UID</title>
<link rel="stylesheet" type="text/css" href="mokaddict.css" />
</head>
<body>
<?php


$logo = "img/mokaddict_logo.png";
echo '<img src="'.$logo.'" />';
echo "<h1>Read UID and display</h1>";
echo "<a href=index.php>Control panel</a><br/>";
echo "<hr>";
echo "<br/>";
echo "<p>1) Press B, keep pressed.</p>";
echo "<p>2) Press A and release both.</p>";
echo "<p>3) Swipe the card.</p>";
echo "<p>4) Refresh the page.</p>";
echo "<br />";

// Get UID from file
$file_path = getcwd()."/../python/rfid.txt";
$f = fopen($file_path, "r+");
$rfid = fgets($f);
if(!$rfid){
    $rfid = "???";
}
ftruncate($f, 0);
fclose($f);

echo "<p>RFID: ".$rfid."</p>";

?>
</body>
</html>
