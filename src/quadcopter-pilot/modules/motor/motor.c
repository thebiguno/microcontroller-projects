uint16_t constrain(uint16_t value, uint16_t min, uint16_t max) {
  if (value < min) return min;
  else if (value > max) return max;
  else return value;
}