#define SCREEN_WIDTH 128
#define SCREEN_LENGTH 64

#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_LENGTH, &Wire, OLED_RESET);

// 
// Reset Miffy
// 
void reset_miffy(){
	display.drawBitmap(
		40,16,
		bitmap_miffy,
		48,48,
		WHITE
	);
}

// 
// Reset Menu Icons
// 
void reset_menu_icons(){
	// Load in menu icons
	Serial.println("Resetting menu icons");
	display.fillRect(0,0,128,16,BLACK);
	for(int step = 0; step < 5; step++){
		int x = 8 + (step * 24);
		// Serial.println(x);

		display.drawBitmap(
			x,
			0,
			bitmap_allArray[step],
			16,16,
			WHITE
		);
	}
}

// 
// Reset Entire Menu
// 
int selection = -1;
int selection_step = 0;
int previous_x = 8;
void reset_entire_menu(){
	// Resets the menu icons and positioning
	selection = -1;
	selection_step = 0;
	Serial.println(selection);
	previous_x = 8;

	reset_menu_icons();
}

// 
// Setup text
// 
void setup_text(int x, int y, int size, bool color) {
    display.setCursor(x, y);
	display.setTextSize(size);
	display.setTextColor(color ? WHITE : BLACK, color ? BLACK: WHITE);
}

// 
// Display text on screen
// 
void display_text(const __FlashStringHelper* input_text, int x, int y, int size, bool color){
	setup_text(x, y, size, color);
	display.println(input_text);
}

// 
// Display number on screen
// 
void display_time(const char* input_text, int x, int y, int size, bool color){
    setup_text(x, y, size, color);
    display.println(input_text);
}