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

uint8_t mySerialNumber = 0x02;
uint8_t len;
uint8_t myID;
uint8_t idNode;
uint8_t idNetwork;
uint8_t idSerialNumber;
uint8_t idChannel;
uint8_t idField;

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

int sendGiveMeANodeID(int idx, int src, int dst, int sdx, int cmd) {
  char buf[250];
  int len;
  
  if (idx == 0x00) {
    itoa(mySerialNumber,buf,10);
    len = strlen(buf);
    buf[len] = ';';
    len = len + 1;
  } else {
    len = 0x00;
  }
  return sendLORA(idx, src, dst, sdx, cmd, buf, len);
}

int* parseString (char *s) {
  char* myArray[strlen(s)];
  int myArrayint[strlen(s)];
  int i = 0;
  char *p = strtok(s, ";");
  while((p != NULL) & (i < 9)) {
    myArray[i++] = p;
    p = strtok(NULL, ";");
  }

  while (i < 10) myArray[i++] = NULL;

  for (i = 0; i < 10; ++i) {
    if (myArray[i] != NULL) {
      myArrayint[i] = atoi(myArray[i]);
    }
  }
  
  return myArrayint;
}

int send_message(int idx, int src, int dst, int sdx, int cmd, char* mess) {
  int len;
  len = sizeof(mess);
  return sendLORA(idx, src, dst, sdx, cmd, mess, len);
}
void listen_reception() {
  int i=0 ;
  int packetReceiv = 0;
  char message[20];
  int bufLen = 0;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available() && bufLen <= 250) {
      message[i++] = (char) LoRa.read();
    }
    packetReceiv = 1;
    bufLen = i ;
  }
  if(packetReceiv) {
      Serial.println("Response received");
      LoRa.packetRssi();
      myMsg.setSerializedMessage((uint8_t *) message, bufLen);

      if (idNode == NULL) {
        // Récupération du node et network ID
        idNode = atoi(strtok((char*)myMsg.data, ";"));
        idNetwork = atoi(strtok(NULL, ";"));
        idSerialNumber = atoi(strtok(NULL, ";"));

        if (myMsg.idx == 0x00 && myMsg.src == 0x01 && myMsg.dst == 0x00 && myMsg.sdx == 0x01 && myMsg.cmd == 0x01 && idSerialNumber == mySerialNumber) {
          myMsg.printMessage();
          sendGiveMeANodeID(idNetwork, idNode, 0x01, 0x02, 0x00);
        }
      }

      // Vérification initialisation du canal de données
      if (myMsg.idx == idNetwork && myMsg.src == 0x01 && myMsg.sdx == 0x02 && myMsg.cmd == 0x01 && myMsg.dst == idNode) {
        myMsg.printMessage();
        idChannel = atoi(strtok((char*)myMsg.data, ";"));
        Serial.println(idChannel);
        idField = atoi(strtok(NULL, ";"));
        send_message(idNetwork, idNode, 0x01, 0x03, 0x01, "23;");
      }
      
  }
}

void setup() {
  Serial.begin(9600);
  LoRa.setPins(10,7,2);
  if(!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  uint8_t radioBufLen = 0;
  sendGiveMeANodeID(0x00, myID, 0x01, 0x01, 0x00);
  Serial.println("ID sent");
}

void loop() {
  listen_reception();
}