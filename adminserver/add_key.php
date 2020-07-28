<html>

  <head>
    <title>ZCC Administrative Service - <? print strftime("%m/%d/%Y"); ?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  </head>

  <body bgcolor="#FFFFFF">

    <form action = "add_key.php" method = "GET">
    <?php

      $address = "localhost";
      $username = "zcc";
      $password = "zcc";
      $database = "ZCCKeys";

      if (isset($_REQUEST['host']) && isset($_REQUEST['scheme']) && isset($_REQUEST['key'])) {

        $host_id = $_REQUEST['host'];
        $scheme_id = $_REQUEST['scheme'];
        $key = $_REQUEST['key'];

        $dbcnx = @mysql_connect($address, $username, $password);

        if (!$dbcnx) {
          exit('<p>Unable to connect to the database server at this time.</p>');
        }

        @mysql_select_db($database, $dbcnx);
        $sql = "INSERT INTO tblKey(HostID, SchemeID, PublicKey) VALUES($host_id, $scheme_id, '$key')";
        $rs = mysql_query($sql);
        print "Successfully Added Key\n";

      }

      print "<h2>Add Key:</h2>";
      print "<table border=\"1\" cellspacing=\"1\" cellpadding=\"1\" bgcolor=\"#FFFFFF\">";
      print "<tr>";
      print "<td height=\"22\" bgcolor=\"#CCFFFF\"><b>Host:</b></td>";
      print "<td height=\"22\">";
      print "<select name = \"host\">";

      $dbcnx = @mysql_connect($address, $username, $password);

      if (!$dbcnx) {
        exit('<p>Unable to connect to the database server at this time.</p>');
      }

      @mysql_select_db($database, $dbcnx);
      $sql = "SELECT * FROM tblHost";
      $rs = mysql_query($sql);

      while($row = mysql_fetch_array($rs)) {
        $id = $row[0];
        $host = $row[1];
        print ("<option value = \"$id\">$host");
      }

      print "</select>";
      print "</td>";
      print "<td>";
      print "<a href = \"add_host.php\">Add Host</a>";
      print "</td>";
      print "</tr>";
      print "<tr>";
      print "<td height=\"22\" bgcolor=\"#CCFFFF\"><b>Encryption Scheme:</b></td>";
      print "<td height=\"22\">";
      print "<select name = \"scheme\">";

      $sql = "SELECT * FROM tblScheme";
      $rs = @mysql_query($sql);

      while($row = mysql_fetch_array($rs)) {
        $id = $row[0];
        $scheme = $row[1];
        print ("<option value = \"$id\">$scheme");
      }

      ?>              

            </select>
          <td>
          <a href = "add_scheme.php">Add Scheme</a>
          </td>
          </td>
        </tr>
        <tr>
          <td height="22" bgcolor="#CCFFFF"><b>Key:</b></td>
          <td height="22">
            <input type = "text" name = "key">
          <td>
        </tr>
      </table><br>
      <input type = "submit" value = "Submit">
    </form>
    </table>

    <br><a href = "admin.php">Home</a>

  </body>

</html>
