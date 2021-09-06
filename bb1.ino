
//BB2
#include <SPI.h>
#include <mcp2515.h>

#define NR_OF_MESSAGES 7

class CanMessageSender
{
  public:
  CanMessageSender()
  {
    lastTimeMessageSend = 0;
  }
 
  void set(can_frame& f, unsigned long interval)
  {
     frame = f;
     intervalMs = interval;
     lastTimeMessageSend = 0;
  }

  bool shouldBeSend(unsigned long actualTime)
  {
    if (actualTime > (lastTimeMessageSend + intervalMs))
    {
      lastTimeMessageSend = actualTime;
      return true;
    }
    return false;
  }
 
  can_frame frame;
  unsigned long intervalMs;
  unsigned long lastTimeMessageSend;
};


struct can_frame canMsg1;
struct can_frame canMsg2;
struct can_frame canMsg3;
struct can_frame canMsg4;
struct can_frame canMsg5;
struct can_frame canMsg6;
struct can_frame canMsg7;

CanMessageSender messages[NR_OF_MESSAGES];

MCP2515 mcp2515(10);


void setup() {
canMsg1.can_id =0x18FD7B3D | CAN_EFF_FLAG; //cycle 1000
canMsg1.can_dlc = 8;
canMsg1.data[0] = 0x1A;
canMsg1.data[1] = 0x1F;
canMsg1.data[2] = 0xFF;
canMsg1.data[3] = 0xFF;
canMsg1.data[4] = 0xFF;
canMsg1.data[5] = 0xFF;
canMsg1.data[6] = 0xFF;
canMsg1.data[7] = 0xFF;

canMsg2.can_id =0x18FD7C3D | CAN_EFF_FLAG;  // cycle 1000 
canMsg2.can_dlc = 8;
canMsg2.data[0] = 0x00;
canMsg2.data[1] = 0x00;
canMsg2.data[2] = 0x00;
canMsg2.data[3] = 0x00;
canMsg2.data[4] = 0x00;
canMsg2.data[5] = 0x00;
canMsg2.data[6] = 0x00;
canMsg2.data[7] = 0x00;

canMsg3.can_id =0x18FDB33D | CAN_EFF_FLAG;  //cycle 500
canMsg3.can_dlc = 8;
canMsg3.data[0] = 0xFF;
canMsg3.data[1] = 0xFF;
canMsg3.data[2] = 0x9E;
canMsg3.data[3] = 0x2E;
canMsg3.data[4] = 0xFF;
canMsg3.data[5] = 0xFF;
canMsg3.data[6] = 0xFF;
canMsg3.data[7] = 0xFF;

canMsg4.can_id =0x18FFAA3D | CAN_EFF_FLAG;
canMsg4.can_dlc = 8;
canMsg4.data[0] = 0x2F;
canMsg4.data[1] = 0x0F;
canMsg4.data[2] = 0x03;
canMsg4.data[3] = 0x00;
canMsg4.data[4] = 0x50;
canMsg4.data[5] = 0x80;
canMsg4.data[6] = 0xFC;
canMsg4.data[7] = 0xFF;

canMsg5.can_id =0x18ECFF3D | CAN_EFF_FLAG;
canMsg5.can_dlc = 8;
canMsg5.data[0] = 0x20;
canMsg5.data[1] = 0x0E;
canMsg5.data[2] = 0x00;
canMsg5.data[3] = 0x02;
canMsg5.data[4] = 0xFF;
canMsg5.data[5] = 0xCA;
canMsg5.data[6] = 0xFE;
canMsg5.data[7] = 0x00;

canMsg6.can_id =0x18EBFF3D | CAN_EFF_FLAG;
canMsg6.can_dlc = 8;
canMsg6.data[0] = 0x01;
canMsg6.data[1] = 0x00;
canMsg6.data[2] = 0xFF;
canMsg6.data[3] = 0x87;
canMsg6.data[4] = 0x0E;
canMsg6.data[5] = 0x00;
canMsg6.data[6] = 0x09; 
canMsg6.data[7] = 0x87;

canMsg7.can_id =0x18EBFF3D | CAN_EFF_FLAG;
canMsg7.can_dlc = 8;
canMsg7.data[0] = 0x02;
canMsg7.data[1] = 0x0E;
canMsg7.data[2] = 0x10;
canMsg7.data[3] = 0x01;
canMsg7.data[4] = 0x87;
canMsg7.data[5] = 0x0E;
canMsg7.data[6] = 0x0F; 
canMsg7.data[7] = 0x01;

messages[0].set(canMsg1, 1000);
messages[1].set(canMsg2, 1000);
messages[2].set(canMsg3, 499);
messages[3].set(canMsg4, 1000);
messages[5].set(canMsg2, 1000);
messages[6].set(canMsg3, 1000);

  while (!Serial);
  Serial.begin(115200);
  SPI.begin();
 
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);   //  baudrates 5,10,20,40,50,80,100,125,200,250,500,1000 clock speed MCP_20MHZ,MCP_16MHZ,MCP_8MHZ
  mcp2515.setNormalMode(); //options: mcp2515.setNormalMode() or mcp2515.setLoopbackMode() or mcp2515.setListenOnlyMode()
 
  Serial.println("Write to CAN");
}

void loop() {


  unsigned long actualTime = millis();     //take a milliseconds from start

  for (int i=0; i < NR_OF_MESSAGES; i++)
  {
    if (messages[i].shouldBeSend(actualTime))
    {
      mcp2515.sendMessage(MCP2515::TXB1, &messages[i].frame);
      Serial.println("Message sent");
      delay (1);
    }
  }
}
 
 
 
