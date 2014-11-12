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
<title>MokAddict log viewer</title>
<link rel="stylesheet" type="text/css" href="mokaddict.css" />
</head>
<body>
<?php

include "utils.php";

$dbh = open_db();

$sql = "SELECT * FROM kfe_log";
$sth = $dbh->prepare($sql);
$sth->execute();

close_db($dbh);

$logo = "img/mokaddict_logo.png";
print '<img src="'.$logo.'" />';
print "<h1>Log viewer</h1>";
print "<a href=index.php>Control panel</a>";
print "<hr>";
print "<br>";

print_table_pdo($sth);

?>
</body>
</html>
