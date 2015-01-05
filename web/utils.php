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

error_reporting(0);

function print_table_pdo($sth){

    $rows = $sth->fetchAll(PDO::FETCH_BOTH);
    $col_names = array_keys($rows[0]);
    $nb_cols = $sth->columnCount();

    if(isset($col_names)){

        echo "<table class='simpletable'>";
        echo "<thead><tr>";
        foreach(range(0, ($nb_cols * 2)-2, 2) as $i){
            echo "<th align='left'>".$col_names[$i]."</th>";
        }
        echo "</tr></thead>";

        foreach($rows as $row){

            if(!isset($row[0])) continue;

            echo "<tr>";
            for($i=0; $i < $nb_cols; $i++){
                echo "<td>".$row[$i]."</td>";
            }
            echo "</tr>";
        }
        echo "</table>";
    }
    else{
        echo "<p>Sorry, nothing to display!</p>";
    }
}


function open_db(){
    try{
        $dbh = new PDO("sqlite:../mokaddict.sqlite");
        $dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
    }
    catch(PDOException $e){
        echo $e->getMessage();
    }
    return $dbh;
}

function close_db($dbh){
    $dbh = null;
}


function get_kfe_nb($dbh, $user_id){

    //echo "<br />".$username."<br />";

    $sql = "SELECT COUNT(*) FROM kfe_log WHERE rfid IN ( SELECT rfid FROM uids WHERE user_id LIKE ( SELECT id FROM addicts WHERE id LIKE :userid ) ) AND auth='TRUE' ";

    $sth = $dbh->prepare($sql);
    $sth->bindValue(":userid", $user_id, PDO::PARAM_STR);
    $sth->execute();

    $rows = $sth->fetch(PDO::FETCH_NUM);
    return $rows[0];
}


function get_kfe_log($dbh, $user_id){

    //echo "<br />".$username."<br />";

    $sql = "SELECT * FROM kfe_log WHERE rfid IN ( SELECT rfid FROM uids WHERE user_id LIKE ( SELECT id FROM addicts WHERE id LIKE :userid ) ) ";

    $sth = $dbh->prepare($sql);
    $sth->bindValue(":userid", $user_id, PDO::PARAM_STR);
    $sth->execute();

    return $sth;
}

function get_user_uids($dbh, $user_id){

    //echo "<br />".$username."<br />";

    $sql = "SELECT * FROM uids WHERE user_id LIKE :userid";

    $sth = $dbh->prepare($sql);
    $sth->bindValue(":userid", $user_id, PDO::PARAM_STR);
    $sth->execute();

    return $sth;
}


function add_user_uid($dbh, $user_id, $rfid, $auth){

    try{
        $sql = "SELECT * FROM uids WHERE rfid LIKE :rfid";
        $sth = $dbh->prepare($sql);
        $sth->bindValue(":rfid", $rfid, PDO::PARAM_STR);
        $sth->execute();
        if($sth->fetchall()){
            return false;
        }
        else{
            $sql = "INSERT INTO uids (user_id, rfid, authorised) VALUES (:userid, :rfid, :auth)";
            $sth = $dbh->prepare($sql);
            $sth->bindParam(":userid", $user_id, PDO::PARAM_INT);
            $sth->bindParam(":rfid", $rfid, PDO::PARAM_STR);
            $sth->bindParam(":auth", $auth, PDO::PARAM_STR);
            $sth->execute();
            //print_r($dbh->errorInfo());
            return true;
        }

    }
    catch(PDOException $e){
        echo $e->getMessage();
    }

}


function add_user($dbh, $username, $first_name, $last_name, $email, $notify){

    //echo "<br />".$username."<br />";

    try{
        $sql = "SELECT * FROM addicts WHERE username LIKE :username";
        $sth = $dbh->prepare($sql);
        $sth->bindValue(":username", $username, PDO::PARAM_STR);
        $sth->execute();

        if($sth->fetchall()){
            return false;
        }
        else{
            $sql = "INSERT INTO addicts (username, first_name, last_name, email, notify) VALUES (:username, :first_name, :last_name, :email, :notify)";
            $sth = $dbh->prepare($sql);
            $sth->bindParam(":username", $username, PDO::PARAM_STR);
            $sth->bindParam(":first_name", $first_name, PDO::PARAM_STR);
            $sth->bindParam(":last_name", $last_name, PDO::PARAM_STR);
            $sth->bindParam(":email", $email, PDO::PARAM_STR);
            $sth->bindParam(":notify", $notify, PDO::PARAM_STR);
            $sth->execute();
            //print_r($dbh->errorInfo());
            return true;
        }

    }
    catch(PDOException $e){
        echo $e->getMessage();
    }

}


function get_userinfo($dbh, $user_id){

    $sql = "SELECT * FROM addicts WHERE id LIKE :userid ";

    $sth = $dbh->prepare($sql);
    $sth->bindValue(":userid", $user_id, PDO::PARAM_STR);
    $sth->execute();

    $userinfo = $sth->fetchall();
    return $userinfo[0];
}

?>
