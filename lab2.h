#include <cstdlib>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "bus.h"

class Simulator
{
public:
  Simulator(Bus* bus, int id, int numNodes, float transDelay, int arrivalWait, float tickDuration, int persistence, float p, float bit_time, int numTicks);
  Simulator();
  ~Simulator();

  void run(int curTick);
  int calc_arrival_time();
  void generation(float t);
  void departure(float t);
  void detecting();

  std::queue<Packet> packet_queue;

private:
  float getRandomProbability();
  float lambda;
  float propagationSpeed; // default is 2*10^8

  float tArrival;

  int stationID;
  int numNodes;
  Bus* localAccessToBus;
  float transmissionDelay;
  float waitCounter;
  int currentI;
  int state;

  float tick_duration;
  bool isNonPersistent;
  bool isPPersistent;
  bool isPPersistentAndHasWaited;
  float bitTime;
  float p;
  int numTicks;

};

enum States {TRANSMITTING, IDLE, WAITING};
enum Persistence{NONPERSISTENT, PPERSISTENT};