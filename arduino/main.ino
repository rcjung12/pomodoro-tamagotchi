#include "project_includes.h"
#include "button_functions.h"

// 
// Setup
// 
void setup() {
  Serial.begin(9600);
  pinMode(button_left, INPUT_PULLUP);
  pinMode(button_middle, INPUT_PULLUP);
  pinMode(button_right, INPUT_PULLUP);

	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();

	// Load in menu icons
	for(int step = 0; step < 5; step++){
		int x = 8 + (step * 24);
		Serial.println(x);

		display.drawBitmap(
			x,
			0,
			bitmap_allArray[step],
			16,16,
			WHITE
		);
	}

	// Load in miffy sprite
	display.drawBitmap(
		40,16,
		bitmap_miffy,
		48,48,
		WHITE
	);
}

// 
// Check for button press
// 
const int buttons[] = {button_left, button_middle, button_right};
const char* button_name[] = {"Left", "Middle", "Right"};

// Function pointer list for button functions
void (*button_functions_list[])() = {
	left_function,
	middle_function,
	right_function
};

bool button_last_state[3] = {HIGH, HIGH, HIGH};

int selection = -1;

void check_button_press() {
	for(int i = 0; i < 3; i++){

		// Get current button state
		bool button_current_state = digitalRead(buttons[i]);

		if(button_last_state[i] == HIGH && button_current_state == LOW) {
			Serial.print(button_name[i]);
			Serial.println(" Button Pressed");

			button_functions_list[i]();
		}

		button_last_state[i] = button_current_state;
	}
}

// 
// Main Loop
// 
void loop() {
  check_button_press();
	display.display();
	delay(10);
}
