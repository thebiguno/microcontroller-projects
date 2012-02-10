import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
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
						if (checkBox.isSelected()) value = value.setBit(h);
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
		
		final JPanel buttonPanel = new JPanel(new BorderLayout());
		
		final JButton submit = new JButton("Set");
		submit.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
//				String t = valueField.getText();
//				t = t.replaceAll("0x", "").replaceAll(",", "");
//				value = new BigInteger(t, 16);
//				
//				for (int i = 0; i < width * height; i++){
//					JCheckBox item = (JCheckBox) checkPanel.getComponent(i);
//					item.setSelected(value.testBit(item.getIconTextGap()));
//				}
			}
		});
		buttonPanel.add(valueField, BorderLayout.CENTER);
		buttonPanel.add(submit, BorderLayout.EAST);
		
		this.setLayout(new BorderLayout());
		this.add(row, BorderLayout.CENTER);
		this.add(buttonPanel, BorderLayout.SOUTH);
				
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
