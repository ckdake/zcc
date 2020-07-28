import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

/**
 * AddItemFrame.java
 * 
 * This class represents the frame that a user uses to add a new Service
 * object to the list of those that utilize the ZCC protcol for security.
 * This frame allows a user a general interface for entering a new item into
 * a ZComboBox drop down box.
 * 
 * @author Zachary M. Allen
 * @version 1.0
 */
public class AddItemFrame extends JFrame {

	/**
	 * Serial Version UID
	 */
	private static final long serialVersionUID = 5761380924488280258L;

	/**
	 * The text field used by the user to enter a new item
	 */
	private final JTextField itemNameField;

	/**
	 * Getter returns value of itemNameField
	 * @return    Current value of itemNameField
	 */
	public JTextField getItemNameField() {
		return itemNameField;
	}

	/**
	 * Constructor
	 * 
	 * @param    addItemTitle    The title of the add item frame
	 * @param    itemBox    The ZComboBox that the item will be added to
	 */
	public AddItemFrame(String addItemTitle, final ZComboBox itemBox) {

		super(addItemTitle);

		final JFrame me = this;

		itemNameField = new JTextField();

		JButton addItemButton = new JButton("Submit");
		addItemButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				itemBox.add(itemNameField.getText(), false);
				me.dispose();
			}
		});

		JPanel addServicePanel = new JPanel(new GridLayout(1, 3));
		addServicePanel.add(new JLabel("Service:"));
		addServicePanel.add(itemNameField);
		addServicePanel.add(addItemButton);

		this.getContentPane().add(addServicePanel);
		this.pack();
		this.setResizable(false);
	}

}
