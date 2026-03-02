#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "miffy_bitmaps.h"
#include "menu_bitmaps.h"

#define button_left 2
#define button_middle 3
#define button_right 4

#define SCREEN_WIDTH 128
#define SCREEN_LENGTH 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_LENGTH, &Wire, OLED_RESET);

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

	for(int step = 0; step < 5; step++){
		int x = 8 + (step * 24);
		Serial.println(x);

		display.drawBitmap(
			x,
			0,
			bitmap_allArray[step],
			16,16,
			WHITE);
	}
}

// 
// Left Function (Select)
// 

// Selection Step
// 5 = feed, 6 = drink, 7 = light, 8 = game, 9 = clock
int selection_step = 0;
int previous_x = 8;

void left_function() {
	int x = 8 + (selection_step * 24);
	// Selection highlight
	for(int i = 0; i < 5; i++) {
		if(i == selection_step) {

			// Highlight current
			display.drawBitmap(
				x,0,
				bitmap_allArray[i + 5],
				16, 16,
				WHITE
			);

			// Unhighlight previous
			int previous_bitmap = i - 1;
			if(i != 0 || previous_x != 8){

				// Set previous bitmap to 4
				if(previous_bitmap == -1) {previous_bitmap = 4;}

				// Clear previous icon
				display.fillRect(
					previous_x, 0,
					16, 16,
					BLACK
				);

				// Draw unselected icon
				display.drawBitmap(
					previous_x, 0,
					bitmap_allArray[previous_bitmap],
					16, 16,
					WHITE
				);
			}
		}
	}

	// Set previous x to current x
	previous_x = x;

	// Increase selection step / Reset selection step
	if(selection_step == 4){
		selection_step = 0;
	}
	else{
		selection_step++;
	}
}

// 
// Check for button press
// 
const int buttons[] = {button_left, button_middle, button_right};
const char* button_name[] = {"Left", "Middle", "Right"};
bool button_last_state[3] = {HIGH, HIGH, HIGH};

void check_button_press() {
	for(int i = 0; i < 3; i++){

		// Get current button state
		bool button_current_state = digitalRead(buttons[i]);

		if(button_last_state[i] == HIGH && button_current_state == LOW) {
			Serial.print(button_name[i]);
			Serial.println(" Button Pressed");

			// Run button functions
			if(buttons[i] == button_left) {
				left_function();
			}
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
