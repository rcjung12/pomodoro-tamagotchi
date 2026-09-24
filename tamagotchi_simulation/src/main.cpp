#include <Arduino.h>

#include "project_includes.h"
#include "button_functions.h"

// 
// Setup
// 
void setup() {
  Serial.begin(9600);

  Wire.begin(5, 6);

  pinMode(button_left, INPUT_PULLUP);
  pinMode(button_middle, INPUT_PULLUP);
  pinMode(button_right, INPUT_PULLUP);

	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();

	// Load in menu icons
	// for(int step = 0; step < 5; step++){
	// 	int x = 8 + (step * 24);
	// 	Serial.println(x);

	// 	display.drawBitmap(
	// 		x,
	// 		0,
	// 		bitmap_allArray[step],
	// 		16,16,
	// 		WHITE
	// 	);
	// }
	reset_menu_icons();

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
// Timer End Alert
// 

bool alert_running = false;
bool alert_flash_state = false;
int alert_flash_count = 0;
const int alert_max_flash_count = 10;
const int flash_interval = 125;
unsigned long last_flash_time;

void start_timer_alert(){
	alert_running = true;
	alert_flash_state = false;
	alert_flash_count = 0;
	last_flash_time = millis();
}

void stop_timer_alert(){
	alert_running = false;
	alert_flash_state = false;
	display.fillRect(0,0,128,64,BLACK);
	reset_entire_menu();
	reset_miffy();
}

// 
// Pomordoro Timer
// 

const __FlashStringHelper* studying_string;
unsigned long elapsed_time;

long seconds, minutes, target_time, time_remaining;

long target_study_time = 1200000; // 1200000 = 20 Minutes
long target_break_time = 300000; // 300000 = 5 Minutes

String string_time;

void pomodoro_timer(){
	// Set target time to the correct timer length
	if (studying == true){
		target_time = target_study_time;
		studying_string = F("Study!");
	}
	else{
		target_time = target_break_time;
		studying_string = F("Break!");
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
			char buffer[6];
			sprintf(buffer, "%02d:%02d", (int)minutes, (int)seconds);

			display.setCursor(0, 56);
			display.setTextSize(1);
			display.setTextColor(WHITE, BLACK);
			if (!alert_running) {
				display.println(buffer);
				display_text(studying_string, 0, 47, 1, true);
			}
		}
		// Reset and flip studying/break
		else{
			start_timer_alert();
			studying = !studying;
			// Once time_remaining hits 0, reset start time
			start_time = millis();
		}
	}
}

void update_timer_alert(){
	if (!alert_running) return;
	
	if (millis() - last_flash_time >= flash_interval){
		last_flash_time = millis();
		alert_flash_state = !alert_flash_state;

		alert_flash_count++;
		if (alert_flash_count >= alert_max_flash_count) {
			stop_timer_alert();
		}
		else {
			display.fillRect(0,0,128,64, alert_flash_state ? BLACK : WHITE);
			display_text(studying_string, 16, 20, 3, alert_flash_state);
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
	update_timer_alert();

	if (drink_action.anim_running == true) {
		drink_action.run_animation(16,16,16,16);
	}
	else if (eat_action.anim_running == true) {
		eat_action.run_animation(16,16,16,16);
	}
	else if (sleep_action.anim_running == true || sleep_action.animation_toggled == true) {
		sleep_action.run_animation(0,16,128,64);
		if (sleep_action.anim_pending_reset) {
			sleep_action.anim_pending_reset = false;
			if (!in_sleep_mode) {
				reset_entire_menu();
				reset_miffy();
			}
			// If in_sleep_mode, do nothing — wait for light_toggle() to exit
		}
	}
	
	// animate_item();
	update_all_stats();
	delay(10);
}
