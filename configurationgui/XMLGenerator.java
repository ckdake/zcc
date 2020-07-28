import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Vector;

public class XMLGenerator {

	private static final String[] SERVICES = { "FTP", "Apache", "HTTP", "MySQL", "zcc_configd"};

	private static final String[] SCHEMES = { "AES-128","AES-192","AES-256"};

	public static void main(String[] args) {

		Vector serviceVector = null;
		serviceVector = generateRandomServices();

		String toExport = "<?xml version=\"1.1\"?>\n"
				+ "<!DOCTYPE services [\n"
				+ "  <!ELEMENT service (name,host,port,type,encrypt)>\n"
				+ "  <!ELEMENT name    (#PCDATA)>\n"
				+ "  <!ELEMENT host    (#PCDATA)>\n"
				+ "  <!ELEMENT port    (#PCDATA)>\n"
				+ "  <!ELEMENT type    (client,server)>\n"
				+ "  <!ELEMENT scheme  (#PCDATA)>\n"
				+ "  <!ELEMENT encrypt (false,true)>\n]>\n\n";
		if (serviceVector.size() == 0) {
			toExport += "<services />\n";
		} else {
			toExport += "<services>\n";
			for (int i = 0; i < serviceVector.size(); i++) {
				toExport += ((ZCCService) serviceVector.elementAt(i))
						.asXMLString();
			}
			toExport += "</services>";
		}
		try {
			BufferedWriter bw = new BufferedWriter(new FileWriter(
					"services.xml"));
			bw.write(toExport);
			bw.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
	}

	private static Vector generateRandomServices() {
		Vector retval = new Vector();
		for (int i = 0; i < 1000; i++) {
			ZCCService temp = generateRandomService();
			retval.add(temp);
		}
		return retval;
	}

	private static ZCCService generateRandomService() {
		String name = SERVICES[(int) (Math.random() * SERVICES.length) % SERVICES.length];
		byte octet1 = (byte) Math.abs((byte) (255 * Math.random()));
		byte octet2 = (byte) Math.abs((byte) (255 * Math.random()));
		byte octet3 = (byte) Math.abs((byte) (255 * Math.random()));
		byte octet4 = (byte) Math.abs((byte) (255 * Math.random()));
		String ip = octet1 + "." + octet2 + "." + octet3 + "." + octet4;
		short port = (short) (Math.random() * 65535);
		String type = "";
		if (((int) (Math.random() * 2) % 2) == 0) {
			type = "client";
		} else {
			type = "server";
		}
		String scheme = SCHEMES[(int) (Math.random() * SCHEMES.length) % SCHEMES.length];
		boolean encrypt = false;
		if (((int) (Math.random() * 2) % 2) == 0) {
			encrypt = true;
		}
		ZCCService retval = new ZCCService(name, ip, (short) Math.abs(port), type, scheme, encrypt);
		return retval;
	}
}