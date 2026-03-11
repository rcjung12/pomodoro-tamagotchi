const long one_hour = 3600000;
const long half_hour = 1800000;
const long twenty_minutes = 1200000;

class Stat{
  public:
    int value;
    const long decay_time;

    long time_elapsed;
    long time_start;

    Stat(int start_value, long decay) : value(start_value), decay_time(decay) {
      time_start = millis();
      time_elapsed = 0;
    }

    void update(){
      time_elapsed = millis() - time_start;
      
      if (time_elapsed >= decay_time && value > 0){
        value--;
        time_start = millis();
      }
    }

    void increase(int amount){
      value += amount;
      if(value > 4){
        value = 4;
      }
    }
}

// Create objects
Stat food(4, one_hour);
Stat water(4, twenty_minutes);
Stat happiness(4, half_hour);

void update_all_stats(){
  food.update();
  water.update();
  happiness.update();
}