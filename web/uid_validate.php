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
<title>MokAddict UID validation</title>
<link rel="stylesheet" type="text/css" href="mokaddict.css" />
</head>
<body>
<?php

include "utils.php";

$user_id=$_POST['user_id'];
$rfid = $_POST['rfid'];
$password = $_POST["password"];

$logo = "img/mokaddict_logo.png";
echo '<img src="'.$logo.'" />';
echo "<h1>UID validation</h1>";
echo "<a href=index.php>Control panel</a>";
echo "<hr>";
echo "<br/>";

// Get password from file
$file_path = getcwd()."/mdp";
$f = fopen($file_path, "r");
$mdp = fgets($f);
fclose($f);

$rgx1 = "/^[A-Z0-9]{8}$/";
$rgx2 = "/^[A-Z0-9]{14}$/";

if(hash("sha512", $password) == $mdp){
    if(preg_match($rgx1, $rfid) || preg_match($rgx2, $rfid)){

        $dbh = open_db();
        $userinfo = get_userinfo($dbh, $user_id);
        if(add_user_uid($dbh, $user_id, $rfid, "TRUE")){
            echo "<p>SUCCESS: UID=".$rfid." added to user=".$userinfo['username']."<br />";
            echo "<a href=view_user_uids.php?user_id=".$user_id.">View ".$userinfo['username']." UIDs</a></p>";
        }
        else{
            echo "<p>ERROR: UID already exists!<br />";
            echo "<a href=add_uid.php?user_id=".$user_id.">Back</a></p>";
        }
        close_db($dbh);
    }
    else{
        echo "<p>ERROR: Invalid UID format!<br />";
        echo "<a href=add_uid.php?user_id=".$user_id.">Back</a></p>";
    }
}
else{
    echo "ERROR: Incorrect password.<br/>";
    echo "<a href=add_uid.php?user_id=".$user_id.">Back</a></p>";
}

?>
</body>
</html>
