#include "project_includes.h"
#include "button_functions.h"

// Game settings
int count_time = 1;
int recall_time = 5;
int min_item_amount = 1;
int max_item_amount = 10;

int count_time_mil = count_time * 1000;
int recall_time_mil = recall_time * 1000;

// Phases
bool in_count_phase = true;
bool in_recall_phase = false;

// Count phase variables
int previous_time = 0;
int current_time;

void count_phase() {
    current_time = millis();
    if (current_time - previous_time >= count_time_mil) {
        // End count phase
        previous_time = 0;
        in_count_phase = false;
        in_recall_phase = true;
    }
}

void recall_phase() {
    current_time = millis();
    if (current_time - previous_time >= recall_time_mil) {
        // End recall phase
        previous_time = 0;
        in_recall_phase = false;
    }
}