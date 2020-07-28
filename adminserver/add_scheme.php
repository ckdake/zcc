<html>

  <head>
    <title>ZCC Administrative Service - <? print strftime("%m/%d/%Y"); ?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  </head>

  <body bgcolor="#FFFFFF">

    <form action = "add_scheme.php" method = "GET">
      <h2>Add Scheme:</h2>
      <table border="1" cellspacing="1" cellpadding="1" bgcolor="#FFFFFF">
        <tr>
          <td height="22" bgcolor="#CCFFFF"><b>Scheme:</b></td>
          <td height="22"><input type = "text" name = "new_scheme"></td>
        </tr>
      </table><br>
      <input type = "submit" value = "Submit">
    </form>
  
    <?php

      $address = "localhost";
      $username = "zcc";
      $password = "zcc";
      $database = "ZCCKeys";

      if (isset($_REQUEST["new_scheme"])) {

        $dbcnx = mysql_connect($address, $username, $password);

        if (!$dbcnx) {
          exit('<p>Unable to connect to the database server at this time.</p>');
        }

        mysql_select_db($database, $dbcnx);

        $scheme = $_REQUEST["new_scheme"];
        $sql = "SELECT tblScheme.SchemeName FROM tblScheme WHERE (((tblScheme.SchemeName)='$scheme'))";
        $rs = mysql_query($sql);

        $i = 0;
        while($row = mysql_fetch_array($rs)) {
          $i++;
        }

        if ($i == 0) {
          $sql = "INSERT INTO tblScheme(SchemeName) VALUES('$scheme')";
          $rs = mysql_query($sql);
          print "Added scheme --> '$scheme'";
        } else {
          print "ERROR --> '$scheme' already exists in database\n";
        }

      }

    ?>

    </table>

    <br><a href = "admin.php">Home</a>

  </body>

</html>
