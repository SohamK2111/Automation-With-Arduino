#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display
float input_value;
float desired_speed;
float duty_cycle;
float arduino_cycle;
float arduino_cycle_mid;
float diff;
float logput;
int decider = 0;
int initial_speed;
bool potentiometer_loop_bool = false;

float get_arduino_cycle(float desired_speed){
    
    float arduino_cycle;
    float duty_cycle;
    
    if (desired_speed < 25){
      duty_cycle = 0;
      
    }
    else {
      duty_cycle = map(desired_speed,25,100,15,90);
      
    }
    arduino_cycle = map(duty_cycle,0,100,255,0);
    return arduino_cycle;
}

void potentiometer_loop(){ 
  
  //-------------------all of this maps A0 value, set by a potentiometer, to a desired speed----------------
  input_value = analogRead(A0);
  
  //mapping analogRead signal to a smaller range
  if (input_value < 5){
    desired_speed = 0;
  }
  
  else if (input_value > 1000){
    desired_speed = 100;
  }
  
  else {
    desired_speed = map(input_value, 5, 1000, 25, 100);
  }
  //---------------------------------all of this converts desired speed to cycle----------------------------
  
  //get_arduino_cycle returns a value between 0 and 255
  if (desired_speed >= (100-diff)){
    arduino_cycle = get_arduino_cycle(desired_speed-diff);
    arduino_cycle_mid = get_arduino_cycle(desired_speed);
  }
  else{
    arduino_cycle = get_arduino_cycle(desired_speed);
    arduino_cycle_mid = get_arduino_cycle(desired_speed+diff);
  }
  
  //----------------------------------all of this writes the cycle to pin 9 & 10-----------------------------
  //write the required arduino cycle to pin 9
  analogWrite(9, arduino_cycle);

  //Ouput for middle fan 
  analogWrite(10, arduino_cycle_mid);


  //Serial.println(input_value/1023);
  Serial.println(arduino_cycle);

  //-----------------------------------all of this prints a value to the lcd----------------------------------
  if (round(desired_speed)!=100){
    lcd.setCursor(13,0);
    lcd.print(" ");
  }
  
  if (round(desired_speed)==0){
    lcd.setCursor(12,0);
    lcd.print(" ");
  }
  
  lcd.setCursor(11,0);
  lcd.print(String(round(desired_speed)));
}

void ramp_loop(int wanted_speed){
  
  //---------------------------------all of this converts desired speed to cycle----------------------------
  
  //get_arduino_cycle returns a value between 0 and 255
  if (wanted_speed >= (100-diff)){
    arduino_cycle = get_arduino_cycle(wanted_speed-diff);
    arduino_cycle_mid = get_arduino_cycle(wanted_speed);
  }
  else{
    arduino_cycle = get_arduino_cycle(wanted_speed);
    arduino_cycle_mid = get_arduino_cycle(wanted_speed+diff);
  }
  
  //----------------------------------all of this writes the cycle to pin 9 & 10-----------------------------
  //write the required arduino cycle to pin 9
  analogWrite(9, arduino_cycle);

  //Ouput for middle fan 
  analogWrite(10, arduino_cycle_mid);


  //Serial.println(input_value/1023);
  Serial.println(arduino_cycle);

  //-----------------------------------all of this prints a value to the lcd----------------------------------
  if (round(wanted_speed)!=100){
    lcd.setCursor(13,0);
    lcd.print(" ");
  }
  
  if (round(wanted_speed)<10){
    lcd.setCursor(12,0);
    lcd.print(" ");
  }
  
  lcd.setCursor(11,0);
  lcd.print(String(round(wanted_speed)));
}

void setup() {
  TCCR1B = TCCR1B & B11111000 | B00000100; // for PWM frequency of 122.55 Hz pins D9 & D10
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  diff = 5;
}

void loop() {
  
  //time program for first 10 seconds; if potentiometer is turned 25%, i.e. A0 reading increases, start potentiometer loop
  
  int initial_time = millis();

  lcd.setCursor(0,0);   //Set cursor to character 0 on line 0
  lcd.print("For Auto wait ");

  while (millis() - initial_time < 10000) {
    
    int starting_number = analogRead(A0);
    int time_left = 10 - round(((millis() - initial_time)/1000));
    
    if (time_left != 10) {
      lcd.setCursor(15, 0);
      lcd.print(" ");
    }

    lcd.setCursor(14,0);
    lcd.print(String(time_left));

    if(starting_number > decider) {
      decider = starting_number;
    }
    
    if (decider >= 20 ) {
      potentiometer_loop_bool = true;
      break;
    }
    else {
      ;
      // if speed remains at < 25 (ideally 0) for the whole 10secs then let this while loop finish.
      // the potentiometer_loop_bool should remain false, so use that to select the ramp loop.
    }

  }

  lcd.setCursor(0,0);   
  lcd.print("                ");

  //while loop broken because speed was increased, so run potentiometer loop
  if (potentiometer_loop_bool == true) {
      
    //Print Manual mode speed on LCD
    lcd.setCursor(0,0);   
    lcd.print("(M) Speed: 000 %");
      
    while (true){
      potentiometer_loop();
    }
  }
    
  else {
      
    //Print automatic mode speed on LCD 
    lcd.setCursor(0,0);   
    lcd.print("(A) Speed: 000 %");
      
      
    //-------------ramp up speed - NOTE: uncomment and upload based on which experiment you're running-------------------------------

    //forward loop experiment
    /*
    for (int wanted_speed = 25; wanted_speed <= 100; wanted_speed ++) {
      ramp_loop(wanted_speed);

      if (wanted_speed < 30) {
        delay(2600);
      }
      else if (wanted_speed < 95) {
        delay(1900);
      }
      else {
        delay(2500);
      }
    }
      
      //ramp down speed
    for (int wanted_speed = 100; wanted_speed >=0 ; wanted_speed = wanted_speed - 5) {
      ramp_loop(wanted_speed);
      delay(900);
    }
    */

    //reverse loop experiment
    for (int wanted_speed = 25; wanted_speed <= 100; wanted_speed = wanted_speed + 5) {
      ramp_loop(wanted_speed);
      delay(900);
    }
      
      //ramp down speed
    for (int wanted_speed = 100; wanted_speed >=0 ; wanted_speed = wanted_speed = wanted_speed - 1) {
      ramp_loop(wanted_speed);
      
      if (wanted_speed > 95) {
        delay(2600);
      }
      else {
        delay(1900);
      }

    }
      
    //Print Manual mode speed on LCD
    lcd.setCursor(0,0); 
    lcd.print("                ");
    lcd.setCursor(0,0);   
    lcd.print("Loop finished");
      
    //leave speed at 0
    while (true) {
    analogWrite(9, 0);
    analogWrite(10, 0);
    }
      
  }
}