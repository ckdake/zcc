<html>

  <head>
    <title>ZCC Administrative Service - <? print strftime("%m/%d/%Y"); ?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  </head>

  <body bgcolor="#FFFFFF">

    <h2>Keys</h2>
    <table border="1" cellspacing="1" cellpadding="1" bgcolor="#FFFFFF">
      <tr bgcolor="#CCFFFF">
        <td height="22"><b>Host</b></td>
        <td height="22"><b>Encryption Scheme</b></td>
        <td height="22"><b>Public Key</b></td>
        <td height="22"><b>Remove Key</b></td>
      </tr>
  
      <?php

        $address = "localhost";
        $username = "zcc";
        $password = "zcc";
        $database = "ZCCKeys";
        $dbcnx = @mysql_connect($address, $username, $password);

        if (!$dbcnx) {
          exit('<p>Unable to connect to the database server at this time.</p>');
        }
        
        @mysql_select_db($database, $dbcnx);

        if (isset($_REQUEST['remove_id'])) {
          $id = $_REQUEST['remove_id'];
          $sql = "DELETE FROM tblKey WHERE KeyID = $id";
          $rs = @mysql_query($sql);
          print "Key successfully removed\n";
        }

        $sql = "SELECT tblKey.KeyID, tblHost.HostName, tblScheme.SchemeName, tblKey.PublicKey FROM tblHost, tblScheme, tblKey WHERE tblHost.HostID = tblKey.HostID AND tblKey.SchemeID = tblScheme.SchemeID";
        $rs = @mysql_query($sql);

        while($row = @mysql_fetch_array($rs)) {
          $id = $row[0];
          $host = $row[1];
          $scheme = $row[2];
          $public_key = $row[3];

          print ("<tr>");
            print ("<td>$host</td>");
            print ("<td>$scheme</td>");
            print ("<td>$public_key</td>");
            print ("<td><a href = \"admin.php?remove_id=$id\">Remove</a></td>");
          print ("</tr>");
        }

        /* Close the database */

      ?>

    </table><br>

    <form action = "add_key.php" method = "GET">
      <input type = "submit" value = "Add Key">
    </form>

  </body>

</html>
