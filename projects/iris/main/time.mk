time_a_c_sources = \
	time32/daylight_seconds.c \
	time32/dst_pointer.c \
	time32/equation_of_time.c \
	time32/geo_location.c \
	time32/gm_sidereal.c \
	time32/gmtime.c \
	time32/gmtime_r.c \
	time32/isLeap.c \
	time32/lm_sidereal.c \
	time32/localtime.c \
	time32/localtime_r.c \
	time32/mk_gmtime.c \
	time32/mktime.c \
	time32/month_length.c \
	time32/moon_phase.c \
	time32/print_lz.c \
	time32/set_dst.c \
	time32/set_position.c \
	time32/set_system_time.c \
	time32/set_zone.c \
	time32/solar_declination.c \
	time32/solar_noon.c \
	time32/sun_rise.c \
	time32/sun_set.c \
	time32/system_time.c \
	time32/time.c \
	time32/tm_store.c \
	time32/utc_offset.c \
	time32/week_of_month.c \
	time32/week_of_year.c

time_a_asm_sources = \
	time32/system_tick.S

time_a_extra_dist = \
	time32/ephemera_common.h
