import java.util.Vector;

import javax.swing.JComboBox;
import javax.swing.JOptionPane;

/**
 * ZComboBox.java
 * 
 * This object extends the functionality of JComboBox. It adds a Vector of the
 * items that are currently in the drop down box, which allows for easier
 * adding, searching, and deleting. Each time the list of items changes, the
 * drop down box is updated accordingly.
 * 
 * @author Zachary M. Allen
 * @version 1.0
 */
public class ZComboBox extends JComboBox {
	
	/**
	 * Serial Version UID
	 */
	private static final long serialVersionUID = -8431766385980604423L;
	
	/**
	 * Items that populate the drop down box
	 */
	private Vector items;

	/**
	 * Constructor
	 */
	public ZComboBox() {
		this.items = new Vector();
	}

	/**
	 * This method is used to add an item to the item list. Upon adding an
	 * item, the associated drop down box is updated. There is a boolean
	 * paramter which turns error supression on/off. This is included because
	 * duplicate values are possible during initialization, but should be
	 * avoided during standard usage.
	 * 
	 * @param    toAdd    Item to add to the box
	 * @param    supressErrors    True if error messages should be suppressed, false otherwise
	 */
	public void add(String toAdd, boolean supressErrors) {

		boolean alreadyInList = false;

		if (toAdd.length() == 0) {
			JOptionPane.showMessageDialog(null, "Invalid item name", "Error",
					JOptionPane.ERROR_MESSAGE);
			return;
		}

		for (int i = 0; i < items.size(); i++) {
			if (toAdd.equals((String) items.elementAt(i))) {
				alreadyInList = true;
			}
		}

		if (alreadyInList) {
			if (!supressErrors) {
				JOptionPane.showMessageDialog(null,
						"That item is already in the list", "Error",
						JOptionPane.ERROR_MESSAGE);
			}
		} else {
			items.add(toAdd);
		}

		refresh();

	}

	/**
	 * Refreshes the drop down box to match the contents of the item list
	 */
	private void refresh() {
		this.removeAllItems();
		for (int i = 0; i < items.size(); i++) {
			this.addItem((String) items.elementAt(i));
		}
	}

}