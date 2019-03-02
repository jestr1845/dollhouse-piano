int speakerOut = 12;   
int singleNote = 10;
int tripleNote = 11;           

// these two are for the activityCheckIn function
int inactivityMinutesLimit = 5;
unsigned long int oldMillis = 0;

byte names[7] = {'c', 'd', 'e', 'f', 'g', 'a', 'b'};  
int tones[7] = {262, 294, 330, 349, 392, 440, 493};

// notes: edcdeeedddeggedcdeeeeddedc
byte maryHadALittleLamb[] = {4,3,2,3,4,4,4,'r',3,3,3,'r',4,6,6,'r','r',4,3,2,3,4,4,4,4,3,3,4,3,2};

// notes: ccggaagffeeddcggffeedggffeedccggaagffeeddc
byte twinkleTwinkleLittleStar[] = {2,2,6,6,7,7,6,'r',5,5,4,4,3,3,2,'r',6,6,5,5,4,4,3,'r',6,6,5,5,4,4,3,'r',2,2,6,6,7,7,6,'r',5,5,4,4,3,3,2};

// notes: ggagcbggagdcgggecbaffecdc
byte happyBirthdayToYou[] = {6,6,7,6,8,2,'r','r',6,6,7,6,3,2,6,'r','r',6,6,4,2,8,7,5,'r','r',5,4,2,3,2};

// notes: gcccdeeedcdeceefggfefgeccdeedcdecggcccdeeedcdec
byte itsyBitsySpider[] = {6,2,2,2,3,4,'r',4,4,3,2,3,4,2,'r','r',4,'r',4,5,6,'r',6,'r',5,4,5,6,4,'r',2,'r',2,'r',3,4,4,3,2,3,4,2,'r',6,6,2,2,2,3,4,'r',4,4,3,2,3,4,2};
//byte itsyBitsySpider[] = {6,'r',2,2,2,3,'r',4,4,4,'r',3,2,3,4,'r','r',2,'r','r',4,4,5,'r',6,6,'r',5,4,5,6,'r',4,'r','r',2,2,3,'r',4,4,'r',3,2,3,4,2,'r',6,6,'r',2,2,2,3,'r',4,4,4,'r',3,2,3,4,'r',2};



void simonGame(void);
void playerPiano (byte ar[], int arSize);
int getResponse(void);
void splash(int startKey);
void activityCheckIn(bool flag);

void setup() {
  pinMode(speakerOut, OUTPUT);
  
  // initialize leds
  for(int j = 13; j < 20; j++)
  pinMode(j, OUTPUT);
 
  // initialize piano keys
  for(int i = 2; i < 9; i++)
  pinMode(i, INPUT_PULLUP);

  // initialize music state switch
  pinMode(singleNote, INPUT_PULLUP); 
  pinMode(tripleNote, INPUT_PULLUP); 
    
  // startup sweep
  for(int k=13; k<20; k++){
    digitalWrite(k, HIGH);
    delay(50);
    digitalWrite(k, LOW);
  }
  for(int k=19; k>12; k--){
    digitalWrite(k, HIGH);
    delay(50);
    digitalWrite(k, LOW);
  }

  
  Serial.begin(9600);

} // *************************** end of setup ***************************



void loop() { 
  activityCheckIn(false);
  
  //check for simonGame easter egg
  if(digitalRead(2) == LOW && digitalRead(8) == LOW){
    splash(5);
    simonGame();
  } 

  //check for maryHadALittleLamb easter egg
  if(digitalRead(2) == LOW && digitalRead(3) == LOW){
    splash(3);
    playerPiano (maryHadALittleLamb, sizeof(maryHadALittleLamb)/sizeof(maryHadALittleLamb[0]));
  }

  //check for twinkleTwinkleLittleStar easter egg
  if(digitalRead(2) == LOW && digitalRead(4) == LOW){
    splash(4);
    playerPiano (twinkleTwinkleLittleStar, sizeof(twinkleTwinkleLittleStar)/sizeof(twinkleTwinkleLittleStar[0]));
  }

  //check for happyBirthdayToYou easter egg  TODO, find a better melody
  if(digitalRead(2) == LOW && digitalRead(5) == LOW){
    //splash(5);
    //playerPiano (happyBirthdayToYou, sizeof(happyBirthdayToYou)/sizeof(happyBirthdayToYou[0]));
  }

  //check for itsyBitsySpider easter egg
  if(digitalRead(2) == LOW && digitalRead(6) == LOW){
    splash(6);
    playerPiano (itsyBitsySpider, sizeof(itsyBitsySpider)/sizeof(itsyBitsySpider[0]));
  }
    
  // check for button press
  for(int j=2; j<9; j++)   
    if(digitalRead(j) == LOW){
      activityCheckIn(true);
      digitalWrite(j+11, HIGH);
      tone(speakerOut, tones[j-2],200);
    }
    else 
      digitalWrite(j+11, LOW);
      


} // *************************** end of loop ***************************

/*
This function takes an array of integers that correspond to the piano buttons and will create a player piano version if the triple 
note switch state is active, otherwise it will wait for the user to follow along
*/
void playerPiano (byte ar[], int arSize){
  int i=0, response;
 
  for(i=0; i<arSize; i++){
    if (ar[i] == 'r'){ // then no tone, but "rest"
      if (digitalRead(tripleNote==HIGH)) 
        delay(200);
      noTone(speakerOut);
      continue;
    }
    
    digitalWrite(ar[i] + 11, HIGH); // turn on light
    
    if(digitalRead(tripleNote == HIGH)){ // then player piano
      tone(speakerOut, tones[ar[i]-2],200); 
      delay(400);
    }
    
    else if(digitalRead(singleNote == HIGH)){ // then user plays along
       //wait for user to press AND RELEASE a key
       response = getResponse(); 
       while(1){
         if(digitalRead(response) == 1){
           noTone(speakerOut);
           break;  
         }
         response = getResponse(); 
         tone(speakerOut, tones[response-2]); 
         delay(200);
       }
      delay(200); 
    }
    
    digitalWrite(ar[i]+11, LOW); // turn off light
    
  }
  splash(ar[i]);
  return;
  
} // *************************** end of playerPiano ***************************



/*
This function is a memory game that uses buttons and leds. The pin and led numbers are from the piano program.
*/
void simonGame(void){
  int counter = 0;
  int simonSequence[10];
  int response;
  
  // fill the array with random numbers
  randomSeed(analogRead(21));
  for(int i = 0; i < 10; i++){
    simonSequence[i] = random(13, 20); // picks a number [13, 20)
  }

  //game loop
  while(1){
    // display the sequence
    for(int j = 0; j <= counter; j++){
    digitalWrite(simonSequence[j], HIGH);
    delay(300);
    digitalWrite(simonSequence[j], LOW);
    delay(100);
    }

    // verify user is copying
    for(int k = 0; k <= counter; k++){
      response = 0;
      //continually check for a button press
      while(response == 0){
        for(int K=2; K<9; K++){   
          if(digitalRead(K) == LOW){
            response = K;
            activityCheckIn(true);
            break;
          }
        }
      }
      
      digitalWrite(response + 11, HIGH);
      delay(100);
      digitalWrite(response + 11, LOW);
      delay(100);
      if(response+11 != simonSequence[k]){ // then gamesOver
          splash(response);
          return;
      }
      else
        delay(100);
    }

    // increment the sequence
    counter ++;
   }// end of game loop

}// *************************** end of simonGame *************************** 

/*
This function will create a splash effect to indicate a transition back to loop
*/
void splash(int startKey){
  int startLight = startKey + 11;
  
  digitalWrite(startKey+11, HIGH);
  delay(100);
  digitalWrite(startKey+11, LOW);
  delay(100);
  
  for(int i = 0; i < 7; i++){
    digitalWrite(startLight + i, HIGH);
    digitalWrite(startLight - i, HIGH);
    delay(100);
    digitalWrite(startLight + i, LOW);
    digitalWrite(startLight - i, LOW);
    delay(100);
  }


  // re-initialize piano keys (keys all read zero at all times until i put this in)
  for(int i = 2; i < 9; i++)
  pinMode(i, INPUT_PULLUP);
 
  return;
}


// *************************** end of splash *************************** 

/*
 * This function determines which button is being pressed and returns the pin of the button
*/
int getResponse(void){
  while(1){
    for(int K=2; K<9; K++){
      if(digitalRead(K) == LOW){
        activityCheckIn(true);
        return K;
      }
    }
  }

}
// *************************** end of getResponse *************************** 

/*
If this function is not called within inactivityMinutesLimit, then sleep indefinitely.  A flag variable is used to distinguish
between calling this function to update timer (true) or to check if the time has elapsed (false)
*/
void activityCheckIn(bool flag){
Serial.print("Current Millis: ");
Serial.print(millis());
Serial.print("\tCurrent oldMillis: ");
Serial.println(oldMillis);
if((millis() - oldMillis) > inactivityMinutesLimit*1000.0*60){
  //TODO: learn sleep, maybe from here https://www.youtube.com/watch?v=8fiYvnBDsDE
  Serial.println("BEEP");
  tone(speakerOut, tones[6], 500);
  // beep in a minute again unless button is pressed
  oldMillis = millis() - 1000.0*60*(inactivityMinutesLimit-1);
}
if (flag)
oldMillis = millis();

return;
}// *************************** end of activityCheckIn *************************** 

