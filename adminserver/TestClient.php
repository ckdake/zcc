<?

  include("xmlrpc.inc");

  $client = new xmlrpc_client("admin_server/KeyService.php", "www.ithought.org", 80);
  $client->setDebug(2);
  
  if (isset($_REQUEST['host']) && isset($_REQUEST['scheme'])) {
    $host_id = $_REQUEST['host'];
    $scheme_id = $_REQUEST['scheme'];
    $format = new xmlrpcmsg("zcc.getkey", array(new xmlrpcval("$host_id", "string"), new xmlrpcval("$scheme_id", "string")));
    $request = $client->send($format);
    $value = $request->value();
    print $value->scalarval();
  }

?>
