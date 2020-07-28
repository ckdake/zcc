import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.List;
import java.awt.Toolkit;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.FactoryConfigurationError;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/**
 * KeyClient.java
 * 
 * KeyClient is a Java program which provides a user interface for configuring
 * the services that will utilize the ZCC protocol. Upon initialization,
 * KeyClient loads an XML file containing service configuration information.
 * KeyClient's graphical interface allows users to add services to be encrypted
 * with ZCC, remove services from being encrypted with ZCC, and toggle services
 * between being encrypted and not being encrypted. This configuration can be
 * saved by exporting it to the XML configuration file. Upon exporting, the
 * zcc_runner service on a user's system is notifed that a configuration update
 * has occured, and the zcc_runner acts accordingly.
 * 
 * @author Zachary M. Allen
 * @version 1.0
 * 
 */
public class KeyClient extends javax.swing.JFrame {

	/**
	 * Serial Version UID
	 */
	private static final long serialVersionUID = 5890656381083259517L;

	/**
	 * Width of frame
	 */
	private static final int WIDTH = 500;

	/**
	 * Height of frame
	 */
	private static final int HEIGHT = 300;

	/**
	 * Horizontal position of frame
	 */
	private static final int X_POSITION = 320;

	/**
	 * Vertical postiion of frame
	 */
	private static final int Y_POSITION = 180;

	/**
	 * XML configuration file to retrieve
	 */
	private static final String CONFIGURATION_FILE = "services.xml";

	/**
	 * String constant for parsing
	 */
	private static final String SERVICE = "SERVICE:";

	/**
	 * String constant for parsing
	 */
	private static final String HOST = "HOST:";

	/**
	 * String constant for parsing
	 */
	private static final String PORT = "PORT:";

	/**
	 * String constant for parsing
	 */
	private static final String TYPE = "TYPE:";

	/**
	 * String constant for parsing
	 */
	private static final String SCHEME = "SCHEME:";

	/**
	 * String constant for parsing
	 */
	private static final String ENCRYPT = "ENCRYPT:";

	/**
	 * Name of icon image file
	 */
	private static final String ICON = "key.jpg";

	/**
	 * Vector which holds service objects
	 */
	private Vector serviceVector;

	/**
	 * Icon image for all frames
	 */
	private Image iconImage;

	/**
	 * AWT List for holding Services
	 */
	private final List serviceList;

	/**
	 * Constructor
	 * 
	 * @param title
	 *            Title of frame
	 */
	public KeyClient(String title) {

		serviceList = new List();

		/* Add icon */
		iconImage = Toolkit.getDefaultToolkit().getImage(
				java.net.URLClassLoader.getSystemResource(ICON));

		/* Set frame variables */
		this.setTitle(title);
		this.setSize(WIDTH, HEIGHT);
		this.setLocation(X_POSITION, Y_POSITION);
		this.setIconImage(iconImage);

		/* Add event listener to terminate program upon close */
		this.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				final Window w = e.getWindow();
				w.setVisible(false);
				w.dispose();
				System.exit(0);
			}
		});

		/* Populate the hostVector instance variable */
		serviceVector = this.populateList(this.loadDocument());

		/* Create a button to add a new entry */
		JButton addButton = new JButton("Add Entry");
		addButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {

				final JFrame addEntryFrame = new JFrame();
				addEntryFrame.setTitle("Add Entry");
				addEntryFrame.setLocation(X_POSITION, Y_POSITION + 500);
				addEntryFrame.addWindowListener(new WindowAdapter() {
					public void windowClosing(WindowEvent e) {
						final Window w = e.getWindow();
						w.setVisible(false);
						w.dispose();
					}
				});

				final ZComboBox serviceBox = new ZComboBox();
				final ZComboBox hostBox = new ZComboBox();
				final ZComboBox schemeBox = new ZComboBox();
				final JTextField portField = new JTextField();
				for (int i = 0; i < serviceVector.size(); i++) {
					ZCCService service = (ZCCService) serviceVector
							.elementAt(i);
					serviceBox.add(service.getName(), true);
					hostBox.add(service.hostAsString(), true);
					schemeBox.add(service.getScheme(), true);
				}

				JButton addServiceButton = new JButton("Add Service");
				JButton addHostButton = new JButton("Add Host");
				JButton addSchemeButton = new JButton("Add Scheme");
				JButton submitButton = new JButton("Submit");

				JPanel dropDownPanel = new JPanel(new GridLayout(7, 3));
				dropDownPanel.add(new JLabel(SERVICE));
				dropDownPanel.add(serviceBox);
				dropDownPanel.add(addServiceButton);
				dropDownPanel.add(new JLabel(HOST));
				dropDownPanel.add(hostBox);
				dropDownPanel.add(addHostButton);
				dropDownPanel.add(new JLabel(PORT));
				dropDownPanel.add(portField);
				dropDownPanel.add(new JLabel(""));
				dropDownPanel.add(new JLabel(TYPE));

				final JComboBox typeBox = new JComboBox();
				typeBox.addItem(ZCCService.T_CLIENT);
				typeBox.addItem(ZCCService.T_SERVER);

				final JCheckBox encryptBox = new JCheckBox("", true);

				dropDownPanel.add(typeBox);
				dropDownPanel.add(new JLabel(""));

				dropDownPanel.add(new JLabel(SCHEME));
				dropDownPanel.add(schemeBox);
				dropDownPanel.add(addSchemeButton);
				
				dropDownPanel.add(new JLabel(ENCRYPT));
				dropDownPanel.add(encryptBox);
				dropDownPanel.add(new JLabel(""));
				dropDownPanel.add(new JLabel(""));
				dropDownPanel.add(submitButton);
				dropDownPanel.add(new JLabel(""));

				addServiceButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						AddItemFrame addServiceFrame = new AddItemFrame(
								"Add Service", serviceBox);
						addServiceFrame.setLocation(X_POSITION,
								Y_POSITION + 400);
						addServiceFrame.setIconImage(iconImage);
						addServiceFrame.setVisible(true);
					}
				});

				addHostButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						final AddItemFrame addHostFrame = new AddItemFrame(
								"Add Host", hostBox);
						addHostFrame.setLocation(X_POSITION, Y_POSITION + 400);
						addHostFrame.setIconImage(iconImage);
						addHostFrame.setVisible(true);
					}
				});

				addSchemeButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						final AddItemFrame addSchemeFrame = new AddItemFrame(
								"Add Scheme", schemeBox);
						addSchemeFrame.setLocation(X_POSITION, Y_POSITION + 400);
						addSchemeFrame.setIconImage(iconImage);
						addSchemeFrame.setVisible(true);
					}
				});

				submitButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {

						int portNumber = 0;
						try {
							portNumber = (new Integer(portField.getText()))
									.intValue();
						} catch (NumberFormatException nfe) {
							errorMessage("Invalid port number");
							return;
						}
						if (portNumber < 0 || portNumber > 65535) {
							errorMessage("Invalid port number");
							return;
						}

						ZCCService s = null;
						s = new ZCCService((String) serviceBox
								.getSelectedItem(), (String) hostBox
								.getSelectedItem(), (short) portNumber,
								(String) typeBox.getSelectedItem(),
								(String) schemeBox.getSelectedItem(),
								encryptBox.isSelected());

						boolean isDuplicateEntry = false;
						for (int i = 0; i < serviceVector.size(); i++) {
							if (s.equals((ZCCService) serviceVector
									.elementAt(i))) {
								isDuplicateEntry = true;
							}
						}

						if (isDuplicateEntry) {
							errorMessage("Duplicate Entry");
						} else {
							serviceVector.add(s);
						}

						refreshList();
						addEntryFrame.dispose();

					}
				});

				JPanel addEntryMainPanel = new JPanel(new FlowLayout());
				addEntryMainPanel.add(dropDownPanel);

				/* Add panels to the frame */
				addEntryFrame.getContentPane().setLayout(new BorderLayout());
				addEntryFrame.getContentPane().add(addEntryMainPanel,
						BorderLayout.CENTER);

				addEntryFrame.setIconImage(iconImage);
				addEntryFrame.pack();
				addEntryFrame.setResizable(false);
				addEntryFrame.setVisible(true);
			}
		});

		/* Create a button to remove all selected entries in the list */
		JButton deleteButton = new JButton("Remove Selected");
		deleteButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				int[] selectedIndices = serviceList.getSelectedIndexes();
				for (int i = 0; i < selectedIndices.length; i++) {
					deleteEntry(selectedIndices[i]);
					refreshList();
				}
			}
		});

		/*
		 * Create a button to toggle encryption for all selected entries in the
		 * list
		 */
		JButton toggleButton = new JButton("Toggle Selected");
		toggleButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				int[] selectedIndices = serviceList.getSelectedIndexes();
				for (int i = 0; i < selectedIndices.length; i++) {
					toggleEntry(selectedIndices[i]);
					refreshList();
				}
			}
		});

		/* Create a button to export the current list to XML */
		JButton exportButton = new JButton("Export to XML");
		exportButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
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
							CONFIGURATION_FILE));
					bw.write(toExport);
					bw.close();
				} catch (IOException e1) {
					errorMessage("Could not export to XML");
				}
				// Signal
			}
		});

		/* Create a panel on which to stick buttons */
		JPanel southPanel = new JPanel(new FlowLayout());
		southPanel.add(addButton);
		southPanel.add(toggleButton);
		southPanel.add(deleteButton);
		southPanel.add(exportButton);

		/* Add panels to the frame */
		this.getContentPane().setLayout(new BorderLayout());
		this.getContentPane().add(southPanel, BorderLayout.SOUTH);
		this.getContentPane().add(serviceList, BorderLayout.CENTER);
		this.setResizable(false);

		refreshList();

	}

	/**
	 * Refreshes the serviceList based on what is currently in the serviceVector
	 */
	private void refreshList() {
		serviceList.removeAll();
		for (Enumeration serviceEnum = serviceVector.elements(); serviceEnum
				.hasMoreElements();) {
			ZCCService service = (ZCCService) serviceEnum.nextElement();
			serviceList.add(service.toString());
		}
		serviceList.repaint();
	}

	/**
	 * Toggles encryption for an entry from the serviceList
	 * 
	 * @param entry
	 *            Index in serviceList of entry to toggle
	 */
	private void toggleEntry(int entry) {
		String compString = serviceList.getItem(entry);
		for (int i = 0; i < serviceVector.size(); i++) {
			ZCCService thisService = (ZCCService) serviceVector.elementAt(i);
			if (compString.equals(thisService.toString())) {
				thisService.setEncrypt(!thisService.isEncrypt());
			}
		}
	}

	/**
	 * Deletes an entry from the serviceList
	 * 
	 * @param entry
	 *            Index in serviceList of entry to delete
	 */
	private void deleteEntry(int entry) {
		String compString = serviceList.getItem(entry);
		for (int i = 0; i < serviceVector.size(); i++) {
			ZCCService thisService = (ZCCService) serviceVector.elementAt(i);
			if (compString.equals(thisService.toString())) {
				serviceVector.removeElementAt(i);
			}
		}
	}

	/**
	 * Populates List from XML configuration file
	 * 
	 * @param document
	 *            XML document
	 * @return Vector of Service objects loaded from XML configuration file
	 */
	private Vector populateList(Document document) {

		Vector serviceList = new Vector();
		NodeList serviceNodes = document.getChildNodes().item(1)
				.getChildNodes();

		for (int i = 0; i < serviceNodes.getLength(); i++) {

			Node serviceNode = serviceNodes.item(i);
			NamedNodeMap serviceMap = serviceNode.getAttributes();

			if (serviceMap == null) {
				continue;
			}

			String service = (serviceMap.getNamedItem("name")).getNodeValue();
			String host = (serviceMap.getNamedItem("host")).getNodeValue();
			short port = (new Short((serviceMap.getNamedItem("port"))
					.getNodeValue())).shortValue();
			String type = (serviceMap.getNamedItem("type")).getNodeValue();
			String scheme = (serviceMap.getNamedItem("scheme")).getNodeValue();
			boolean encrypt = (new Boolean((serviceMap.getNamedItem("encrypt"))
					.getNodeValue())).booleanValue();

			ZCCService newService = new ZCCService(service, host, port, type, scheme,
					encrypt);
			serviceList.add(newService);

		}

		return serviceList;

	}

	/**
	 * Loads XML file with services, hosts, ports, types, and encryption
	 * 
	 * @return XML Document object
	 */
	private Document loadDocument() {
		Document xmlDocument = null;
		try {
			xmlDocument = DocumentBuilderFactory.newInstance()
					.newDocumentBuilder().parse(CONFIGURATION_FILE);
		} catch (SAXException e1) {
			e1.printStackTrace();
		} catch (IOException e1) {
			e1.printStackTrace();
		} catch (ParserConfigurationException e1) {
			e1.printStackTrace();
		} catch (FactoryConfigurationError e1) {
			e1.printStackTrace();
		}
		return xmlDocument;
	}

	/**
	 * Main method which runs KeyClient program.
	 * 
	 * @param args
	 *            Command line arguments
	 */
	public static void main(final String[] args) {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} catch (InstantiationException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (UnsupportedLookAndFeelException e) {
			e.printStackTrace();
		}
		final KeyClient keyClientFrame = new KeyClient("ZCC Key Client");
		keyClientFrame.setVisible(true);
	}

	/**
	 * Displays an error message when an exception is caught
	 * 
	 * @param string
	 *            Error message to display
	 */
	public static void errorMessage(String string) {
		JOptionPane.showMessageDialog(null, string, "Error",
				JOptionPane.ERROR_MESSAGE);
	}

}