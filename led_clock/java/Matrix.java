import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.math.BigInteger;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;

public class Matrix extends JFrame {
	private static final long serialVersionUID = 1L;

	public Matrix() {
		final JTextField valueField = new JTextField();
		final JPanel row = new JPanel(new GridLayout(1, 8));

		final ActionListener listener = new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				final StringBuilder sb = new StringBuilder();

				for (int w = 0; w < 8; w++) {
					final JPanel col = (JPanel) row.getComponent(w);
					BigInteger value = new BigInteger("0");
					
					for (int h = 0; h < 8; h++) {
						final JCheckBox checkBox = (JCheckBox) col.getComponent(h);
						if (checkBox.isSelected()) value = value.setBit(7-h);
					}

					if (w > 0) sb.append(", ");
					sb.append("0x");
					sb.append(value.toString(16));
				}
				
				valueField.setText(sb.toString());
			}
		};
		
		for (int w = 0; w < 8; w++) {
			final JPanel col = new JPanel(new GridLayout(8,1));
			for (int h = 0; h < 8; h++) {
				final JCheckBox checkBox = new JCheckBox();
				checkBox.addActionListener(listener);
				col.add(checkBox);
			}
			row.add(col);
		}
		
		valueField.addKeyListener(new KeyAdapter() {
			@Override
			public void keyTyped(KeyEvent e) {
				try {
					final String t = valueField.getText();
					final String[] a = t.split(", ");
				
					for (int w = 0; w < a.length; w++) {
						final BigInteger v = new BigInteger(a[w].replaceAll("0x",""),16);
						final JPanel col = (JPanel) row.getComponent(w);
						for (int h = 0; h < 8; h++) {
							final JCheckBox checkBox = (JCheckBox) col.getComponent(h);
							checkBox.setSelected(v.testBit(7-h));
						}
					}
				} catch (Throwable t) {
					;
				}
			}
		});
		this.setLayout(new BorderLayout());
		this.add(row, BorderLayout.CENTER);
		this.add(valueField, BorderLayout.SOUTH);
				
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setLocationRelativeTo(null);
		this.pack();
		this.setVisible(true);
	}
	
	public static void main(String[] args) throws Exception {
		SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				new Matrix();
			}
		});
	}	
}
