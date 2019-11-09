#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
	uint8_t r0_chip_id : 8;
	uint8_t r0_unused1 : 8;

	uint8_t r2_dhiz : 1;
	uint8_t r2_dmute : 1;
	uint8_t r2_mono : 1;
	uint8_t r2_bass : 1;
	uint8_t r2_rclk : 1;
	uint8_t r2_rclk_di : 1;
	uint8_t r2_seekup : 1;
	uint8_t r2_seek : 1;
	uint8_t r2_seek_mode : 1;
	uint8_t r2_clk_mode : 3;
	uint8_t r2_rds_en : 1;
	uint8_t r2_new_method : 1;
	uint8_t r2_soft_reset : 1;
	uint8_t r2_enable : 1;

	//You need to be careful here; this 10 bit value is left shifted 6 bits.  This is the only place
	// in this struct where this is an issue; everywhere else the msb / lsb fields are right shifted.
	uint8_t r3_channel_msb : 8;
	uint8_t r3_channel_lsb : 2;
	uint8_t r3_direct : 1;
	uint8_t r3_tune : 1;
	uint8_t r3_band : 2;
	uint8_t r3_space : 2;

	uint8_t r4_rsvd1 : 1;
	uint8_t r4_unused1 : 1;
	uint8_t r4_rsvd2 : 2;
	uint8_t r4_de : 1;
	uint8_t r4_rsvd3 : 1;
	uint8_t r4_softmute_en : 1;
	uint8_t r4_afcd : 1;
	uint8_t r4_unused2 : 8;

	uint8_t r5_int_mode : 1;
	uint8_t r5_rsvd1 : 3;
	uint8_t r5_seek_snr : 4;
	uint8_t r5_unused1 : 2;
	uint8_t r5_rsvd2 : 2;
	uint8_t r5_volume : 4;

	uint8_t r6_rsvd1 : 1;
	uint8_t r6_open_mode : 2;
	uint8_t r6_unused1 : 5;
	uint8_t r6_unused2 : 8;

	uint8_t r7_rsvd1 : 1;
	uint8_t r7_th_softblend : 5;
	uint8_t r7_65m_50m_mode : 1;
	uint8_t r7_rsvd2 : 1;
	uint8_t r7_seek_th_old : 6;
	uint8_t r7_softblend_en : 1;
	uint8_t r7_freq_mode : 1;

	uint8_t rA_rdsr : 1;
	uint8_t rA_stc : 1;
	uint8_t rA_sf : 1;
	uint8_t rA_rdss : 1;
	uint8_t rA_blk_e : 1;
	uint8_t rA_st : 1;
	uint8_t rA_read_chan_msb : 2;
	uint8_t rA_read_chan_lsb : 8;

	uint8_t rB_rssi : 7;
	uint8_t rB_fm_true : 1;
	uint8_t rB_fm_ready : 1;
	uint8_t rB_rsvd1 : 2;
	uint8_t rB_abcd_e : 1;
	uint8_t rB_blera : 2;
	uint8_t rB_blerb : 2;

	uint8_t rC_rdsa_msb : 8;
	uint8_t rC_rdsa_lsb : 8;

	uint8_t rD_rdsb_msb : 8;
	uint8_t rD_rdsb_lsb : 8;

	uint8_t rE_rdsc_msb : 8;
	uint8_t rE_rdsc_lsb : 8;

	uint8_t rF_rdsd_msb : 8;
	uint8_t rF_rdsd_lsb : 8;
} rda5807_register_fields_t;

typedef union {
	rda5807_register_fields_t fields;
	uint8_t data[26];
} rda5807_registers_t;

void printStuff(rda5807_registers_t r){
	printf("sizeof(r): %lu\n", sizeof(r));
	printf("r0_chipId: %02X, r2_dhiz: %d, r2_dmute: %d, r2_mono: %d, r2_bass: %d, r3_channel: %d\n", r.fields.r0_chip_id, r.fields.r2_dhiz, r.fields.r2_dmute, r.fields.r2_mono, r.fields.r2_bass, (r.fields.r3_channel_msb << 8) | r.fields.r3_channel_lsb);
	printf("data: [ ");
	for (uint8_t i = 0; i < sizeof(r.data); i++){
		printf("%02X ", r.data[i]);
	}
	printf("]\n\n");
}

int main (void){
	rda5807_registers_t r;
	for (uint8_t i = 8; i < sizeof(r.data); i++){
		r.data[i] = 0x00;
	}

	printStuff(r);

	r.fields.r0_chip_id = 0x58;
	r.fields.r0_unused1 = 0x04;
	r.fields.r2_dhiz = 1;
	r.fields.r2_mono = 1;
	r.fields.r2_rclk = 1;

	r.fields.r3_channel_msb = 0xFF;
	r.fields.r3_channel_lsb = 0x03;

	r.fields.rA_read_chan_msb = 0x03;
	r.fields.rA_read_chan_lsb = 0xC8;
	r.fields.rF_rdsd_msb = 0xBE;
	r.fields.rF_rdsd_lsb = 0xEF;

	printStuff(r);

	r.data[2] = 0x55;

	printStuff(r);

	return 0;
}
