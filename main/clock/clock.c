//milliseconds to hexadecimal (F:F:F:F)
int ms = ???;
int hr - ms / 5400000;	// 1/16 day (hex hour)
ms -= hr * 5400000;
int mx = ms / 337500;	// 1/256 day (hex maxime)
ms -= mx * 337500;
int mn = ms / 21094;	// 1/4096 day (hex minute)
ms -= mn * 21094;
int sc = ms / 1318;		// 1/65536 day (hex second)

//milliseconds to mayan (19:19:19:19)
int ms = ???;
int a = ms / 4320000;	// 1/20 day
ms -= a * 4320000;
int b = ms / 216000;	// 1/400 day
ms -= b * 216000;
int c = ms / 10800;		// 1/8000 day
ms -= c * 10800;
int d = ms / 540;		// 1/160000 day

//milliseconds to traditional (24:59:59)
int ms = ???;
int hr = ms / 3600000;	// 1/24 day (hour)
ms -= hr * 3600000;
int mn = ms / 60000;	// 1/1440 day (minute)
ms -= mn / 60000;
int sc = ms / 1000;		// 1/86400 day (second)

//milliseconds to decimal (9:9:9:9)
int ms = ???;
int dd = ms / 8640000;	// 1/10 day (deciday)
ms -= dd * 8640000;
int cd = ms / 864000;	// 1/100 day (centiday)
ms -= cd * 864000;
int md = ms / 86400;	// 1/1000 day (milliday)
ms -= md * 86400;
int ud = ms / 8640;		// 1/10000 day (microday)

//milliseconds to octal (8:8:8:8:8:8)
int ms = ???;
int a = ms / 10800000;	// 1/8 day
ms -= a * 10800000;
int b = ms / 1350000;	// 1/64 day
ms -= b * 1350000;
int c = ms / 168750;	// 1/512 day
ms -= c * 168750;
int d = ms / 21094;		// 1/4096 day
ms -= d * 21094;
int e = ms / 2637;		// 1/32768 day
ms -= e * 21094;
int f = ms / 330;		// 1/262144 day

//milliseconds to d'ni (25:25:25:25)
int ms = ???;
int a = ms / 3456000;	// 1/25 day
ms -= a * 3456000;
int b = ms / 138240;	// 1/625 day
ms -= b * 138240;
int c = ms / 5530;		// 1/15625 day
ms -= c * 5530;
int d = ms / 221;		// 1/390625 day

//milliseconds to swatch (999)
int ms = ???;
int beat = ms / 86400;	// 1/1000 day