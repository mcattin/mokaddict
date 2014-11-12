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
<title>MokAddict user UIDs</title>
<link rel="stylesheet" type="text/css" href="mokaddict.css" />
</head>
<body>
<?php

include "utils.php";

$user_id=$_GET['user_id'];


$dbh = open_db();

$userinfo = get_userinfo($dbh, $user_id);
$user_uids = get_user_uids($dbh, $user_id);

close_db($dbh);

$logo = "img/mokaddict_logo.png";
echo '<img src="'.$logo.'" />';
echo "<h1>User UIDs for: ".$userinfo['username']."</h1>";
echo "<a href=index.php>Control panel</a><br/>";
echo "<a href=view_user.php>User viewer</a>";
echo "<hr>";
echo "<br>";

print_table_pdo($user_uids);

?>
</body>
</html>
