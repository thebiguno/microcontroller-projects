Vref = 1.25
R1 = 240
Vout = 9

R2 = R1 * ( (Vout / Vref) - 1 )

# Closest resistor value from above
R2 = 1500
Vout = Vref * ( 1 + (R2/R1) )

# Calculate +5% tolerance
disp("+5% R2");
Vout = Vref * ( 1 + (1.05 * (R2/R1)) )

# Calculate -5% tolerance
disp("-5% R2");
Vout = Vref * ( 1 + 0.95 * (R2/R1) )



