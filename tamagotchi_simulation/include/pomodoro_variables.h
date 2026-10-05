// Pomodoro clock interval variables
bool setting_clock_intervals = false;
bool timer_active = false;

int count_timers_set = 0;			// 0 if no timers set | 1 if study timer set | 2 if both timers set

int input_pomodoro_times[] = {
    0,      // Filler element
    20,     // Default study time (0)
    5       // Default break time (1)
};
int timer_interval_increment = 5;	// Increment timer inverval by 5 minutes
long min_mil_multiplier = 60000;	// Minute to millisecond multiplier

// Pomodoro clock variables from main.cpp
extern long target_study_time, target_break_time;