// Custom string copy function
void stringCopy(char* destination, const char* text, size_t length) {
	if (length <= 0) return; // If length is 0, do nothing
    strncpy(destination, text, length - 1); // Copy the string
    destination[length - 1] = '\0'; // Add the terminating character
}

// Format a currency - Inspired by the Currency library made by RobTillaart
char* addThousandsSeparators(double value, int decimals, char decimalSeparator, char thousandSeparator, char symbol = ' ') {
	static char tmp[30]; // Temporary buffer to store the formatted string
	uint8_t index = 0; // Index for placing characters in tmp
	int64_t v = (int64_t)value; // Convert the value to an integer
	bool negative = v < 0; // Flag for negative numbers
	if (negative) v = -v; // Make v positive for processing
	int pos = -decimals; // Tracks the position relative to the decimal point
	while ((pos < 1) || (v > 0)) { // Loop until we've processed all digits
		if ((pos == 0) && (decimals > 0)) tmp[index++] = decimalSeparator; // Add decimal separator
		if ((pos > 0) && (pos % 3 == 0)) tmp[index++] = thousandSeparator; // Add thousand separator
		pos++;
		tmp[index++] = (v % 10) + '0'; // Extract and store the last digit of v
		v /= 10; // Remove the last digit from v
	}
	if (negative) tmp[index++] = '-'; // Add negative sign if necessary
	if (symbol != ' ') {
		tmp[index++] = ' '; // Add space
		tmp[index++] = symbol; // Add the currency symbol if there is
	}
	tmp[index] = '\0'; // Null-terminate the string
	for (uint8_t i = 0, j = index - 1; i < index / 2; i++, j--) { // Reverse the string since it was built backwards
		char c = tmp[i];
		tmp[i] = tmp[j];
		tmp[j] = c;
	}
	return tmp; // Return the pointer to the formatted string
}

// Map a value between into a range
int mapValue(double x, double inMin, double inMax, int outMin, int outMax) {
	return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}