f = 60

Iload = 0.3

C = linspace(0.000470, 0.005000, 100);

Vripple = Iload ./ ( 2 * f * C);

plot(C, Vripple);


C = 0.00080

Vripple = Iload / ( 2 * f * C)


Vripple = 1.5

C = Iload / ( 2 * f * Vripple )


Vripple * 0.001


