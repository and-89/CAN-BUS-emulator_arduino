#include <SPI.h>
#include <mcp2515.h>

#define NR_OF_MESSAGES 4

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

CanMessageSender messages[NR_OF_MESSAGES];

MCP2515 mcp2515(10);


void setup() {
canMsg1.can_id =0x1CFF903D | CAN_EFF_FLAG;
canMsg1.can_dlc = 4;
canMsg1.data[0] = 0x00;
canMsg1.data[1] = 0x00;
canMsg1.data[2] = 0x00;
canMsg1.data[3] = 0x00;

canMsg2.can_id =0x10FF2E3D | CAN_EFF_FLAG;
canMsg2.can_dlc = 8;
canMsg2.data[0] = 0x00;
canMsg2.data[1] = 0x00;
canMsg2.data[2] = 0xFF;
canMsg2.data[3] = 0xFF;
canMsg2.data[4] = 0xFF;
canMsg2.data[5] = 0xFF;
canMsg2.data[6] = 0xFF;
canMsg2.data[7] = 0xFF;

canMsg3.can_id =0x19FF013D | CAN_EFF_FLAG;
canMsg3.can_dlc = 8;
canMsg3.data[0] = 0x00;
canMsg3.data[1] = 0x00;
canMsg3.data[2] = 0x00;
canMsg3.data[3] = 0x00;
canMsg3.data[4] = 0x00;
canMsg3.data[5] = 0x00;
canMsg3.data[6] = 0xFF;
canMsg3.data[7] = 0x01;

canMsg4.can_id =0x18F77E3D | CAN_EFF_FLAG;
canMsg4.can_dlc = 8;
canMsg4.data[0] = 0x00;
canMsg4.data[1] = 0xFE;
canMsg4.data[2] = 0xFF;
canMsg4.data[3] = 0xFF;
canMsg4.data[4] = 0xFF;
canMsg4.data[5] = 0xFF;
canMsg4.data[6] = 0xFF;
canMsg4.data[7] = 0xFF;

messages[0].set(canMsg1, 99);
messages[1].set(canMsg2, 99);
messages[2].set(canMsg3, 998);
messages[3].set(canMsg4, 1498);

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
