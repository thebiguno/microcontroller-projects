package cranky;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.IOException;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

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
import com.jgoodies.binding.adapter.SpinnerAdapterFactory;
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
//	private static final Color AZURE = new Color(41,122,204);
	
	// tuning
	private int IGN_ADVANCE = 0x00;
	private int INJ_DURATION = 0x40;
	private int LOAD_ZONES = 0x80;
	private int RPM_ZONES = 0x88;
	
	// unused tuning
//	private int CRANKING_TICKS = 0x90;
//	private int MAX_IGN_DWELL = 0x91;

	// running
	private int LOAD_ZONE = 0x92;
	private int RPM_ZONE = 0x93;
	private int ADC_TP = 0x94;
	private int IGN_ADV_DEG = 0x95;
	private int IGN_DWELL = 0x96;
	private int CRANK_TICKS = 0x97;

	// unused running
//	private int INJ = 0x98;
//	private int CAM = 0x99;
//	private int CRANK = 0x9a;
//	private int IGN = 0x9b;
//	private int INJ_DC = 0x9c;
//	private int CRANK_SYNC = 0x9d;
//	private int CRANKING = 0x9e;

	private final Executor executor = Executors.newSingleThreadExecutor();
	private final HIDDevice[] device = new HIDDevice[2];
	private final ValueHolder[] values = new ValueHolder[158];
	private final JSpinner[][] spinners = new JSpinner[8][8];
//	private final SpinnerNumberModel[][] sparkAdvance = new SpinnerNumberModel[8][8];
//	private final SpinnerNumberModel[][] injectorDuration = new SpinnerNumberModel[8][8];
//	private final SpinnerNumberModel[] loadZones = new SpinnerNumberModel[8];
//	private final SpinnerNumberModel[] rpmZones = new SpinnerNumberModel[8];
//	private final ValueHolder dwellModel = new ValueHolder();
//	private final ValueHolder advModel = new ValueHolder();
//	private final ValueHolder rpmZoneModel = new ValueHolder();
//	private final ValueHolder loadZoneModel = new ValueHolder();

//	private final ValueHolder rpmModel = new ValueHolder();
//	private final ValueHolder tpModel = new ValueHolder();
	
	private final SpinnerNumberModel simulatorRpmModel = new SpinnerNumberModel(300, 300, 10000, 10);
	private final SpinnerNumberModel simulatorTpModel = new SpinnerNumberModel(0, 0, 100, 1);
	
	private byte[] rxBuffer = new byte[64];
	
	public Frame() {
		LayoutMap.getRoot().columnPut("label", "l:m");
		
		ClassPathLibraryLoader.loadNativeHIDLibrary();
		
		for (int i = 0; i < values.length; i++) {
			values[i] = new ValueHolder();
		}
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setTitle("Cranky");
		setSize(1024, 768);
		setBackground(BLACK);
		
		final JToolBar toolbar = new JToolBar();
		toolbar.setFloatable(false);
		toolbar.add(connectEcmAction);
		toolbar.add(connectSimulatorAction);
		
		final JPanel panel = new JPanel(new BorderLayout());
		panel.add(toolbar, BorderLayout.PAGE_START);
		panel.add(layoutTuning(), BorderLayout.PAGE_END);
		panel.add(layoutGuages(), BorderLayout.CENTER);

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
		
		values[CRANK_TICKS].addValueChangeListener(new PropertyChangeListener() {
			@Override
			public void propertyChange(PropertyChangeEvent evt) {
				rpm.setValue(26041d / (0xFF & (Integer) evt.getNewValue()));
			}
		});
		
		final Radial tp = new Radial();
		tp.setTitle("Throttle Position");
		tp.setUnitString("%");
		tp.setMaxValue(100);
		tp.setThreshold(80);
		tp.setSections(new Section(80,100,ORANGE));
		tp.setSectionsVisible(true);
		customize(tp);
		
		values[ADC_TP].addValueChangeListener(new PropertyChangeListener() {
			@Override
			public void propertyChange(PropertyChangeEvent evt) {
				tp.setValue(((Integer) evt.getNewValue()).doubleValue() / 2.55d);
			}
		});

		final Radial dwell = new Radial();
		dwell.setTitle("Dwell");
		dwell.setUnitString("ms");
		dwell.setMaxValue(10);
		dwell.setSections(new Section(6,8,ORANGE), new Section(8,10,RED));
		dwell.setSectionsVisible(true);
		customize(dwell);
		
		values[IGN_DWELL].addValueChangeListener(new PropertyChangeListener() {
			@Override
			public void propertyChange(PropertyChangeEvent evt) {
				// TODO
				dwell.setValue((Integer) evt.getNewValue());
			}
		});

		final Radial adv = new Radial();
		adv.setDigitalFont(true);
		adv.setTitle("Ignition Advance");
		adv.setUnitString("\u00b0");
		adv.setMaxValue(50);
		customize(adv);
		
		values[IGN_ADV_DEG].addValueChangeListener(new PropertyChangeListener() {
			@Override
			public void propertyChange(PropertyChangeEvent evt) {
				adv.setValue((Integer) evt.getNewValue());
			}
		});

		final JPanel result = new JPanel(new GridLayout(1, 4, 5, 5));
		result.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
//		result.setBackground(BLACK);
		result.add(rpm);
		result.add(tp);
		result.add(dwell);
		result.add(adv);

		return result;
	}
	
	private JPanel layoutTuning() {
		final JPanel grid = new JPanel(new GridLayout(9,9,5,5));
		grid.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

		grid.add(new JLabel("TP%\\RPM"));
		
		for (int i = 0; i < 8; i++) {
			final int rpmZoneIndex = RPM_ZONES + i;
			final ValueHolder rpmZone = values[rpmZoneIndex];
			rpmZone.addValueChangeListener(new PropertyChangeListener() {
				@Override
				public void propertyChange(PropertyChangeEvent evt) {
					final byte[] b = new byte[3];
					b[0] = (byte) rpmZoneIndex;
					b[1] = 1;
					b[2] = (byte) (26041d / (Integer) rpmZone.getValue());
					executor.execute(new Writer(device[0], b));
				}
			});
			grid.add(new JSpinner(SpinnerAdapterFactory.createNumberAdapter(rpmZone, 300, 300, 10000, 10)));
		}
		
		for (int i = 0; i < 8; i++) {
			final int loadZoneIndex = LOAD_ZONES + i;
			final ValueHolder loadZone = values[loadZoneIndex];
			loadZone.addValueChangeListener(new PropertyChangeListener() {
				@Override
				public void propertyChange(PropertyChangeEvent evt) {
					final byte[] b = new byte[3];
					b[0] = (byte) loadZoneIndex;
					b[1] = 1;
					b[2] = (byte) (2.55d * (Integer) loadZone.getValue());
					executor.execute(new Writer(device[0], b));
				}
			});
			grid.add(new JSpinner(SpinnerAdapterFactory.createNumberAdapter(loadZone, 300, 300, 10000, 10)));
			
			for (int j = 0; j < 8; j++) {
				final int advanceIndex = IGN_ADVANCE+(i*8)+j;
				final int durationIndex = INJ_DURATION+(i*8)+j;
				final ValueHolder advance = values[advanceIndex];
				final ValueHolder duration = values[durationIndex];
				advance.addPropertyChangeListener(new PropertyChangeListener() {
					@Override
					public void propertyChange(PropertyChangeEvent evt) {
						final byte[] b = new byte[3];
						b[0] = (byte) advanceIndex;
						b[1] = 1;
						b[2] = (byte) (int) (Integer) advance.getValue();
						executor.execute(new Writer(device[0], b));
					}
				});
				duration.addPropertyChangeListener(new PropertyChangeListener() {
					@Override
					public void propertyChange(PropertyChangeEvent evt) {
						final byte[] b = new byte[3];
						b[0] = (byte) durationIndex;
						b[1] = 1;
						b[2] = (byte) (int) (Integer) duration.getValue();
						executor.execute(new Writer(device[0], b));
					}
				});
				final SpinnerNumberModel advanceModel = SpinnerAdapterFactory.createNumberAdapter(advance, 0, 0, 59, 1);
				final SpinnerNumberModel durationModel = SpinnerAdapterFactory.createNumberAdapter(duration, 0, 0, 59, 1);
				final JSpinner spinner = new JSpinner(advanceModel);
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
				//int i = (Integer) values[RPM_ZONE].getValue();
				//int j = (Integer) values[LOAD_ZONE].getValue();
				//spinners[i][j].setBackground(ORANGE);
			}
		};
		values[RPM_ZONE].addValueChangeListener(l);
		values[LOAD_ZONE].addValueChangeListener(l);
		
		final JComboBox<String> display = new JComboBox<String>(new String[] { "advance", "duration" });
		display.setRenderer(new DefaultListCellRenderer() {
			private static final long serialVersionUID = 1L;

			@Override
			public Component getListCellRendererComponent(JList<?> list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
				final JLabel result = (JLabel) super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);
				if ("advance".equals(value)) {
					result.setText("Spark Advance (degrees)");
				} else if ("duration".equals(value)) {
					result.setText("Injection Duration (ms)");
				}
				return result;
			}
		});
		
		
		// simulator
		
		final ChangeListener listener = new ChangeListener() {
			@Override
			public void stateChanged(ChangeEvent evt) {
				if (device[1] != null) {
					final int rpm = (Integer) simulatorRpmModel.getValue();
					final int tp = (Integer) simulatorTpModel.getValue();
					
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
					
					final byte[] b = new byte[3];
					b[0] = ocr0a;
					b[1] = ocr1ah;
					b[2] = ocr1al;
					executor.execute(new Writer(device[1], b));
				}
			}
		};
		
		simulatorRpmModel.addChangeListener(listener);
		simulatorTpModel.addChangeListener(listener);
		
		final DefaultFormBuilder builder = new DefaultFormBuilder(new FormLayout("$label, $lcg, f:p:g"));
		builder.append("Tune", display);
		builder.append("RPM", new JSpinner(simulatorRpmModel));
		builder.append("Throttle", new JSpinner(simulatorTpModel));
		builder.getPanel().setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

		final JPanel result = new JPanel(new BorderLayout());
		result.add(grid, BorderLayout.CENTER);
		result.add(builder.getPanel(), BorderLayout.PAGE_END);
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
		private Thread thread;
		{
			putValue(NAME, "Connect ECM");
		}
		@Override
		public void actionPerformed(ActionEvent evt) {
			try {
				if (device[0] == null) {
					device[0] = HIDManager.getInstance().openById(0x574a, 0x6372, null);
					thread = new Thread(new Runnable() {
						@Override
						public void run() {
							final Thread thisThread = Thread.currentThread();
							while (thread == thisThread) {
								if (device[0] == null) {
									thread = null;
									putValue(NAME, "Connect ECM");
								}
								try {
									int read = device[0].read(rxBuffer);
									if (read > 0) {
										int addr = 0xFF & rxBuffer[0];
										int len = 0xFF & rxBuffer[1];
										if ((len & 0x80) == 0x80) {
											// read request
											// the ECM doesn't send read requests
										} else {
											// write request
											for (int i = 0; i < len; i++) {
												int b = 0xFF & rxBuffer[i+2];
												values[addr+i].setValue(b);
												
//												System.out.print(b);
//												System.out.print(", ");
											}
//											System.out.println();
										}
									}
								} catch (IOException e) {
									device[0] = null;
								}
							}
						}
					});
					thread.start();
					putValue(NAME, "Disconnect ECM");
				} else {
					thread = null;
					try { device[0].close(); } catch (Throwable t) {}
					device[0] = null;
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
					simulatorRpmModel.setValue(300);
					simulatorTpModel.setValue(0);
					putValue(NAME, "Disconnect Simulator");
				} else {
					try { device[1].close(); } catch (Throwable t) {}
					device[1] = null;
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
	
	class Writer implements Runnable {
		private final byte[] b;
		private final HIDDevice device;
		
		public Writer(HIDDevice device, byte[] b) {
			this.device = device;
			this.b = b;
		}
		
		@Override
		public void run() {
			try {
				device.write(b);
			} catch (IOException e) {
				try { device.close(); } catch (Throwable t) {}
				e.printStackTrace();
			}
		}
		
	};
}
