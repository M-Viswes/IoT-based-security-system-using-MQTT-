// This file contains the C++ code for publishing MQTT, which is used for data exchange
// between constrained devices and server applications

// Including the necessary header files
#include<PubSubClient.h>
#include<ESP8266WiFi.h>

// WiFi
// Your personal network SSID
const char* ssid = "name";   
// Your personal network password             
const char* wifi_password = "password"; 

// MQTT
// IP address of the MQTT broker
char* mqtt_server = "192.168.215.182";  
char* ID_topic = "home/system/ID";
// MQTT username
char* mqtt_username = "icn";
// MQTT password 
char* mqtt_password = "icn"; 
// MQTT client ID
char* clientID = "Nodemcu"; 

// Creation of wifiClient and client
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); 

// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

// Setup function to start a serial communication so that Arduino can send out commands
// through the USB connection
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
  ; // wait for serial port to connect. Needed for native USB port only
  }

}

String ID = "1";
void loop() {
  // Calling the user-defined function
  connect_MQTT();
    // To read characters from the serial buffer into a string
    String ID = Serial.readString();
    Serial.print("ID number: ");
    Serial.println(ID);
   
   delay(500);
   // Publishing the ID topic and the ID
   if (client.publish(ID_topic, String(ID).c_str())) {
   Serial.println("ID sent!");
   }
   // Orelse it prints an error message and tries to reconnect the client model and publish
   // ID topic and the respective ID
   else {
      Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(ID_topic, String(ID).c_str());
   }
   // To disconnect the client
   client.disconnect();
   delay(100);

}
