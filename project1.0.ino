//                                                         //
//    Program       : Mindwave with Arduino_Robo Car       //
//    Interfacing   : HC-05 Bluetooth Module               //
//    Output        : Robo Control using Attention         //

#define BAUDRATE 57600
#define LED 13
#define ATT_TH 60
#define echoPin 7 // Echo Pin
#define trigPin 8 // Trigger Pin

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance
int Mot_L_Plus  = 3;
int Mot_L_Minus = 5;
int Mot_R_Plus  = 6;
int Mot_R_Minus = 9;

byte payloadData[32] = {0};
byte checksum=0,generatedchecksum=0,Poorquality,Attention;
int  Att_Avg=0,Plength,Temp;
int  k=0,j=0;

void setup() 
{
   Serial.begin(BAUDRATE);           // USB
   pinMode(LED, OUTPUT);
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
   pinMode(Mot_L_Plus, OUTPUT);   
   pinMode(Mot_L_Minus, OUTPUT); 
   pinMode(Mot_R_Plus, OUTPUT);   
   pinMode(Mot_R_Minus, OUTPUT); 
}

byte ReadOneByte()           // One Byte Read Function
{
  int ByteRead;
  while(!Serial.available());
  ByteRead = Serial.read();
  return ByteRead;
}

void loop()                     // Main Function
{
  delayMicroseconds(2); 

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration/58.2;
 
  if (distance >= maximumRange || distance <= minimumRange){
  /* Send a negative number to computer and Turn LED ON 
  to indicate "out of range" */
  Serial.println("-1");
  digitalWrite(LED, HIGH); 
  }
  else {
  /* Send the distance to the computer using Serial protocol, and
  turn LED OFF to indicate successful reading. */
  Serial.println(distance);
  digitalWrite(LED, LOW); 
  }
 
  //Delay 50ms before next reading.
  delay(50);
  while (1)
  {
    if(ReadOneByte() == 170)        // AA 1 st Sync data
    {
      if(ReadOneByte() == 170)      // AA 2 st Sync data
      {
        Plength = ReadOneByte();
        if(Plength == 32)   // Big Packet
        { 
          generatedchecksum = 0;
          for(int i = 0; i < Plength; i++) 
          {  
            payloadData[i]     = ReadOneByte();      //Read payload into memory
            generatedchecksum  += payloadData[i] ;
          }
          generatedchecksum = 255 - generatedchecksum;
          checksum  = ReadOneByte();
        
          if(checksum == generatedchecksum)        // Varify Checksum
          {
            Poorquality  = payloadData[1];
            Attention    = payloadData[29];
            Serial.print(Attention);
            Serial.print("\n");
            Attention_Fun(Attention);
           }
          } 
        }
      }
    }         
 } 

 void Robot_Forword()
 {
   digitalWrite(Mot_L_Plus, LOW);   
   digitalWrite(Mot_L_Minus, HIGH); 
   digitalWrite(Mot_R_Plus, LOW);   
   digitalWrite(Mot_R_Minus, HIGH);  
 }
 void Robot_Backward()
 {
  digitalWrite(Mot_L_Plus,HIGH);
  digitalWrite(Mot_L_Minus,LOW);
  digitalWrite(Mot_R_Plus,HIGH);
  digitalWrite(Mot_R_Minus,LOW);
 }
 
 void Robot_Stop()
 {
   digitalWrite(Mot_L_Plus, LOW);   
   digitalWrite(Mot_L_Minus, LOW); 
   digitalWrite(Mot_R_Plus, LOW);   
   digitalWrite(Mot_R_Minus, LOW); 
 }
 
 void Attention_Fun(byte data1)
 {
    if (Poorquality == 0)
    {            
      if (j<3)
       {
         Temp += data1;
         j++;
       }
       else
       {
         Temp += data1;
         Att_Avg = Temp/4;
         Serial.print("Att_Avg");
         Serial.print(Att_Avg);
         Serial.print("\n");
         if (Att_Avg >=60)
         {
             digitalWrite(LED, HIGH);
             Robot_Forword();
         }
         else if(Att_Avg >=20 && Att_Avg <60)
         {
          digitalWrite(LED, HIGH);
          Robot_Backward();
         }
         else
         {
             digitalWrite(LED, LOW);
             Robot_Stop();
         }
         j=0;
         Temp=0;
        }
     }
     else
     {
          Robot_Stop(); 
          Serial.print("Signal Poor Quality");
          Serial.print("\n");
     } 
 }

