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
<title>MokAddict user viewer</title>
<link rel="stylesheet" type="text/css" href="mokaddict.css" />
</head>
<body>
<?php
include "utils.php";

$logo = "img/mokaddict_logo.png";
print '<img src="'.$logo.'" />';
echo "<h1>User viewer</h1>";
echo "<a href=index.php>Control panel</a>";
echo "<hr>";

$dbh = open_db();

$sql = "SELECT * FROM addicts";
$sth = $dbh->prepare($sql);
$sth->execute();

$rows = $sth->fetchAll(PDO::FETCH_BOTH);
$col_names = array_keys($rows[0]);
$nb_cols = $sth->columnCount();

if(isset($col_names)){

    echo "<table class='simpletable'>";
    echo "<thead><tr>";
    foreach(range(0, ($nb_cols * 2)-2, 2) as $i){
        echo "<th align='left'>".$col_names[$i]."</th>";
    }
    echo "<th align='left'>UIDs</th>";
    echo "<th align='left'>Consumption</th>";
    echo "<th align='left'>TOTAL</th>";
    echo "</tr></thead>";

    foreach($rows as $row){

        if(!isset($row[0])) continue;

        echo "<tr>";
        for($i=0; $i < $nb_cols; $i++){
            echo "<td>".$row[$i]."</td>";
        }
        echo "<td><a href=view_user_uids.php?user_id=".$row['id'].">view</a> ";
        echo "<a href=add_uid.php?user_id=".$row['id'].">add</a></td>";
        echo "<td><a href=view_user_detail.php?user_id=".$row['id'].">view</a></td>";
        $kfe_nb = get_kfe_nb($dbh, $row['id']);
        echo "<td align='center'><b>".$kfe_nb."</b></td>";
        echo "</tr>";
    }
    echo "</table>";
}
else{
    echo "<p>Sorry, nothing to display!</p>";
}

close_db($dbh);

?>
</body>
</html>
