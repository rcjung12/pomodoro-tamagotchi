#include "project_includes.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_LENGTH, &Wire, OLED_RESET);

////////////////////////////////////////////
////////// Left Function (Select) //////////
////////////////////////////////////////////

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

	// Return as a "page" for middle button
	return selection_step;
}

//////////////////////////////////////
////////// Middile Function //////////
//////////////////////////////////////
void middle_function(){
	Serial.println("Middle");
}

////////////////////////////////////
////////// Right Function //////////
////////////////////////////////////
void right_function(){
	Serial.println("Right");
}