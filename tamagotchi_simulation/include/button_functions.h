#include "project_includes.h"

// In stats screen
bool in_stats_screen = false;

// Sleep/Light values
bool in_sleep_mode = false;
bool sleep_locked = false;

// Game values
bool game_locked = false;

// 
// Animate Function
// 
const unsigned char** current_animation = nullptr;
int anim_size;

long last_frame_time;
int anim_current_frame;
bool anim_running;

int anim_x = 16;
int anim_y = 16;

void animate_item(){
	if (current_animation == nullptr) return;

	if (millis() - last_frame_time >= 250 && anim_running == true) {
		last_frame_time = millis();

		display.fillRect(anim_x,anim_y,16,16,BLACK);
		display.drawBitmap(anim_x, anim_y, current_animation[anim_current_frame], 16, 16, WHITE);

		anim_current_frame++;

		// End animation
		if (anim_current_frame > anim_size){
			current_animation = nullptr;
			anim_current_frame = 0;
			anim_running = false;
			display.fillRect(anim_x,anim_y,16,16,BLACK);
		}
	}
}

// Display timer increment settings
void display_increments() {
	display.fillRect(0,0,128,64,BLACK);
	display_text(F("+5"), 8, 8, 1, true);
	display_text(F("OK"), 8, 28, 1, true);
	display_text(F("-5"), 8, 48, 1, true);
	if (count_timers_set == 1) {
		display_text(F("Set Study Time"), 32, 8, 1, true);
	}
	else if (count_timers_set == 2) {
		display_text(F("Set Break Time"), 32, 8 ,1, true);
	}
	char buffer[12];
	snprintf_P(buffer, sizeof(buffer), PSTR("%02d minutes"), (int)input_pomodoro_times[count_timers_set]);
	display_time(buffer, 44, 28, 1, true);
}

////////////////////////////////////////////
////////// Left Function (Select) //////////
////////////////////////////////////////////

// Selection Step
// 5 = feed, 6 = drink, 7 = light, 8 = game, 9 = clock

void left_function() {
	if (in_sleep_mode == true) return;
	if (in_stats_screen == true) return;

	// Default functionality of left button
	if (!setting_clock_intervals) {
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

				// Return as a "page" for middle button
				selection = i;
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

	// In setting clock intervals; increase time
	else if (setting_clock_intervals) {
		input_pomodoro_times[count_timers_set] += timer_interval_increment;
		display_increments();
		Serial.println("New time " + String(input_pomodoro_times[count_timers_set]));
	}
}

////////////////////////////////////////////////
////////// Middile Function (Confirm) //////////
////////////////////////////////////////////////

unsigned long start_time = 0;
bool studying = false;
bool resetting = false;

class Action{
	public:
		const unsigned char** current_animation;
		const int anim_size;
		
		int anim_current_frame;
		bool anim_running;
		bool animation_toggled;
		long last_frame_time;

		bool anim_pending_reset;

		// int anim_x = 16;
		// int anim_y = 16;

	Action(const unsigned char** animation_bitmap, const int animation_size) : current_animation(animation_bitmap), anim_size(animation_size){
		anim_current_frame = 0;
		anim_running = false;
		anim_pending_reset = false;
	}

	void start_action(){
		anim_running = true;
	}

	void toggle_animation(){
		animation_toggled = !animation_toggled;
	}

	void run_animation(int anim_x, int anim_y, int size_x, int size_y){
		if (millis() - last_frame_time >= 250 && (anim_running == true|| animation_toggled == true)) {
			last_frame_time = millis();

			display.fillRect(anim_x,anim_y,size_x,size_y,BLACK);
			display.drawBitmap(anim_x, anim_y, current_animation[anim_current_frame], size_x, size_y, WHITE);

			anim_current_frame++;

			// End animation
			if (anim_current_frame >= anim_size){
				anim_current_frame = 0;
				anim_running = false;
				if (animation_toggled == false) display.fillRect(anim_x,anim_y,size_x,size_y,BLACK);
				anim_pending_reset = true;
			}
		}
	}
};

// Create action object
Action drink_action(bitmap_water_bottle_anim_array,bitmap_water_bottle_anim_len);
Action eat_action(apple_bitmap_allArray,apple_bitmap_allArray_LEN);
Action sleep_action(sleeping_anim_allArray,sleeping_anim_allArray_LEN);

void eat(){
	eat_action.start_action();
	food.increase(1);
}

void drink(){
	// Animation Information
	// current_animation = bitmap_water_bottle_anim_array;
	// anim_size = bitmap_water_bottle_anim_len;
	// anim_current_frame = 0;
	// anim_running = true;
	drink_action.start_action();

	// Stat increase
	water.increase(2);
}

void light_toggle() {
	if (!sleep_locked) {
		in_sleep_mode = !in_sleep_mode;
		sleep_action.toggle_animation();

		if (in_sleep_mode) {
			display.fillRect(0, 0, 128, 64, BLACK);
		} else {
			// Exiting sleep mode — clear and redraw
			display.fillRect(0, 0, 128, 64, BLACK);
			reset_entire_menu();
			reset_miffy();
			display.display();
		}
	}
}

void game(){
	if (!game_locked) {
		display_text(F("Game   "), 0, 16, 1, true);
		happiness.increase(3);
	}
}

void pomodoro_clock(){
	// Enter clock interval editor
	if (!setting_clock_intervals && !timer_active) {
		Serial.println("Enter clock interval edit");
		// Display the increments on screen
		display_increments();
		setting_clock_intervals = true;

	}
	// Confirm time then activate timer
	if (setting_clock_intervals && !timer_active && count_timers_set < 3) {
		target_study_time = input_pomodoro_times[1] * min_mil_multiplier;
		target_break_time = input_pomodoro_times[2] * min_mil_multiplier;
		Serial.println("Before: " + String(count_timers_set));
		count_timers_set++;
		display_increments();
		Serial.println("After: " + String(count_timers_set));
	}
	// Both timers set, start timer
	if (setting_clock_intervals && !timer_active && count_timers_set >= 3) {
			timer_active = true;
			setting_clock_intervals = false;
			display.fillRect(0, 0, 128, 64, BLACK);
			reset_entire_menu();
			reset_miffy();
	}

	// Once intervals are set, start timer
	if (!setting_clock_intervals && timer_active) {
		// Start studying timer
		if (start_time == 0){
			sleep_locked = game_locked = true;

			studying = true;
			start_time = millis();
		}
		// Prompt cancel confirm
		else if (resetting == false) {
			display_text(F("Confirm Stop Timer?"), 0, 16, 1, true);
			resetting = true;
			return;
		}
		// Cancel Timer
		else if (resetting == true) {
			start_time = 0;
			sleep_locked = game_locked = false;

			// Reset screen
			display.fillRect(0,16, 128, 48, BLACK);
			reset_miffy();

			resetting = false;
			timer_active = false;
			count_timers_set = 0;
		}
	}

	Serial.println("Study:" + String(target_study_time) + " Break:" + String(target_break_time));
	Serial.println(String(setting_clock_intervals) + " " + String(timer_active) + " " + String(count_timers_set));
}

void (*selection_functions[])() = {
  eat,
  drink,
  light_toggle,
  game,
  pomodoro_clock
};

void middle_function(){
	if (selection < 0 || selection > 4) return;

	selection_functions[selection]();
	display.display();
}

////////////////////////////////////
////////// Right Function //////////
////////////////////////////////////
struct stat_data
{
	int* value;
	const unsigned char* bitmap;
};

stat_data stats_value_array[3] = {
	{&food.value, bitmap_allArray[0]},
	{&water.value, bitmap_allArray[1]},
	{&happiness.value, bitmap_allArray[3]}
};

int amount_of_stats = sizeof(stats_value_array) / sizeof(stats_value_array[0]);

void right_function(){
	if (!setting_clock_intervals) {
		if (in_sleep_mode == true) return;
		resetting = false;
		if (selection >= 0) {
			reset_entire_menu();
		}
		else {
			// Enter stats screen
			if (in_stats_screen == false){
				// Clear screen
				display.fillRect(0,0,128,64,BLACK);

				display_text(F("Health"),32,0,2, true);
				for (int i=0; i<amount_of_stats; i++) {
					int bar_position = 16 + (16 * i);

					// Serial.println(*stats_value_array[i].value);
					display.drawBitmap(0,bar_position, stats_value_array[i].bitmap, 16, 16, WHITE);
					display.drawBitmap(0,bar_position, icon_stat_bar_allArray[*stats_value_array[i].value],128,16,WHITE);
				}
				in_stats_screen = !in_stats_screen;
			}
			// Exit stats screen
			else{
				// Go back to miffy
				reset_menu_icons();
				display.fillRect(0,16,128,48,BLACK);
				reset_miffy();
				in_stats_screen = !in_stats_screen;
			}
		}
	}

	// In setting clock intervals; decrease time
	else if (setting_clock_intervals) {
		if (input_pomodoro_times[count_timers_set] >= 5) {
			input_pomodoro_times[count_timers_set] -= timer_interval_increment;
			display_increments();
			Serial.println("New time " + String(input_pomodoro_times[count_timers_set]));
		}
	}
}