#include <SPI.h>
#include <mcp2515.h>

#define NR_OF_MESSAGES 6

class CanMessageSender
{
  public:
  CanMessageSender()
  {
    lastTimeMessageSend = 0;
    intervalMs = 0;
    updater = nullptr;
    enabled = true;
  }

  void set(can_frame& f, unsigned long interval, void (*updateFn)(can_frame&, unsigned long) = nullptr)
  {
    frame = f;
    intervalMs = interval;
    updater = updateFn;
    lastTimeMessageSend = 0;
    enabled = true;
  }

  bool shouldBeSend(unsigned long actualTime)
  {
    if (!enabled)
    {
      return false;
    }

    if (actualTime - lastTimeMessageSend >= intervalMs)
    {
      lastTimeMessageSend = actualTime;
      return true;
    }
    return false;
  }

  void applyUpdater(unsigned long actualTime)
  {
    if (updater != nullptr)
    {
      updater(frame, actualTime);
    }
  }

  void updateInterval(unsigned long interval)
  {
    intervalMs = interval;
  }

  void setEnabled(bool state)
  {
    enabled = state;
  }

  bool isEnabled()
  {
    return enabled;
  }

  can_frame frame;
  unsigned long intervalMs;
  unsigned long lastTimeMessageSend;
  void (*updater)(can_frame&, unsigned long);
  bool enabled;
};

struct can_frame canMsg1;
struct can_frame canMsg2;
struct can_frame canMsg3;
struct can_frame canMsg4;
struct can_frame canMsg5;
struct can_frame canMsg6;

CanMessageSender messages[NR_OF_MESSAGES];

MCP2515 mcp2515(10);

void updateRollingCounter(can_frame& frame, unsigned long actualTime);
void updateHeartbeat(can_frame& frame, unsigned long actualTime);
void updateTemperature(can_frame& frame, unsigned long actualTime);
void processSerialCommands();
void printStatus();

void setup() {
  canMsg1.can_id = 0x1CFF903D | CAN_EFF_FLAG;
  canMsg1.can_dlc = 4;
  canMsg1.data[0] = 0x00;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x00;
  canMsg1.data[3] = 0x00;

  canMsg2.can_id = 0x10FF2E3D | CAN_EFF_FLAG;
  canMsg2.can_dlc = 8;
  canMsg2.data[0] = 0x00;
  canMsg2.data[1] = 0x00;
  canMsg2.data[2] = 0xFF;
  canMsg2.data[3] = 0xFF;
  canMsg2.data[4] = 0xFF;
  canMsg2.data[5] = 0xFF;
  canMsg2.data[6] = 0xFF;
  canMsg2.data[7] = 0xFF;

  canMsg3.can_id = 0x19FF013D | CAN_EFF_FLAG;
  canMsg3.can_dlc = 8;
  canMsg3.data[0] = 0x00;
  canMsg3.data[1] = 0x00;
  canMsg3.data[2] = 0x00;
  canMsg3.data[3] = 0x00;
  canMsg3.data[4] = 0x00;
  canMsg3.data[5] = 0x00;
  canMsg3.data[6] = 0xFF;
  canMsg3.data[7] = 0x01;

  canMsg4.can_id = 0x18F77E3D | CAN_EFF_FLAG;
  canMsg4.can_dlc = 8;
  canMsg4.data[0] = 0x00;
  canMsg4.data[1] = 0xFE;
  canMsg4.data[2] = 0xFF;
  canMsg4.data[3] = 0xFF;
  canMsg4.data[4] = 0xFF;
  canMsg4.data[5] = 0xFF;
  canMsg4.data[6] = 0xFF;
  canMsg4.data[7] = 0xFF;

  canMsg5.can_id = 0x18FFAA3D | CAN_EFF_FLAG;
  canMsg5.can_dlc = 8;
  canMsg5.data[0] = 0x2F;
  canMsg5.data[1] = 0x0F;
  canMsg5.data[2] = 0x03;
  canMsg5.data[3] = 0x00;
  canMsg5.data[4] = 0x50;
  canMsg5.data[5] = 0x80;
  canMsg5.data[6] = 0xFC;
  canMsg5.data[7] = 0xFF;

  canMsg6.can_id = 0x18EBFF3D | CAN_EFF_FLAG;
  canMsg6.can_dlc = 8;
  canMsg6.data[0] = 0x01;
  canMsg6.data[1] = 0x00;
  canMsg6.data[2] = 0xFF;
  canMsg6.data[3] = 0x87;
  canMsg6.data[4] = 0x0E;
  canMsg6.data[5] = 0x00;
  canMsg6.data[6] = 0x09;
  canMsg6.data[7] = 0x87;

  messages[0].set(canMsg1, 99, updateRollingCounter);
  messages[1].set(canMsg2, 99);
  messages[2].set(canMsg3, 998, updateTemperature);
  messages[3].set(canMsg4, 1498);
  messages[4].set(canMsg5, 750);
  messages[5].set(canMsg6, 500, updateHeartbeat);

  while (!Serial);
  Serial.begin(115200);
  SPI.begin();

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);   //  baudrates 5,10,20,40,50,80,100,125,200,250,500,1000 clock speed MCP_20MHZ,MCP_16MHZ,MCP_8MHZ
  mcp2515.setNormalMode(); //options: mcp2515.setNormalMode() or mcp2515.setLoopbackMode() or mcp2515.setListenOnlyMode()

  Serial.println("Write to CAN");
  Serial.println("Commands: status | interval <idx> <ms> | enable <idx> <0/1>");
}

void loop() {
  processSerialCommands();

  unsigned long actualTime = millis();     //take a milliseconds from start

  for (int i = 0; i < NR_OF_MESSAGES; i++)
  {
    if (messages[i].shouldBeSend(actualTime))
    {
      messages[i].applyUpdater(actualTime);
      mcp2515.sendMessage(MCP2515::TXB1, &messages[i].frame);
      Serial.print("Message sent from slot ");
      Serial.print(i);
      Serial.print(" with ID 0x");
      Serial.println(messages[i].frame.can_id, HEX);
      delay (1);
    }
  }
}

void updateRollingCounter(can_frame& frame, unsigned long actualTime)
{
  (void) actualTime;
  frame.data[0] = (frame.data[0] + 1) & 0xFF;
  frame.data[1] = (frame.data[0] + 0x10) & 0xFF;
}

void updateHeartbeat(can_frame& frame, unsigned long actualTime)
{
  uint8_t seconds = (actualTime / 1000) & 0xFF;
  frame.data[0] = seconds;
  frame.data[7] = ~seconds;
}

void updateTemperature(can_frame& frame, unsigned long actualTime)
{
  // Fake temperature in 0.1 degC, oscillating between 200 and 350
  uint16_t base = 200;
  uint16_t swing = (actualTime / 100) % 150; // 0..149
  uint16_t temp = base + swing;
  frame.data[0] = temp & 0xFF;
  frame.data[1] = (temp >> 8) & 0xFF;
}

void processSerialCommands()
{
  while (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.length() == 0)
    {
      return;
    }

    if (command.equalsIgnoreCase("status"))
    {
      printStatus();
      return;
    }

    int firstSpace = command.indexOf(' ');
    if (firstSpace < 0)
    {
      Serial.println("Unknown command. Use: status | interval <idx> <ms> | enable <idx> <0/1>");
      return;
    }

    String action = command.substring(0, firstSpace);
    String parameters = command.substring(firstSpace + 1);
    parameters.trim();

    if (action.equalsIgnoreCase("interval"))
    {
      int secondSpace = parameters.indexOf(' ');
      if (secondSpace < 0)
      {
        Serial.println("Usage: interval <idx> <ms>");
        return;
      }

      int index = parameters.substring(0, secondSpace).toInt();
      unsigned long interval = parameters.substring(secondSpace + 1).toInt();
      if (index < 0 || index >= NR_OF_MESSAGES)
      {
        Serial.println("Invalid message index");
        return;
      }
      messages[index].updateInterval(interval);
      Serial.print("Interval for message ");
      Serial.print(index);
      Serial.print(" updated to ");
      Serial.print(interval);
      Serial.println(" ms");
      return;
    }

    if (action.equalsIgnoreCase("enable"))
    {
      int secondSpace = parameters.indexOf(' ');
      if (secondSpace < 0)
      {
        Serial.println("Usage: enable <idx> <0/1>");
        return;
      }

      int index = parameters.substring(0, secondSpace).toInt();
      bool enableState = parameters.substring(secondSpace + 1).toInt() != 0;
      if (index < 0 || index >= NR_OF_MESSAGES)
      {
        Serial.println("Invalid message index");
        return;
      }

      messages[index].setEnabled(enableState);
      Serial.print("Message ");
      Serial.print(index);
      Serial.print(" ");
      Serial.println(enableState ? "enabled" : "disabled");
      return;
    }

    Serial.println("Unknown command. Use: status | interval <idx> <ms> | enable <idx> <0/1>");
  }
}

void printStatus()
{
  Serial.println("CAN Emulator Status:");
  for (int i = 0; i < NR_OF_MESSAGES; i++)
  {
    Serial.print("[");
    Serial.print(i);
    Serial.print("] ID 0x");
    Serial.print(messages[i].frame.can_id, HEX);
    Serial.print(" interval ");
    Serial.print(messages[i].intervalMs);
    Serial.print(" ms - ");
    Serial.println(messages[i].isEnabled() ? "enabled" : "disabled");
  }
}
