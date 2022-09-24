import paho.mqtt.client as mqtt

MQTT_ADDRESS = '192.168.215.182'
MQTT_USER = 'icn'
MQTT_PASSWORD = 'icn'
MQTT_TOPIC = 'home/+/+'
import mysql.connector
import Adafruit_CharLCD as LCD

db = mysql.connector.connect(
  host="localhost",
  user="admin",
  passwd="user",
  database="attendance"
)

cursor = db.cursor(buffered=True)
lcd = LCD.Adafruit_CharLCD(4, 24, 23, 17, 18, 22, 16, 2, 4);

def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    """The callback for when a PUBLISH message is received from the server."""
    print(msg.topic + ' ' + str(msg.payload))
    id = str(msg.payload)
    id = id[2:id.find("\\")]
    if len(id) > 0:
        print(id,str(id))
    #cursor.execute("SELECT user_id,in_Server FROM attendanceall WHERE user_id= "+str(id))
    #result = cursor.fetchone()
    #if cursor.rowcount >=1:
        print("UPDATE attendanceall SET in_Server = \"YES\" WHERE user_id = 1")
        cursor.execute(f"UPDATE attendanceall SET in_Server = \"YES\" WHERE user_id =\"{str(id)}\";")
        db.commit()
        #sql = "UPDATE attendanceall SET in_Server = %s WHERE user_id = %s"
        #cursor.execute(sql,('"YES"',id))
        lcd.message("Proceed ! ")
        print("Done")

def main():
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()


if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()
