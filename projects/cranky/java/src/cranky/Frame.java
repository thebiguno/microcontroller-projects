package cranky;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.IOException;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.JToolBar;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import com.codeminders.hidapi.ClassPathLibraryLoader;
import com.codeminders.hidapi.HIDDevice;
import com.codeminders.hidapi.HIDDeviceNotFoundException;
import com.codeminders.hidapi.HIDManager;
import com.jgoodies.binding.value.ValueHolder;
import com.jgoodies.forms.builder.DefaultFormBuilder;
import com.jgoodies.forms.layout.FormLayout;
import com.jgoodies.forms.layout.LayoutMap;

import eu.hansolo.steelseries.gauges.Radial;
import eu.hansolo.steelseries.tools.BackgroundColor;
import eu.hansolo.steelseries.tools.FrameDesign;
import eu.hansolo.steelseries.tools.KnobStyle;
import eu.hansolo.steelseries.tools.KnobType;
import eu.hansolo.steelseries.tools.PointerType;
import eu.hansolo.steelseries.tools.Section;

public class Frame extends JFrame {
	private static final long serialVersionUID = 1L;

	private static final Color BLACK = new Color(41,41,41);
	private static final Color RED = new Color(204,41,41);
	private static final Color ORANGE = new Color(204,122,41);
	private static final Color AZURE = new Color(41,122,204);

	private final SpinnerNumberModel[][] sparkAdvance = new SpinnerNumberModel[8][8];
	private final SpinnerNumberModel[][] injectorDuration = new SpinnerNumberModel[8][8];
	private final JSpinner[][] spinners = new JSpinner[8][8];
	private final SpinnerNumberModel[] loadZones = new SpinnerNumberModel[8];
	private final SpinnerNumberModel[] rpmZones = new SpinnerNumberModel[8];
	private final ValueHolder rpmZone = new ValueHolder();
	private final ValueHolder loadZone = new ValueHolder();
	private final HIDDevice[] device = new HIDDevice[2];

	// simulator
	final SpinnerNumberModel rpmModel = new SpinnerNumberModel(300, 300, 10000, 10);
	final SpinnerNumberModel tpModel = new SpinnerNumberModel(0, 0, 100, 10);


	public Frame() {
		LayoutMap.getRoot().columnPut("label", "l:m");
		
		ClassPathLibraryLoader.loadNativeHIDLibrary();
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setTitle("Cranky");
		setSize(640, 800);
		setBackground(BLACK);
		
		final JToolBar toolbar = new JToolBar();
		toolbar.setFloatable(false);
		toolbar.add(connectEcmAction);
		toolbar.add(connectSimulatorAction);
		
		final JPanel panel = new JPanel(new BorderLayout());
		panel.add(toolbar, BorderLayout.PAGE_START);
		panel.add(layoutTuning(), BorderLayout.PAGE_END);
		panel.add(layoutGuages(), BorderLayout.CENTER);
		panel.add(layoutSimulator(), BorderLayout.LINE_END);

		getContentPane().add(panel);
		setLocationByPlatform(true);
	}
	
	private JPanel layoutGuages() {
		final Radial rpm = new Radial();
		rpm.setTitle("Engine Speed");
		rpm.setUnitString("RPM x 1000");
		rpm.setMaxValue(8);
		rpm.setSections(new Section(6,7,ORANGE),new Section(7,8,RED));
		rpm.setSectionsVisible(true);
		customize(rpm);
		
		final Radial tp = new Radial();
		tp.setTitle("Throttle Position");
		tp.setUnitString("%");
		tp.setMaxValue(100);
		tp.setSections(new Section(80,100,ORANGE));
		tp.setSectionsVisible(true);
		customize(tp);
		
		final Radial dwell = new Radial();
		dwell.setTitle("Dwell");
		dwell.setUnitString("ms");
		dwell.setMaxValue(10);
		dwell.setSections(new Section(6,8,ORANGE), new Section(8,10,RED));
		dwell.setSectionsVisible(true);
		customize(dwell);
		
		final Radial adv = new Radial();
		adv.setDigitalFont(true);
		adv.setTitle("Ignition Advance");
		adv.setUnitString("\u00b0");
		adv.setMaxValue(50);
		customize(adv);
		
		final JPanel result = new JPanel(new GridLayout(2, 4, 5, 5));
		result.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
//		result.setBackground(BLACK);
		result.add(rpm);
		result.add(tp);
		result.add(dwell);
		result.add(adv);

		return result;
	}
	
	private JPanel layoutSimulator() {
		final byte[] bytes = new byte[3];
		
		final ChangeListener listener = new ChangeListener() {
			@Override
			public void stateChanged(ChangeEvent evt) {
				if (device[1] != null) {
					try {
						final int rpm = (Integer) rpmModel.getValue();
						final int tp = (Integer) tpModel.getValue();
						
						// first byte is OCR0A; throttle position
						final byte ocr0a = (byte) (2.55f * tp);
						
						// second and third bytes are OCR1AH and OCR1AL; RPM
						// 500 rpm / 60 = 8.3 Hz = 120 ms / 72 = 1666 us/event = timer1 value of 3332 (120 ms period)
						// 10000 rpm / 60 = 166.6 Hz = 6 ms / 72 = 83 us/event = timer1 value of 166 (6 ms period)
						final double hz = (double) rpm / 60.0d;
						final double ms = 1.0d / hz;
						final double s = ms * 1000000;
						final int ocr1a = (int) s / 36;
						final byte ocr1al = (byte) ocr1a;
						final byte ocr1ah = (byte) (ocr1a >> 8);
						
						bytes[0] = ocr0a;
						bytes[1] = ocr1ah;
						bytes[2] = ocr1al;
						device[1].write(bytes);
					} catch (IOException e) {
						JOptionPane.showMessageDialog(getContentPane(), e.getMessage());
					}
				}
			}
		};
		
		rpmModel.addChangeListener(listener);
		tpModel.addChangeListener(listener);
		
		final DefaultFormBuilder builder = new DefaultFormBuilder(new FormLayout("$label, $lcg, f:p:g"));
		builder.append("RPM", new JSpinner(rpmModel));
		builder.append("Throttle", new JSpinner(tpModel));
		
		return builder.getPanel();
	}
	
	private JPanel layoutTuning() {
		final JPanel grid = new JPanel(new GridLayout(9,9,5,5));
		
		grid.add(new JLabel("TP%\\RPM"));
		
		for (int i = 0; i < 8; i++) {
			final SpinnerNumberModel rpmZone = new SpinnerNumberModel(300, 300, 9000, 10);
			rpmZones[i] = rpmZone;
			grid.add(new JSpinner(rpmZone));
		}
		
		for (int i = 0; i < 8; i++) {
			final SpinnerNumberModel loadZone = new SpinnerNumberModel(0, 0, 100, 10);
			loadZones[i] = loadZone;
			grid.add(new JSpinner(loadZone));
			
			for (int j = 0; j < 8; j++) {
				final SpinnerNumberModel advance = new SpinnerNumberModel(0, 0, 59, 1);
				final SpinnerNumberModel duration = new SpinnerNumberModel(0, 0, 59, 1);
				final JSpinner spinner = new JSpinner(advance);
				sparkAdvance[i][j] = advance;
				injectorDuration[i][j] = duration;
				spinners[i][j] = spinner;
				grid.add(spinner);
			}
		}
		
		final PropertyChangeListener l = new PropertyChangeListener() {
			@Override
			public void propertyChange(PropertyChangeEvent evt) {
				for (int i = 0; i < 8; i++) {
					for (int j = 0; j < 8; j++) {
						spinners[i][j].setBackground(Color.WHITE);
					}
				}
				int i = (Integer) rpmZone.getValue();
				int j = (Integer) loadZone.getValue();
				spinners[i][j].setBackground(ORANGE);
			}
		};
		rpmZone.addValueChangeListener(l);
		loadZone.addValueChangeListener(l);
		
		final JComboBox<String> display = new JComboBox<String>(new String[] { "advance", "duration" });
		display.setRenderer(new DefaultListCellRenderer() {
			private static final long serialVersionUID = 1L;

			@Override
			public Component getListCellRendererComponent(JList<?> list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
				final JLabel result = (JLabel) super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);
				if ("advance".equals(value)) {
					result.setText("Spark Advance (degrees)");
				} else if ("duration".equals(value)) {
					result.setText("Ignition Duration (ms)");
				}
				return result;
			}
		});

		final JPanel result = new JPanel(new BorderLayout());
		result.add(grid, BorderLayout.CENTER);
		result.add(display, BorderLayout.PAGE_END);
		return result;
	}
	
	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				final Frame f = new Frame();
				f.setVisible(true);
			}
		});
	}
	
	private static void customize(Radial r) {
		r.setDigitalFont(true);
		r.setBackgroundColor(BackgroundColor.BEIGE);
		r.setFrameDesign(FrameDesign.GLOSSY_METAL);
		r.setPointerType(PointerType.TYPE2);
		r.setKnobType(KnobType.BIG_STD_KNOB);
		r.setKnobStyle(KnobStyle.BLACK);
		r.setFrameVisible(true);
	}
	
	private AbstractAction connectEcmAction = new AbstractAction() {
		private static final long serialVersionUID = 1L;
		{
			putValue(NAME, "Connect ECM");
		}
		@Override
		public void actionPerformed(ActionEvent evt) {
			try {
				if (device[0] == null) {
					device[0] = HIDManager.getInstance().openById(0x574a, 0x6372, null);
					putValue(NAME, "Disconnect ECM");
				} else {
					device[0].close();
					putValue(NAME, "Connect ECM");
				}
			} catch (HIDDeviceNotFoundException e) {
				JOptionPane.showMessageDialog(getContentPane(), "ECM not found");
			} catch (IOException e) {
				e.printStackTrace();
				JOptionPane.showMessageDialog(getContentPane(), e.getMessage());
			}
		}
	};
	
	private AbstractAction connectSimulatorAction = new AbstractAction() {
		private static final long serialVersionUID = 1L;
		{
			putValue(NAME, "Connect Simulator");
		}
		@Override
		public void actionPerformed(ActionEvent evt) {
			try {
				if (device[1] == null) {
					device[1] = HIDManager.getInstance().openById(0x574a, 0x6373, null);
					rpmModel.setValue(300);
					tpModel.setValue(0);
					putValue(NAME, "Disconnect Simulator");
				} else {
					device[1].close();
					putValue(NAME, "Connect Simulator");
				}
			} catch (HIDDeviceNotFoundException e) {
				JOptionPane.showMessageDialog(getContentPane(), "Simulator not found");
			} catch (IOException e) {
				e.printStackTrace();
				JOptionPane.showMessageDialog(getContentPane(), e.getMessage());
			}
		}
	};
}
