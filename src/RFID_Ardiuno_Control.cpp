#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
int greenLedPin = 4;
int redLedPin = 5;
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

byte* data;
String a = "";
String b = "";
String c = "";
String d = "";
String content;
String recv = "";

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
String recvString;
boolean newData = false;
boolean recvState = false;
static boolean recvInProgress = false;

void setup() {
        // put your setup code here, to run once:
        SPI.begin();
        mfrc522.PCD_Init();
        Serial.begin(115200);
        lcd.begin(16, 2);
        pinMode(greenLedPin, OUTPUT);
        pinMode(redLedPin, OUTPUT);
        Serial.write(" <Arduino is ready>");

}

void loop() {
        // put your main code here, to run repeatedly:
        rfidData();
        if (recvState == true) {
                recvWithStartEndMarker();
                dataResponse();
                if (recvString == "ON") {
                        digitalWrite(redLedPin, LOW);
                        digitalWrite(greenLedPin, HIGH);
                        lcd.setCursor(0, 1);
                        lcd.print("ACCESS GRANTED ");
                }
                else if (recvString == "OFF") {
                        digitalWrite(greenLedPin, LOW);
                        digitalWrite(redLedPin, HIGH);
                        lcd.setCursor(0, 1);
                        lcd.print("ACCESS DENIED  ");
                }
                recvState = false;
        }
}

void recvWithStartEndMarker() {
        static byte ndx = 0;
        char startMarker = '<';
        char endMarker = '>';
        char rc;

        while (Serial.available() > 0 && newData == false) {
                rc = Serial.read();

                if (recvInProgress == true) {
                        if (rc != endMarker) {
                                receivedChars[ndx] = rc;
                                ndx++;
                                if (ndx >= numChars) {
                                        ndx = numChars - 1;
                                }
                        }
                        else {
                                receivedChars[ndx] = '\0'; // terminate the string
                                ndx = 0;
                                newData = true;
                        }
                }
                else if (rc == startMarker) {
                        recvInProgress = true;
                }
        }
}

void writeString(String stringData) {
        for (int i = 0; i < stringData.length(); i++) {
                Serial.write(stringData[i]);
        }
}


void dataResponse() {
        if (newData == true) {
                recvString = String(receivedChars);
        }

        newData = false;
        recvInProgress = false;
}


void rfidData() {
        if (!mfrc522.PICC_IsNewCardPresent()) {
                return;
        }
        if (!mfrc522.PICC_ReadCardSerial()) {
                return;
        }
        data = mfrc522.uid.uidByte, mfrc522.uid.size;
        a = String(data[0], HEX);
        b = String(data[1], HEX);
        c = String(data[2], HEX);
        d = String(data[3], HEX);
        String dataAppend = String(a) + String(c) + String(c) + String(d);
        writeString(" "+dataAppend);
        lcd.setCursor(0, 0);
        lcd.print(dataAppend);

        delay(500);
        recvState = true;
}
