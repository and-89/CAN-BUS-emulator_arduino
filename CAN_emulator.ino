//CAN EMULATOR

#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg1; 
//struct can_frame canMsg2; 

MCP2515 mcp2515(10);


void setup() {

  canMsg1.can_id  = 0x18FF3892 | CAN_EFF_FLAG; // for extend: | CAN_EFF_FLAG; 
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0xC0;
  canMsg1.data[1] = 0xFF;
  canMsg1.data[2] = 0xFF;
  canMsg1.data[3] = 0xFF;
  canMsg1.data[4] = 0xFF;
  canMsg1.data[5] = 0xFF;
  canMsg1.data[6] = 0xFF;
  canMsg1.data[7] = 0xFF;

 /* canMsg2.can_id  = 0xCFF113D | CAN_EFF_FLAG;
  canMsg2.can_dlc = 8;
  canMsg2.data[0] = 0x01;
  canMsg2.data[1] = 0xFF;
  canMsg2.data[2] = 0xFF;
  canMsg2.data[3] = 0x0D;
  canMsg2.data[4] = 0xFF;
  canMsg2.data[5] = 0xFF;
  canMsg2.data[6] = 0xFF;
  canMsg2.data[7] = 0xFF;
*/
  
  while (!Serial);
  Serial.begin(115200);
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);   //  baudrates 5,10,20,40,50,80,100,125,200,250,500,1000 clock speed MCP_20MHZ,MCP_16MHZ,MCP_8MHZ
  mcp2515.setNormalMode(); //options: mcp2515.setNormalMode() or mcp2515.setLoopbackMode() or mcp2515.setListenOnlyMode()
  
  Serial.println("Example: Write to CAN");
}

void loop() {
  
  mcp2515.sendMessage(MCP2515::TXB1, &canMsg1);     // mcp2515.sendMessage(&canMsg1);
  delay(100);
  //mcp2515.sendMessage(MCP2515::TXB1, &canMsg2);
  //  delay(100);
  Serial.println("Messages sent");
  


}
