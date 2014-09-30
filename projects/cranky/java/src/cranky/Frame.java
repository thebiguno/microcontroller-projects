package cranky;

import java.awt.Color;
import java.awt.GridLayout;
import java.awt.LayoutManager;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;

import eu.hansolo.steelseries.gauges.Radial;
import eu.hansolo.steelseries.tools.BackgroundColor;
import eu.hansolo.steelseries.tools.FrameDesign;
import eu.hansolo.steelseries.tools.KnobStyle;
import eu.hansolo.steelseries.tools.KnobType;
import eu.hansolo.steelseries.tools.PointerType;
import eu.hansolo.steelseries.tools.Section;

public class Frame extends JFrame {
	private static final long serialVersionUID = 1L;

	private static final Color RED = new Color(220,49,47);
	private static final Color YELLOW = new Color(181,137,24);
	private static final Color BLUE = new Color(38,139,210);

	public Frame() {
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setTitle("Cranky");
		setSize(1000, 500);
		setBackground(Color.BLACK);
		
		final Radial rpm = new Radial();
		rpm.setTitle("Engine Speed");
		rpm.setUnitString("RPM x 1000");
		rpm.setMaxValue(10);
		rpm.setSections(new Section(5,7,YELLOW),new Section(7,10,RED));
		rpm.setSectionsVisible(true);
		customize(rpm);
		
		final Radial tp = new Radial();
		tp.setTitle("Throttle Position");
		tp.setUnitString("%");
		tp.setMaxValue(100);
		tp.setSections(new Section(80,100,YELLOW));
		tp.setSectionsVisible(true);
		customize(tp);
		
		final Radial map = new Radial();
		map.setTitle("MAP");
		map.setUnitString("kPa");
		map.setMaxValue(300);
		map.setSections(new Section(200,250,YELLOW), new Section(250,300,RED));
		map.setSectionsVisible(true);
		customize(map);
		
		final Radial batt = new Radial();
		batt.setTitle("Battery");
		batt.setUnitString("V");
		batt.setMinValue(8);
		batt.setMaxValue(16);
		batt.setSections(new Section(8,10,RED), new Section(14, 16, RED));
		batt.setSectionsVisible(true);
		customize(batt);
		
		final Radial mat = new Radial();
		mat.setTitle("Manifold Air");
		mat.setUnitString("\u2103");
		mat.setMinValue(50);
		mat.setMaxValue(130);
		mat.setSections(new Section(100,120,YELLOW), new Section(120,130,RED));
		mat.setSectionsVisible(true);
		customize(mat);

		final Radial clt = new Radial();
		clt.setTitle("Coolant");
		clt.setUnitString("\u2103");
		clt.setMinValue(50);
		clt.setMaxValue(130);
		clt.setSections(new Section(100,120,YELLOW), new Section(120,130,RED));
		clt.setSectionsVisible(true);
		customize(clt);
		
		final Radial afr = new Radial();
		afr.setTitle("Air/Fuel");
		afr.setUnitString("1:1");
		afr.setMinValue(10);
		afr.setMaxValue(18);
		afr.setSections(new Section(10,13,YELLOW), new Section(15,18,YELLOW));
		afr.setSectionsVisible(true);
		customize(afr);
		
		final Radial dwell = new Radial();
		dwell.setTitle("Dwell");
		dwell.setUnitString("ms");
		dwell.setMaxValue(10);
		dwell.setSections(new Section(6,8,YELLOW), new Section(8,10,RED));
		dwell.setSectionsVisible(true);
		customize(dwell);
		
		final Radial adv = new Radial();
		adv.setDigitalFont(true);
		adv.setTitle("Ignition Advance");
		adv.setUnitString("\u00b0");
		adv.setMaxValue(50);
		customize(adv);
		
		final JPanel guages = new JPanel(new GridLayout(2, 4, 5, 5));
		guages.setBackground(Color.BLACK);
		guages.add(rpm);
		guages.add(tp);
		guages.add(batt);
		guages.add(afr);
		guages.add(map);
		guages.add(mat);
		guages.add(clt);
		guages.add(dwell);
		guages.add(adv);

		final JPanel advance = new JPanel(new GridLayout(8,8,5,5));
		for (int i = 0; i < 64; i++) {
			advance.add(new JTextField());
		}
		
		getContentPane().add(guages);
		setLocationByPlatform(true);
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
		r.setBackgroundColor(BackgroundColor.BLACK);
		r.setFrameDesign(FrameDesign.GLOSSY_METAL);
		r.setPointerType(PointerType.TYPE2);
		r.setKnobType(KnobType.BIG_STD_KNOB);
		r.setKnobStyle(KnobStyle.BLACK);
	}
}
