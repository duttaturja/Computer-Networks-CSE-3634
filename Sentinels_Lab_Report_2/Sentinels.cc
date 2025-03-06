/*
 * Sentinels.cc
 *
 *  Created on: Mar 5, 2025
 *      Author: Turja Dutta
 */




#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;
class SentinelsL2 : public cSimpleModule
{
  private:
    int counter;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(SentinelsL2);

void SentinelsL2::initialize()
{
    counter = par("limit");

    if (par("sendMsgOnInit").boolValue() == true) {
        EV << "Sending initial message\n";
        cMessage *msg = new cMessage("SentinelsLabReport2");
        send(msg, "out");
    }
}

void SentinelsL2::handleMessage(cMessage *msg)
{
    counter--;
    if (counter == 0) {
        EV << getName() << "'s counter reached zero, deleting message\n";
        delete msg;
    }
    else {
        EV << getName() << "'s counter is " << counter << ", sending back message\n";
        send(msg, "out");
    }
}
