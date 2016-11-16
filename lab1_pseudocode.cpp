#include "lab1.h"
#include <math.h>

static float waitExpBackoff(int i)
{
  //srand(time(NULL));
  float r = (pow(double(2), double(i)) - 1) * ((float)rand() / (float)RAND_MAX);
  return r * 512;
}


Simulator::Simulator() :
numOfTicks(0),
lambda(0),
tArrival(0),
isInfiniteBuffer(true),
currentI(0),
waitCounter(0),
state(IDLE)
{
}

Simulator::Simulator(Bus* bus, int id, int totalNumberOfStations, float transDelay, int arrivalRate, float tickDuration, int persistenceMode, float pval)
{
  localAccessToBus = bus;
  stationID = id;
  numNodes = totalNumberOfStations;
  transmissionDelay = transDelay;
  propogationSpeed = 2 * pow(10.0, 8.0);
  currentI = 0;
  waitCounter = 0;
  state = IDLE;
  // Calculate first packet arrival time
  tArrival = calc_arrival_time();
  lambda = arrivalRate;
  isInfiniteBuffer = true;
  tick_duration = tickDuration;

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
      p = pval;
    default: // Assume 1-Persistent
      isPPersistent = true;
      p = 1;
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
  }
  else{
    // Increment running sum for average number of packets in queue
    if (state != TRANSMITTING)
      departure(curTick);
    else
      detecting();
  }
}

int Simulator::calc_arrival_time(){
  // 1 tick is interpreted as 1 ms
  double u = (double)rand() / (double)RAND_MAX; //generate random number between 0...1
  int arrival_time = ((double)(-1 / lambda)*(double)log(1 - u)) / tick_duration;
  return arrival_time;
}

void Simulator::generation(float t){
  if (t >= tArrival) {

    Packet new_packet;
    tArrival = t + calc_arrival_time();
    //std::cout << "Station ID: " << stationID << " random arrival time is :" << tArrival << std::endl;
    new_packet.generationTime = t;
    packet_queue.push(new_packet);
  }
}

void Simulator::departure(float t) {
  if (packet_queue.size() > 0) {
    Packet departingPacket = packet_queue.front();
    for (int i = 0; i <= numNodes; i++){
      float propDelay = abs(stationID - i) * 10 / (propogationSpeed * tick_duration);
      (departingPacket.timeToFirstBit).insert(std::make_pair(i, propDelay));
      (departingPacket.timeToLastBit).insert(std::make_pair(i, propDelay + transmissionDelay));
    }
    departingPacket.senderID = stationID;
    departingPacket.isReceived = false;

    if (localAccessToBus->bus.size() == 0){
      packet_queue.pop();
      localAccessToBus->bus.insert(std::make_pair(stationID, departingPacket));
      state = TRANSMITTING;
    }
    else if (localAccessToBus->bus.size() >= 1){
      bool isBusFree = true;
      for (Bus::iter iterator = localAccessToBus->bus.begin(); iterator != localAccessToBus->bus.end(); iterator++){

        int wait = (iterator->second.timeToFirstBit).at(stationID);
        if (wait <= 0 && !iterator->second.isReceived){
          isBusFree = false;
          //do nothing; (sensing medium) and the medium is busy
          if (isNonPersistent)
            waitCounter = waitExpBackoff(currentI)/tick_duration;
        }
      }
      if(isBusFree){
        packet_queue.pop();
        localAccessToBus->bus.insert(std::make_pair(stationID, departingPacket));
        state = TRANSMITTING;
      }
    }
  }
}

void Simulator::detecting(){
  for (Bus::iter iterator = localAccessToBus->bus.begin(); iterator != localAccessToBus->bus.end(); iterator++){
    if (iterator->second.isReceived && iterator->first == stationID){
      currentI = 0;
      state = IDLE;
    }
    else{
      // detect for collisions
      if (localAccessToBus->bus.size() > 1){
        if (iterator->first != stationID){
          if (iterator->second.timeToFirstBit.at(stationID) <= 0){
            localAccessToBus->isCollision = true;
            currentI++;
            if (currentI < 10){
              waitCounter = waitExpBackoff(currentI)/tick_duration;
              state = WAITING;
            }
            else{
              //TODO: need to go to error state - what does that mean?
              currentI = 0;
            }
          }
        }
      }
    }
  }
}
