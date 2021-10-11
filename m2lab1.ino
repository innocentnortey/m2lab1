int pinBuzzer = 6;
int pinYellow = 4;
int pinPush = 5;
int pinData = 10;
int pinLatch = 11;
int pinClock = 12;
int pinDig[] = {8,9};
boolean toggle1 = 0;
bool initial  =  true;
bool redMode = false;
bool greenMode = false;
bool yellowMode = false;
int displayTime = 0;
int pinRed = 2;
int pinGreen = 3; 
byte numDisp[] = {B00111111, //0
                      B00000110, //1 
                      B01011011, //2
                      B01001111, //3 
                      B01100110, //4
                      B01101101, //5
                      B01111100, //6
                      B00000111, //7
                      B01111111, //8
                      B01100111 //9
                     };
void setup() {
  // put your setup code here, to run once:
  pinMode(pinPush, INPUT_PULLUP);
  pinMode(pinYellow, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinLatch, OUTPUT);
  pinMode(pinData, OUTPUT);
  pinMode(pinClock, OUTPUT);
  pinMode(pinDig[0],OUTPUT);
  pinMode(pinDig[1],OUTPUT);
  digitalWrite(pinDig[0], LOW);
  digitalWrite(pinDig[1], LOW);

  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 63000;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts

}

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (Yellow)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)

  if (initial){
    if (toggle1){
      digitalWrite(pinRed,HIGH);
      displayTime = 1;
      toggle1 = 0;
    }
    else{
      displayTime = 0;
      digitalWrite(pinRed,LOW);
      toggle1 = 1;
    }
  }else{
    displayTime -= 1;
  }
  
}

void buzzMe(){
    digitalWrite(pinBuzzer,HIGH);
    delay(1);
    digitalWrite(pinBuzzer,LOW);
}

void display(int disp){
  int firstDig = disp % 10;
  int secondDig = (disp/10)%10;
  for(int i = 0; i<2 ; i++){
    for(int j = 0; j<2 ; j++){
      digitalWrite(pinDig[j], LOW);
    }
    digitalWrite(pinLatch, LOW);
    if(i == 0){
      shiftOut(pinData, pinClock, MSBFIRST, numDisp[firstDig]);
    }else{
      shiftOut(pinData, pinClock, MSBFIRST, numDisp[secondDig]);
    }
    digitalWrite(pinLatch, HIGH);
    digitalWrite(pinDig[i], HIGH);
    delay(1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  display(displayTime);
  bool pushButton = !digitalRead(pinPush);
  if (pushButton && (initial)){
    initial  = false;
    redMode = true;
    displayTime = 20;
  }
  if(redMode){
    digitalWrite(pinGreen, LOW);
    digitalWrite(pinYellow, LOW);
    digitalWrite(pinRed, HIGH);
    if(displayTime <= 0){
      redMode = false;
      greenMode = true;
      displayTime =  20;
    }
    if(displayTime <= 3){
      buzzMe();
    }
  }
  if(greenMode){
    digitalWrite(pinGreen, HIGH);
    digitalWrite(pinYellow, LOW);
    digitalWrite(pinRed, LOW);
    if(displayTime <= 0){
      greenMode = false;
      yellowMode = true;
      displayTime = 3;
    }
    if(displayTime <= 3){
      buzzMe();
    }
  }
  if(yellowMode){
    digitalWrite(pinGreen, LOW);
    digitalWrite(pinYellow, HIGH);
    digitalWrite(pinRed, LOW);
    if(displayTime <= 0){
      redMode = true;
      yellowMode = false;
      displayTime = 20;
    }
    if(displayTime <= 3){
      buzzMe();
    }  
  }
}
