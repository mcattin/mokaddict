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
<title>MokAddict control panel</title>
<link rel="stylesheet" type="text/css" href="mokaddict.css" />
</head>
<body>
<?php
$logo = "img/mokaddict_logo.png";
echo '<img src="'.$logo.'" />';
echo "<h1>Control panel</h1>";
echo "<hr>";
echo "<br><a href=view_user.php>User viewer</a>";
echo "<br><a href=view_log.php>Log viewer</a>";
echo "<br><a href=new_user.php>New user</a>";
//echo "<br><a href=test.php>Test</a>";

//phpinfo();

?>
</body>
</html>
