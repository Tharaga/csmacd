#include "lab2.h"
#include <math.h>

static float waitExpBackoff(int i, float bitTime)
{
  float r = (pow(double(2), double(i)) - 1) * ((float)rand() / (float)RAND_MAX);
  return r * 512 * bitTime;
}


Simulator::Simulator() :
lambda(0),
tArrival(0),
currentI(0),
waitCounter(0),
state(IDLE)
{
}

Simulator::Simulator(Bus* bus, int id, int totalNumberOfStations, float transDelay, int arrivalRate, float tickDuration, int persistenceMode, float pval, float bit_time, int num_ticks)
{
  localAccessToBus = bus;
  stationID = id;
  numNodes = totalNumberOfStations;
  transmissionDelay = transDelay;
  propagationSpeed = 2 * pow(10.0, 8.0);
  currentI = 0;
  waitCounter = 0;
  state = IDLE;
  // Calculate first packet arrival time
  tArrival = calc_arrival_time();
  lambda = arrivalRate;
  tick_duration = tickDuration;
  bitTime = bit_time;
  numTicks = num_ticks;
  switch (persistenceMode)
  {
    case NONPERSISTENT:
      isPPersistent = false;
      isNonPersistent = true;
      p = 0;
      break;
    case PPERSISTENT:
      isPPersistent = true;
      isNonPersistent = false;
      isPPersistentAndHasWaited = false;
      p = pval;
      break;
  }
}

Simulator::~Simulator()
{
}

void Simulator::run(int curTick)
{
  generation(curTick);
  if (waitCounter > 0){
    waitCounter--;
    if (waitCounter <= 0)
      state = IDLE;
  } else {
    if (state != TRANSMITTING)
      departure(curTick);
    else
      detecting();
  }
}

float Simulator::getRandomProbability(){
  float val = (float)rand() / (float)RAND_MAX; //generate random number between 0...1}
  return val;
}

int Simulator::calc_arrival_time(){
  // 1 tick is interpreted as 1 ms
  double u = (double)rand() / (double)RAND_MAX; //generate random number between 0...1
  int arrival_time = (((double)(-1 / lambda)*(double)log(1 - u)) / tick_duration);
  return arrival_time;
}

void Simulator::generation(float t){
  if (t >= tArrival) {
    Packet new_packet;
	// to prevent the arrival time from being beyond the total simulation time, we added the mod to make sure
	// that the calc_arrival_time is not greater than the number of ticks remaining in the simulation
    tArrival = t + (int(calc_arrival_time())%(numTicks-int(t)));
    new_packet.generationTime = t;
    packet_queue.push(new_packet);
  }
}

void Simulator::departure(float t) {
  if (packet_queue.size() > 0) {
    Packet departingPacket = packet_queue.front();
    for (int i = 0; i <= numNodes; i++){
      float propDelay = (abs(stationID - i) * 10 )/propagationSpeed;
      (departingPacket.timeToFirstBit).insert(std::make_pair(i, propDelay));
      (departingPacket.timeToLastBit).insert(std::make_pair(i, propDelay + transmissionDelay));
    }
    departingPacket.senderID = stationID;
    departingPacket.isReceived = false;
    departingPacket.collided = false;

    bool isBusFree = true;
    // Else check if current station believes bus to be free (due to propagation delay)
    if (localAccessToBus->bus.size() >= 1){
      for (Bus::iter iterator = localAccessToBus->bus.begin(); iterator != localAccessToBus->bus.end(); iterator++){
        float wait = (iterator->second.timeToFirstBit).at(stationID);
        if (wait <= 0 && !iterator->second.isReceived){
          isBusFree = false;
          //do nothing; (sensing medium) and the medium is busy
          if (isNonPersistent) {
            waitCounter = waitExpBackoff(currentI, bitTime)/tick_duration;
            state = WAITING;
          }
        }
      }
    }
    if(isBusFree){
        if (isPPersistent)
        {
          float randVal = getRandomProbability();
          if (randVal > p)
          {
            waitCounter = 5;
            isPPersistentAndHasWaited = true;
            return;
          }
        }
        packet_queue.pop();
        localAccessToBus->bus.insert(std::make_pair(stationID, departingPacket));
        state = TRANSMITTING;
    } else {
		if (isPPersistent && isPPersistentAndHasWaited)
		{
			waitCounter = waitExpBackoff(currentI, bitTime) / tick_duration;
			isPPersistentAndHasWaited = false;
			state = WAITING;
			return;
		}
    }
  }
}

void Simulator::detecting(){
  if (localAccessToBus->bus.find(stationID) == localAccessToBus->bus.end())
  { 
    // packet not on bus due to race condition (sensed by all nodes and removed from bus)
    currentI = 0;
    state = IDLE;
    return;
  }

  for (Bus::iter iterator = localAccessToBus->bus.begin(); iterator != localAccessToBus->bus.end(); iterator++){
    if (iterator->second.timeToLastBit.at(stationID) <= 0 && iterator->first == stationID){
      // Finished transmitting -- stop detecting
      currentI = 0;
      state = IDLE;
      return;
    }
    else{
      // detect for collisions
      if (localAccessToBus->bus.size() > 1){
        if (iterator->second.timeToFirstBit.at(stationID) <= 0) {
          if (iterator->first != stationID || (iterator->first == stationID && iterator->second.collided)) {
            localAccessToBus->isCollision = true;
            currentI++;
            if (currentI < 10){
              waitCounter = waitExpBackoff(currentI, bitTime)/tick_duration;
              state = WAITING;
              return;
            } else {
              // This is the error state when i saturates. Reset node to neutral state.
              currentI = 0;
              state = IDLE;
              return;
            }
          }
        }
      }
    }
  }
}

