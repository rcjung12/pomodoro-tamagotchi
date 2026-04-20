#include "project_includes.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_LENGTH, &Wire, OLED_RESET);

// In stats screen
bool in_stats_screen = false;

int selection = -1;

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

////////////////////////////////////////////
////////// Left Function (Select) //////////
////////////////////////////////////////////

// Selection Step
// 5 = feed, 6 = drink, 7 = light, 8 = game, 9 = clock
int selection_step = 0;
int previous_x = 8;

void left_function() {
	if (in_stats_screen == true) return;
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

////////////////////////////////////////////////
////////// Middile Function (Confirm) //////////
////////////////////////////////////////////////
unsigned long start_time = 0;
bool studying = false;
bool resetting = false;

void display_text(const __FlashStringHelper* input_text, int x, int y, int size){
	display.setCursor(x, y);
	display.setTextSize(size);
	display.setTextColor(WHITE, BLACK);
	display.println(input_text);
}

class Action{
	public:
		const unsigned char** current_animation;
		const int anim_size;
		
		int anim_current_frame;
		bool anim_running;
		long last_frame_time;

		int anim_x = 16;
		int anim_y = 16;

	Action(const unsigned char** animation_bitmap, const int animation_size) : current_animation(animation_bitmap), anim_size(animation_size){
		anim_current_frame = 0;
		anim_running = false;
	}

	void start_action(){
		anim_running = true;
	}

	void run_animation(){
		if (millis() - last_frame_time >= 250 && anim_running == true) {
			last_frame_time = millis();

			display.fillRect(anim_x,anim_y,16,16,BLACK);
			display.drawBitmap(anim_x, anim_y, current_animation[anim_current_frame], 16, 16, WHITE);

			anim_current_frame++;

			// End animation
			if (anim_current_frame > anim_size){
				anim_current_frame = 0;
				anim_running = false;
				display.fillRect(anim_x,anim_y,16,16,BLACK);
			}
		}
	}
};

// Create action object
Action drink_action(bitmap_water_bottle_anim_array,bitmap_water_bottle_anim_len);
Action eat_action(apple_bitmap_allArray,apple_bitmap_allArray_LEN);

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

void light_toggle(){
	display_text(F("Light   "), 0, 16, 1);
}

void game(){
	display_text(F("Game   "), 0, 16, 1);
	happiness.increase(3);
}

void clock(){
	// Start studying timer
	if (start_time == 0){
		studying = true;
		start_time = millis();
	}
	// Prompt cancel confirm
	else if (resetting == false) {
		display_text(F("Confirm Stop Timer?"), 0, 16, 1);
		resetting = true;
		return;
	}
	// Cancel Timer
	else if (resetting == true) {
		start_time = 0;

		// Reset screen
		display.fillRect(0,16, 128, 48, BLACK);
		reset_miffy();

		resetting = false;
	}
}

void (*selection_functions[])() = {
  eat,
  drink,
  light_toggle,
  game,
  clock
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
	resetting = false;
	if (selection >= 0) {
		selection = -1;
		selection_step = 0;
		Serial.println(selection);
		previous_x = 8;

		reset_menu_icons();
	}
	else {
		// Enter stats screen
		if (in_stats_screen == false){
			// Clear screen
			display.fillRect(0,0,128,64,BLACK);

			display_text(F("Health"),32,0,2);
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