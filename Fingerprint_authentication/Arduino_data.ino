

// Including the necessary header file
#include <Adafruit_Fingerprint.h>
#define enroll 8
// To allow serial communication on other digital pins
// of an Arduino board
SoftwareSerial mySerial(2, 3); //(Rx, Tx)
SoftwareSerial espSerial(5, 6);
// To interface to the fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

// Setup function for fingerprint sensor enrollment
void setup()  
{
  // To establish a serial communication to allow 
  // devices to talk to each other
  Serial.begin(9600);
  while (!Serial); 
  espSerial.begin(9600);
  delay(100);
  // To configure the port as an input or an input. To configure 
  // the specified pin to behave either as an input (with or 
  // without an internal weak pull-up or pull-down resistor), 
  // or an output.
  pinMode(enroll, INPUT);
  // To print to the serial port the fingerprint sensor enrollment
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");
  // Set the data rate for the sensor serial port
  finger.begin(57600);
  // To verify the fingerprint and display as "Found" if it is verified and 
  // found orelse display as "Not Found"
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
}

int val = 0;
int ID_buffer = -1;
void loop ()
{   
  // To read the value from a specified digital pin as HIGH or LOW
  // If the pin is HIGH, we enroll the fingerprint
  if (digitalRead(enroll) == 1) {
      Serial.println("Enrolling new ID...");
      Enroll();
    }
	// Else, to print to the serial port to place the finger and obtain the data
	//  and to update it in the database
   else {
        Serial.println("Place Your Finger");
        while (ID_buffer == -1) {
          ID_buffer = getFingerprintIDez();
          if (ID_buffer == -1) {
            Serial.println(".");
            }
          }
          espSerial.println(String(ID_buffer));
          Serial.println("Sent ID Data");
    }
    ID_buffer = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function to get the fingerprint enrollment
uint8_t getFingerprintEnroll() {

  int p = -1;
  // To print to the serial port, to wait for the valid fingerprint to be enrolled and id
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  // While loop to iterate if the detected fingerprint is not satisfied, to match the right 
  // case to print to the serial port the right statement
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // If the fingerprint detection is a success and to match the right case and to print to the serial 
  // port the right statement
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // To print to the serial port, to remove the finger
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  // While loop to iterate when no finger is detected and if satisfied 
  // to get the image of the finger
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  // To print to the serial port, the ID and to place the finger again
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  // If the finger detected is not satisfied, we iterate through the while loop
  // to match the right case it falls under
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // If the fingerprint detection is a success and to match the right case and to print to the serial 
  // port the right statement
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // To print to the serial port to create a model 
  Serial.print("Creating model for #");  Serial.println(id);
  // Model creation
  p = finger.createModel();
  // If-else conditions to check the validity of the created model
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // To communicate with and keep the state of the fingerprint sensors stored
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  // If-else conditions to check if the created model is stored or not
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to read and return a number, which has arrived in the serial buffer and
// that is ready to be read
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to get the fingerprint image and check for its validity in the database
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // To return the fingerprint ID when found a match
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to enroll a fingerprint ID to the database and to print the enrolling ID
void Enroll() {
  
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);

  while (! getFingerprintEnroll());
  
}
