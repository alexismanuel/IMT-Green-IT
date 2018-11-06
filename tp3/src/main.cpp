#include <Arduino.h>
#include <T2WhisperNode.h>
#include <LoRa.h>
#include <SPI.h>              // include libraries

#define T2_WPN_BOARD T2_WPN_VER_RF95

#include <RH_RF69.h>
//RH_RF95 myRadio;

#define RADIO_FREQUENCY 916.0
#define RADIO_TX_POWER 13
#define RADIO_ENCRYPTION_KEY { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F } // Only used by RF69

// Radio
uint8_t radioBuf[(T2_MESSAGE_HEADERS_LEN + T2_MESSAGE_MAX_DATA_LEN)];

// T2 Message
T2Message myMsg;

uint8_t mySerialNumber = 0x09;
uint8_t len;
uint8_t myID;

int sendLORA(int idx, int src, int dst, int sdx, int cmd, const char *data, int len) {
  uint8_t radioBufLen = 0;

  myMsg.idx = idx;
  myMsg.src = src;
  myMsg.dst = dst;
  myMsg.sdx = sdx;
  myMsg.cmd = cmd;
  myMsg.len = len;
  memcpy(myMsg.data, data, len);

  myMsg.getSerializedMessage(radioBuf, &radioBufLen);
  Serial.print("taille apres serialisation ");
  Serial.println(radioBufLen);

  myMsg.printMessage();
  Serial.println("beforeSend");

  LoRa.beginPacket();
  LoRa.write(radioBuf,radioBufLen);
  LoRa.endPacket();

  delay(10);
  Serial.println("AfterSend");
  return 1;
}

int sendGiveMeANodeID() {
  char buf[10];
  int len;

  itoa(mySerialNumber,buf,10);
  len = strlen(buf);
  buf[len] = ';';
  len = len + 1;
  return sendLORA(0x00, myID, 0x01, 0x01, 0x00, buf, len);
}

void setup() {
  Serial.begin(9600);
  LoRa.setPins(10,7,2);
  if(!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  myMsg.cmd = 0x00;
  myMsg.idx = 0x00;
  myMsg.src = 0x00;
  myMsg.dst = 0x01;
  myMsg.sdx = 0x01;
  myMsg.data[0] = 403;
  myMsg.len = 1;

  uint8_t radioBufLen = 0;

  int res;
  res = sendGiveMeANodeID();
  Serial.println("ID sent");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    uint8_t i;
  	i=0;
    uint8_t* message;
	  while (LoRa.available()) {
      message[i++] = (char) LoRa.read();
      Serial.println(message[i]);
      /*myMsg.setSerializedMessage(message, len);
      myMsg.printMessage();*/
	  }
  }
}