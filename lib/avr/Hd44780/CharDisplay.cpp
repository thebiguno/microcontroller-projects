#include "CharDisplay.h"

using namespace digitalcave;

CharDisplay::CharDisplay(Hd44780* hd44780, uint8_t rows, uint8_t cols) : CharDisplay::CharDisplay(hd44780, rows, cols, NULL){}

CharDisplay::CharDisplay(Hd44780* hd44780, uint8_t rows, uint8_t cols, int8_t* row_offsets){
	this->hd44780 = hd44780;
	this->rows = rows;
	this->cols = cols;
	if (row_offsets == NULL){
		this->row_offsets[0] = 0x00;
		this->row_offsets[1] = 0x40;

		if (rows == 4){
			this->row_offsets[2] = 0x14;
			this->row_offsets[3] = 0x54;
		}
		else {
			this->row_offsets[2] = -1;
			this->row_offsets[3] = -1;
		}
	}
	else {
		this->row_offsets[0] = row_offsets[0];
		this->row_offsets[1] = row_offsets[1];
		this->row_offsets[2] = row_offsets[2];
		this->row_offsets[3] = row_offsets[3];
	}
	
	this->clear();
}

void CharDisplay::write_text(uint8_t row, uint8_t col, const char* text, uint8_t length){
	this->write_text(row, col, (char*) text, length);
}

void CharDisplay::write_text(uint8_t row, uint8_t col, char* text, uint8_t length){
	if (row >= 4 || col >= 20) return;
	uint8_t i = 0;
	for (uint8_t c = col; c < 20 && i < length; c++){
		this->write_text(row, c, text[i++]);
	}
}

void CharDisplay::write_text(uint8_t row, uint8_t col, char text){
	if (row >= 4 || col >= 20) return;
	
	if (this->buffer[(row * 20) + col] != text) {
		this->buffer[(row * 20) + col] = text;
		this->dirty[row] |= (uint32_t) 1 << col;
	}
}

void CharDisplay::set_cursor_position(uint8_t row, uint8_t col){
	if (row >= 4 || col >= 20) return;
	this->cursor_row = row;
	this->cursor_col = col;
	this->hd44780->set_ddram_address(this->row_offsets[row] + col);
}

void CharDisplay::get_cursor_position(uint8_t* row, uint8_t* col){
	*row = this->cursor_row;
	*col = this->cursor_col;
}

void CharDisplay::clear(){
	this->hd44780->clear();
	for(uint8_t i = 0; i < 80; i++){
		this->buffer[i] = ' ';
	}
	for(uint8_t r = 0; r < 4; r++){
		this->dirty[r] = 0;
	}
}

void CharDisplay::refresh(){
	uint8_t changed = 0;
	for(uint8_t r = 0; r < this->rows; r++){
		if (this->dirty[r] != 0x00){
			for (uint8_t c = 0; c < this->cols; c++){
				if (this->dirty[r] & (uint32_t) 1 << c){
					this->hd44780->set_ddram_address(this->row_offsets[r] + c);
					this->hd44780->write_byte(this->buffer[(r * 20) + c]);
					changed = 1;
				}
			}
			this->dirty[r] = 0;
		}
	}
	
	if (changed){
		this->set_cursor_position(this->cursor_row, this->cursor_col);
	}
}