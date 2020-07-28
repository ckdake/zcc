<?

  include("xmlrpc.inc");
  include("xmlrpcs.inc");


  function getkey($par) {

    $address = "localhost";
    $username = "zcc";
    $password = "zcc";
    $database = "ZCCKeys";

    $dbcnx = @mysql_connect($address, $username, $password);
 
    $host = $par->getParam(0)->scalarval();
    $scheme = $par->getParam(1)->scalarval();

    if (!$dbcnx) {
      exit('<p>Unable to connect to the database server at this time.</p>');
    }
        
    @mysql_select_db($database, $dbcnx);

    $sql = "SELECT tblHost.HostID FROM tblHost WHERE (((tblHost.HostName)='$host'))";
    $rs = mysql_query($sql);
    while($row = mysql_fetch_array($rs)) {
      $host_id = $row[0];
    }

    $sql = "SELECT tblScheme.SchemeID FROM tblScheme WHERE (((tblScheme.SchemeName)='$scheme'))";
    $rs = mysql_query($sql);
    while($row = mysql_fetch_array($rs)) {
      $scheme_id = $row[0];
    }

    $sql = "SELECT tblKey.PublicKey FROM tblKey WHERE (((tblKey.HostID)=$host_id) AND ((tblKey.SchemeID)=$scheme_id))";
    $rs = mysql_query($sql);
    while($row = mysql_fetch_array($rs)) {
      $key =$row[0];
    }

    return new xmlrpcresp(new xmlrpcval($key));
  }

  $s = new xmlrpc_server( array("zcc.getkey" => array("function" => "getkey")));

?>
