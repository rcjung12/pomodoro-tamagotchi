#include <Arduino.h>

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
	reset_miffy();
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

void check_button_press() {
	for(int i = 0; i < 3; i++){

		// Get current button state
		bool button_current_state = digitalRead(buttons[i]);

		if(button_last_state[i] == HIGH && button_current_state == LOW) {

			button_functions_list[i]();
		}

		button_last_state[i] = button_current_state;
	}
}

// 
// Pomordoro Timer
// 

String studying_string;
unsigned long elapsed_time;

long seconds;
long minutes;

long target_study_time = 1200000; // 1200000 = 20 Minutes
long target_break_time = 300000; // 300000 = 5 Minutes

long target_time;
long time_remaining;

String string_time;

void pomodoro_timer(){
	// Set target time to the correct timer length
	if (studying == true){
		target_time = target_study_time;
		studying_string = "Study!";
	}
	else{
		target_time = target_break_time;
		studying_string = "Break!";
	}

	// Check if user started the timer
	if (start_time != 0){

		// Timer variables
		elapsed_time = millis() - start_time;

		time_remaining = target_time - elapsed_time;

		minutes = (time_remaining / 1000 ) / 60;
		seconds = (time_remaining / 1000) % 60;

		// Display Timer
		if (time_remaining > 0) {
			// Display if studying/on break
			display_text(studying_string, 0, 47, 1);
			
			char buffer[6]; // Space for "00:00" + null terminator
			sprintf(buffer, "%02d:%02d", (int)minutes, (int)seconds);
			String string_time = String(buffer);

			display_text(string_time, 0, 56, 1);
		}
		// Reset and flip studying/break
		else{
			studying = !studying;
			// Once time_remaining hits 0, reset start time
			start_time = millis();
		}
	}
}

// 
// Main Loop
// 
void loop() {
  check_button_press();
	display.display();
	pomodoro_timer();
	animate_item();

	update_all_stats();
	delay(10);
}
