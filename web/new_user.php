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
<title>MokAddict new user</title>
<link rel="stylesheet" type="text/css" href="mokaddict.css" />
</head>
<body>
<?php

include "utils.php";

$logo = "img/mokaddict_logo.png";
echo '<img src="'.$logo.'" />';
echo "<h1>New user</h1>";
echo "<a href=index.php>Control panel</a>";
echo "<hr>";
echo "<br/>";

$submit = isset($_POST["submit"]);
$username = $_POST["username"];
$first_name = $_POST["first_name"];
$last_name = $_POST["last_name"];
$email = $_POST["email"];
$notify = $_POST["notify"];
$password = $_POST["password"];

// Get password from file
$file_path = getcwd()."/mdp";
$f = fopen($file_path, "r");
$mdp = fgets($f);
fclose($f);

if($submit){

    $info = array($username, $first_name, $last_name, $email, $notify);
    $err = 0;
    foreach($info as $idx => $unit){
        switch($idx){
        case 4:
            $rgx = "/^((TRUE)|(FALSE))$/";
            break;
        case 3:
            $rgx = "/^[A-Z0-9._%+-]+@[A-Z0-9.-]+.[A-Z]{2,4}$/i";
            break;
        case 2:
            $rgx = "/^[A-Za-z]+$/";
            break;
        case 1:
            $rgx = "/^[A-Za-z]+$/";
            break;
        case 0:
            $rgx = "/^[A-Za-z]+$/";
            break;
        default:
            echo "???";
            die("$unit ?");
            break;
        }
        $n = preg_match($rgx, $unit, $matches);
        if($idx == 0){
            if("" == $unit){
                echo "ERROR: Username is mandatory.<br/>";
                $err++;
            } else if($n == 0){
                echo "ERROR: Bad characters in ".$unit.".<br/>";
                $err++;
            }
        }
        else if( ($idx == 1) || ($idx == 2) ){
            if ( ($n == 0) && ("" != $unit) ) {
                echo "ERROR: Bad characters in ".$unit.".<br/>";
                $err++;
            }
        } else {
            if ( ($n == 0) && ("" != $unit) ) {
                echo "ERROR: Incorrect format in ".$unit.".<br/>";
                $err++;
            }
        }
    }

    if(0 != $err){
        print_form($username, $first_name, $last_name, $email, $notify);
    }
    else if(hash("sha512", $password) == $mdp){
        $dbh = open_db();
        if(add_user($dbh, $username, $first_name, $last_name, $email, $notify)){
            echo "<p>SUCCESS: User ".$usename." added.<br/>";
            echo "<a href=view_user.php>User viewer</a></p>";
        }
        else{
            echo "<p>ERROR: User ".$username." already exists!</p><br/>";
            print_form($username, $first_name, $last_name, $email, $notify);
        }
        close_db($dbh);
    }
    else{
        echo "ERROR: Incorrect password.<br/>";
        print_form($username, $first_name, $last_name, $email, $notify);
    }

}
else{
    print_form($username, $first_name, $last_name, $email, $notify);
}


function print_form($username, $first_name, $last_name, $email, $notify){

echo '<form action="new_user.php" method="post">';
echo '    <p>Username*: <input type="text" name="username" value="'.$username.'" /><br /></p>';
echo '    <p>First name: <input type="text" name="first_name" value="'.$first_name.'"/><br /></p>';
echo '    <p>Last name: <input type="text" name="last_name" value="'.$last_name.'" /><br /></p>';
echo '    <p>email: <input type="text" name="email" value="'.$email.'" /><br /></p>';
echo '    <p>email notification:';
echo '    <input type="radio" name="notify" value="FALSE" checked="checked" /> No';
echo '    <input type="radio" name="notify" value="TRUE" /> Yes</p>';
echo '    <p><br/></p>';
echo '    <p>Password: <input type="password" name="password" value=""/><br /></p>';
echo '    <p><input type="submit" name="submit" value="Register"></p>';
echo '</form>';

echo '<p>* mandatory</p>';

}


?>
</body>
</html>
