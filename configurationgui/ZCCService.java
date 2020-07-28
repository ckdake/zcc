/**
 * Service.java
 * 
 * Service is a representation of a service that a user can configure for
 * encryption with the ZCC protocol. It holds a user-defined service name, a
 * host IP address, a port number, a service type (client or server), and
 * encryption status (of or off).
 * 
 * @author Zachary M. Allen
 * @version 1.0
 * 
 */
public class ZCCService {

	/**
	 * Error message String for an invalid IP address
	 */
	private static final String INVALID_IP_ADDRESS = "Invalid IP Address";

	/**
	 * Constant representing IPv4
	 */
	private static final byte IPv4 = 4;

	/**
	 * Constant representing IPv6
	 */
	private static final byte IPv6 = 6;

	/**
	 * Constant representing a client service
	 */
	private static final byte CLIENT = 0;

	/**
	 * Constant represeting server service
	 */
	private static final byte SERVER = 1;

	/**
	 * String constant representing client
	 */
	public static final String T_CLIENT = "client";

	/**
	 * String constant representing server
	 */
	public static final String T_SERVER = "server";

	/**
	 * Constant representing the size of an IP address octet
	 */
	private static final int MAX_OCTET = 256;

	/**
	 * The service name - determined by the user
	 */
	private String name;

	/**
	 * The IP address of the host to encrypt traffic to/from
	 */
	private long host;

	/**
	 * The port to/from which traffic will be encrypted
	 */
	private short port;

	/**
	 * The type of service (client or server)
	 */
	private byte type;

	/**
	 * The encryption scheme used to encrypt traffic to/from this service
	 */
	private String scheme;

	/**
	 * True if this service's traffic is encrypted; false otherwise
	 */
	private boolean encrypt;

	/**
	 * The number of octets in the host's IP address. Used for IPv4/IPv6.
	 */
	private int numOctets;

	/**
	 * Constructor
	 * 
	 * @param newName
	 *            Name of new service
	 * @param newHost
	 *            New host IP address
	 * @param newPort
	 *            New port
	 * @param newScheme
	 *            New encryption scheme
	 * @param newType
	 *            New type of service
	 * @param newEncrypt
	 *            New encryption status
	 */
	public ZCCService(String newName, String newHost, short newPort,
			String newType, String newScheme, boolean newEncrypt) {
		name = newName;
		host = convertIPToLong(newHost);
		port = newPort;
		type = convertTypeToByte(newType);
		scheme = newScheme;
		encrypt = newEncrypt;
	}

	/**
	 * Helper method converts type from String to byte
	 * 
	 * @param newType
	 *            String representation of service type
	 * @return Byte representation of service type
	 */
	private byte convertTypeToByte(String newType) {
		byte retval = 0;
		if (newType.equals(T_CLIENT)) {
			retval = CLIENT;
		} else if (newType.equals(T_SERVER)) {
			retval = SERVER;
		}
		return retval;
	}

	/**
	 * Helper method converts IP address from String to long
	 * 
	 * @param newHost
	 *            String representation of host IP address
	 * @return Long representation of host IP address
	 */
	private long convertIPToLong(String newHost) {

		long toReturn = 0;
		long multiplier = 1;

		String[] octets = newHost.split("\\.");
		numOctets = octets.length;

		if (numOctets != IPv4 && numOctets != IPv6) {
			KeyClient.errorMessage(INVALID_IP_ADDRESS);
		}

		for (int i = (numOctets - 1); i >= 0; i--) {
			try {
				toReturn += (new Long(octets[i])).longValue() * multiplier;
				multiplier *= MAX_OCTET;
			} catch (NumberFormatException nfe) {
				KeyClient.errorMessage(INVALID_IP_ADDRESS);
			}
		}

		return toReturn;

	}

	/**
	 * Constructs a String representation of a Service used for exporting
	 * services to an XML configuration file.
	 * 
	 * @return XML element String representation of Service
	 */
	public String asXMLString() {
		return "  <service name = \"" + name + "\" host = \"" + hostAsString()
				+ "\" port = \"" + port + "\" type = \"" + typeAsString()
				+ "\" scheme = \"" + scheme + "\" encrypt = \"" + encrypt
				+ "\" />\n";
	}

	/**
	 * Constructs a String representation of a Service used for listing Services
	 * to the user
	 * 
	 * @return String representation of Service
	 */
	public String toString() {
		return "SERVICE: " + name + " HOST: " + hostAsString() + " PORT: "
				+ port + " TYPE: " + typeAsString() + " SCHEME: " + scheme
				+ " ENCRYPT: " + encrypt;
	}

	/**
	 * Converts host IP address to a String in dotted decimal notation
	 * 
	 * @return String of host IP address in dotted decimal notation
	 */
	public String hostAsString() {

		String toPrint = "";
		long tempHost = host;

		for (int i = 0; i < numOctets; i++) {
			String octet = new String("" + (tempHost % MAX_OCTET));
			if (i == (numOctets - 1)) {
				toPrint = octet + toPrint;
			} else {
				toPrint = "." + octet + toPrint;
			}
			tempHost /= MAX_OCTET;
		}

		return toPrint;
	}

	/**
	 * Converts service type to a String
	 * 
	 * @return String representation of service type
	 */
	private String typeAsString() {
		String toPrint = "";
		if (type == CLIENT) {
			toPrint = "client";
		} else if (type == SERVER) {
			toPrint = "server";
		}
		return toPrint;
	}

	/**
	 * Determines if a given Service object is equal to this Service
	 * 
	 * @param s
	 *            Service object to compare this object to
	 * @return True if this Service and parameter are equal, false otherwise
	 */
	public boolean equals(ZCCService s) {
		boolean retval = false;
		if (s.getName().equals(name) && s.getHost() == host
				&& s.getScheme() == scheme 
				&& s.getPort() == port) {
			retval = true;
		}
		return retval;
	}

	public String getScheme() {
		return scheme;
	}

	private short getPort() {
		return port;
	}

	public String getName() {
		return name;
	}

	private long getHost() {
		return host;
	}

	public boolean isEncrypt() {
		return encrypt;
	}

	public void setEncrypt(boolean encrypt) {
		this.encrypt = encrypt;
	}

}