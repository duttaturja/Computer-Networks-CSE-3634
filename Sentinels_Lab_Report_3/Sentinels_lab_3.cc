#include <omnetpp.h>
using namespace omnetpp;
class SentinelsL3 : public cSimpleModule
{
  private:
    int senderIndex;
    int receiverIndex;
    static int totalMessageCount;

  protected:
    void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

int SentinelsL3::totalMessageCount = 0;
Define_Module(SentinelsL3);

void SentinelsL3::initialize()
{
    senderIndex = par("sender");
    receiverIndex = par("receiver");

    if (getIndex() == senderIndex) {
        char msgname[20];
        sprintf(msgname, "turja-%d", getIndex());
        cMessage *msg = new cMessage(msgname);
        scheduleAt(0.0, msg);
    }
    EV << "Sender module index: " << senderIndex << "\n";
    EV << "Receiver module index: " << receiverIndex << "\n";
}

void SentinelsL3::handleMessage(cMessage *msg)
{
    if (getIndex() == receiverIndex) {
        EV << "Message " << msg << " arrived at receiver " << receiverIndex << ".\n";
        delete msg;
    } else {
        forwardMessage(msg);
    }
}

void SentinelsL3::forwardMessage(cMessage *msg)
{
    totalMessageCount++;
    int n = gateSize("gate");
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " on gate[" << k << "]. Total messages forwarded: " << totalMessageCount << "\n";
    send(msg, "gate$o", k);
}


