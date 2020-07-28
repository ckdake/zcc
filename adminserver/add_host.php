<html>

  <head>
    <title>ZCC Administrative Service - <? print strftime("%m/%d/%Y"); ?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  </head>

  <body bgcolor="#FFFFFF">

    <form action = "add_host.php" method = "GET">
      <h2>Add Host:</h2>
      <table border="1" cellspacing="1" cellpadding="1" bgcolor="#FFFFFF">
        <tr>
          <td height="22" bgcolor="#CCFFFF"><b>Host:</b></td>
          <td height="22"><input type = "text" name = "new_host"></td>
        </tr>
      </table><br>
      <input type = "submit" value = "Submit">
    </form>
  
    <?php

      $address = "localhost";
      $username = "zcc";
      $password = "zcc";
      $database = "ZCCKeys";

      if (isset($_REQUEST["new_host"])) {

        $dbcnx = mysql_connect($address, $username, $password);

        if (!$dbcnx) {
          exit('<p>Unable to connect to the database server at this time.</p>');
        }
        
        mysql_select_db($database, $dbcnx);

        $host = $_REQUEST["new_host"];
        $sql = "SELECT tblHost.HostName FROM tblHost WHERE (((tblHost.HostName)='$host'))";
        $rs = mysql_query($sql);

        $i = 0;
        while($row = mysql_fetch_array($rs)) {
          $i++;
        }

        if ($i == 0) {
          $sql = "INSERT INTO tblHost(HostName) VALUES('$host')";
          $rs = mysql_query($sql);
          print "Added host --> '$host'";
        } else {
          print "ERROR --> '$host' already exists in database\n";
        }

      }

      ?>

    </table>

    <br><a href = "admin.php">Home</a>

  </body>

</html>
