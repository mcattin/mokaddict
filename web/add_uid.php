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

include "utils.php";

$user_id=$_GET['user_id'];
$submit = isset($_POST["submit"]);

// Get username from database
$dbh = open_db();
$userinfo = get_userinfo($dbh, $user_id);
$user_uids = get_user_uids($dbh, $user_id);
close_db($dbh);

// Get password from file
$file_path = getcwd()."/mdp";
$f = fopen($file_path, "r");
$mdp = fgets($f);
fclose($f);

$logo = "img/mokaddict_logo.png";
echo '<img src="'.$logo.'" />';
echo "<h1>Add UID for: ".$userinfo['username']."</h1>";
echo "<a href=index.php>Control panel</a><br/>";
echo "<a href=view_user.php>User viewer</a>";
echo "<hr>";
echo "<br/>";
echo "<p>1) Press B, keep pressed.</p>";
echo "<p>2) Press A and release both.</p>";
echo "<p>3) Swipe the card.</p>";
echo "<p>4) Refresh the page.</p>";
echo "<br />";


$rgx1 = "/^[A-Z0-9]{8}$/";
$rgx2 = "/^[A-Z0-9]{14}$/";

if($submit){

    $rfid = $_POST["rfid"];
    $password = $_POST["password"];

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
                print_form($user_id, $rfid);
            }
            close_db($dbh);
        }
        else{
            echo "<p>ERROR: Invalid UID format!<br />";
            print_form($user_id, $rfid);
        }
    }
    else{
        echo "ERROR: Incorrect password.<br/>";
        print_form($user_id, $rfid);
    }
}
else{

    // Get UID from file
    $file_path = getcwd()."/../python/rfid.txt";
    $f = fopen($file_path, "r+");
    $rfid = fgets($f);
    if(!$rfid){
        $rfid = "???";
    }
    ftruncate($f, 0);
    fclose($f);


    print_form($user_id, $rfid);
}

function print_form($user_id, $rfid){

echo "<form action='add_uid.php?user_id=".$user_id."' method='post'>";
echo "  <p>User ID: <input type='text' name='user_id' value=".$user_id." readonly='readonly'/><br /></p>";
echo "  <p>RFID: <input type='text' name='rfid' value=".$rfid." readonly='readonly'/><br /></p>";
echo '  <p><br/></p>';
echo '  <p>Password: <input type="password" name="password" value=""/><br /></p>';
echo "  <p><input type='submit' name='submit' value='Add'></p>";
echo "</form>";

}


?>
</body>
</html>
