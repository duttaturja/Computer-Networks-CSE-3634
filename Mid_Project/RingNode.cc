/*
 * RingNode.cc
 *
 *  Created on: May 1, 2025
 *      Author: Turja Dutta
 */


#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

// Message types
enum MessageType {
    TOKEN_MSG = 1,
    DATA_MSG = 2
};

// Message class for token passing
class TokenMessage : public cMessage {
  private:
    int tokenCount;

  public:
    TokenMessage(const char *name = nullptr, MessageType type = TOKEN_MSG) : cMessage(name, type), tokenCount(0) {}

    void setTokenCount(int count) { tokenCount = count; }
    int getTokenCount() const { return tokenCount; }
};

// Message class for data messages
class DataMessage : public cMessage {
  private:
    int sourceId;
    int destinationId;
    std::string content;

  public:
    DataMessage(const char *name = nullptr) : cMessage(name, DATA_MSG), sourceId(-1), destinationId(-1) {}

    void setSourceId(int id) { sourceId = id; }
    int getSourceId() const { return sourceId; }

    void setDestinationId(int id) { destinationId = id; }
    int getDestinationId() const { return destinationId; }

    void setContent(const std::string &msg) { content = msg; }
    const std::string& getContent() const { return content; }
};

class RingNode : public cSimpleModule {
  private:
    int nodeId;
    int numNodes;
    bool hasToken;
    int tokenCount;
    simtime_t sendMessageTime;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void sendDataMessage();
    void passToken();

  public:
    RingNode();
    virtual ~RingNode();
};

Define_Module(RingNode);

RingNode::RingNode() {
    // Constructor
}

RingNode::~RingNode() {
    // Destructor - clean up any dynamically allocated resources
}

void RingNode::initialize() {
    nodeId = par("nodeId");
    numNodes = par("numNodes");
    tokenCount = par("initialTokens");
    hasToken = (tokenCount > 0);

    // Schedule time to send messages if we have tokens
    if (hasToken) {
        EV << "Node " << nodeId << " initialized with " << tokenCount << " tokens.\n";
        sendMessageTime = simTime() + dblrand() * 5.0; // Random time to send the first message
        scheduleAt(sendMessageTime, new cMessage("selfMsg"));
    } else {
        EV << "Node " << nodeId << " initialized with no tokens.\n";
    }
}

void RingNode::handleMessage(cMessage *msg) {
    // Check if it's a self-message (time to send a data message)
    if (msg->isSelfMessage()) {
        if (hasToken && tokenCount > 0) {
            // Send a data message to a random node
            sendDataMessage();

            // Schedule next send
            scheduleAt(simTime() + dblrand() * 10.0, new cMessage("selfMsg"));
        }
        delete msg;
        return;
    }

    // Process incoming messages
    if (msg->getKind() == TOKEN_MSG) {
        // Received token
        TokenMessage *tokenMsg = check_and_cast<TokenMessage *>(msg);
        tokenCount = tokenMsg->getTokenCount();
        hasToken = true;

        EV << "Node " << nodeId << " received token with " << tokenCount << " token count.\n";

        // Hold token for a while, then pass it or use it
        if (dblrand() < 0.3 && tokenCount > 0) {  // 30% chance to use token right away
            scheduleAt(simTime() + 1.0, new cMessage("selfMsg"));
        } else {
            // Pass token after some delay
            scheduleAt(simTime() + 2.0, new cMessage("passToken"));
        }
        delete msg;
    }
    else if (msg->getKind() == DATA_MSG) {
        // Process data message
        DataMessage *dataMsg = check_and_cast<DataMessage *>(msg);

        // Check if this node is the destination
        if (dataMsg->getDestinationId() == nodeId) {
            EV << "Node " << nodeId << " received message from Node " << dataMsg->getSourceId()
               << ": " << dataMsg->getContent() << "\n";
            delete msg;
        } else {
            // Forward message to next node
            EV << "Node " << nodeId << " forwarding message from Node " << dataMsg->getSourceId()
               << " to Node " << dataMsg->getDestinationId() << "\n";
            send(msg, "out");
        }
    }
    else if (strcmp(msg->getName(), "passToken") == 0) {
        // Time to pass the token
        passToken();
        delete msg;
    }
}

void RingNode::sendDataMessage() {
    if (!hasToken || tokenCount <= 0) {
        EV << "Node " << nodeId << " cannot send message: no token available.\n";
        return;
    }

    // Decrement token count
    tokenCount--;

    // Create a data message to a random destination
    int destId;
    do {
        destId = intuniform(0, numNodes - 1);
    } while (destId == nodeId);

    DataMessage *dataMsg = new DataMessage("dataMsg");
    dataMsg->setSourceId(nodeId);
    dataMsg->setDestinationId(destId);
    dataMsg->setContent("Hello from Node " + std::to_string(nodeId) + " to Node " + std::to_string(destId));

    EV << "Node " << nodeId << " sending message to Node " << destId
       << ". Remaining tokens: " << tokenCount << "\n";

    send(dataMsg, "out");

    // If no more tokens, pass the token
    if (tokenCount == 0) {
        passToken();
    }
}

void RingNode::passToken() {
    if (!hasToken) {
        return;
    }

    EV << "Node " << nodeId << " passing token with count " << tokenCount << " to next node.\n";

    // Create token message and send it to next node
    TokenMessage *tokenMsg = new TokenMessage("token");
    tokenMsg->setTokenCount(tokenCount);

    // Pass token to next node
    send(tokenMsg, "out");

    // No longer have the token
    hasToken = false;
}

