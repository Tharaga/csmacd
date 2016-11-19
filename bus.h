#include <queue>
#include <map>
#include <iostream>

struct Packet {
  std::map<int, float> timeToFirstBit;
  std::map<int, float> timeToLastBit;
  int senderID;
  int generationTime;
  int transmittedTime;
  bool isReceived;
  bool collided;
};

class Bus
{
public:
  Bus();
  ~Bus();
  void decrementWaits(int curTick);
  void setTickDuration(float inputTickDuration);

  std::map<int, Packet> bus;
  typedef std::map<int, Packet>::iterator iter;
  int successfulPacketCount;
  int numberOfCollisions;
  bool isCollision;
  float tick_duration;
  int totalDelay;

private:

};

