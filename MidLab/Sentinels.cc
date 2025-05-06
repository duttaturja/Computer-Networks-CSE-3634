/*
 * Sentinels.cc
 *
 *  Created on: May 7, 2025
 *      Author: Turja Dutta
 */

#include <omnetpp.h>

using namespace omnetpp;

class Node : public cSimpleModule {
private:
    int counter;       // Counter that decreases when sending/forwarding
    bool isSender;     // Whether this node initiates messages
    static int nodeCount; // Tracks total number of nodes in the network

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void sendMessage();
    virtual void removeNode();
};

Define_Module(Node);

// Initialize static variable
int Node::nodeCount = 0;

void Node::initialize() {
    counter = par("limit").intValue();
    isSender = par("sendMsgOnInit").boolValue();
    nodeCount++; // Increment node count on creation
    EV << "Node " << getName() << " initialized, counter = " << counter << ", nodeCount = " << nodeCount << "\n";

    // All nodes schedule periodic send attempts
    scheduleAt(simTime(), new cMessage("triggerSend"));
}

void Node::sendMessage() {
    // Do not send if only one node remains
    if (nodeCount <= 1) {
        EV << getName() << ": Only one node left, not sending\n";
        return;
    }

    if (counter > 0) {
        counter--;
        cMessage *msg = new cMessage("SentinelsLabReport2");
        msg->addPar("sender") = getName();
        msg->addPar("receiver") = par("receiverName").stringValue();
        msg->addPar("acknowledged") = 0;
        msg->addPar("lastForwarder") = getName(); // Initialize lastForwarder

        if (gate("out")->isConnected()) {
            cModule *nextNode = gate("out")->getNextGate()->getOwnerModule();
            EV << getName() << " sent message to " << par("receiverName").stringValue() << " via " << nextNode->getName() << " (Token = " << counter << ")\n";
            send(msg, "out");
        } else {
            EV << getName() << ": Output gate not connected, deleting message\n";
            delete msg;
        }

        // Schedule next send attempt
        if (counter > 0 && nodeCount > 1) {
            scheduleAt(simTime() + 1.0, new cMessage("triggerSend"));
        }

        // Remove node immediately if counter reaches zero
        if (counter == 0 && nodeCount > 1) {
            EV << getName() << "'s counter reached zero, removing node\n";
            removeNode();
        }
    } else {
        EV << getName() << "'s counter is zero, cannot send\n";
    }
}

void Node::handleMessage(cMessage *msg) {
    // Handle self-messages
    if (msg->isSelfMessage()) {
        if (strcmp(msg->getName(), "triggerSend") == 0) {
            sendMessage();
            delete msg;
        }
        return;
    }

    // If only one node remains, process message without forwarding
    if (nodeCount <= 1) {
        const char *sender = msg->par("sender").stringValue();
        const char *receiver = msg->par("receiver").stringValue();
        const char *lastForwarder = msg->par("lastForwarder").stringValue();
        int acknowledged = msg->par("acknowledged").longValue();
        EV << "Message received by " << getName() << " from " << lastForwarder << "\n";
        if (strcmp(receiver, getName()) == 0 && acknowledged == 0) {
            EV << getName() << " is the receiver, acknowledging message from " << sender << "\n";
        } else if (strcmp(sender, getName()) == 0 && acknowledged == 1) {
            EV << getName() << "'s message was acknowledged by " << receiver << "\n";
        }
        EV << getName() << ": Last node, deleting message\n";
        delete msg;
        return;
    }

    // Normal message handling
    const char *sender = msg->par("sender").stringValue();
    const char *receiver = msg->par("receiver").stringValue();
    const char *lastForwarder = msg->par("lastForwarder").stringValue();
    int acknowledged = msg->par("acknowledged").longValue();

    EV << "Message received by " << getName() << " from " << lastForwarder << "\n";

    if (strcmp(receiver, getName()) == 0 && acknowledged == 0) {
        // Node is the receiver, acknowledge and forward
        EV << getName() << " is the receiver, acknowledging message from " << sender << "\n";
        msg->par("acknowledged") = 1;
        msg->par("lastForwarder") = getName();
        if (gate("out")->isConnected()) {
            cModule *nextNode = gate("out")->getNextGate()->getOwnerModule();
            EV << getName() << " forwarding acknowledged message to " << nextNode->getName() << "\n";
            send(msg, "out");
        } else {
            EV << getName() << ": Output gate not connected, deleting message\n";
            delete msg;
        }
    } else if (strcmp(sender, getName()) == 0 && acknowledged == 1) {
        // Message returned to sender with acknowledgment
        EV << getName() << "'s message was acknowledged by " << receiver << "\n";
        delete msg;
        // Schedule next send if sender and conditions allow
        if (isSender && counter > 0 && nodeCount > 1) {
            scheduleAt(simTime() + 1.0, new cMessage("triggerSend"));
        } else if (counter == 0 && nodeCount > 1) {
            EV << getName() << "'s Token reached zero, removing node\n";
            removeNode();
        }
    } else {
        // Node forwards the message
        if (counter > 0) {
            counter--;
            msg->par("lastForwarder") = getName();
            if (gate("out")->isConnected()) {
                cModule *nextNode = gate("out")->getNextGate()->getOwnerModule();
                EV << getName() << " forwarding message to " << nextNode->getName() << " (counter = " << counter << ")\n";
                send(msg, "out");
            } else {
                EV << getName() << ": Output gate not connected, deleting message\n";
                delete msg;
            }
            // Remove node immediately if counter reaches zero
            if (counter == 0 && nodeCount > 1) {
                EV << getName() << "'s counter reached zero, removing node\n";
                removeNode();
            }
        } else {
            EV << getName() << "'s counter is zero, cannot forward, deleting message\n";
            delete msg;
        }
    }
}

void Node::removeNode() {
    nodeCount--; // Decrement node count
    EV << "Node count decreased to " << nodeCount << "\n";

    if (nodeCount > 1) {
        // Get predecessor and successor
        cGate *inGate = gate("in");
        cGate *outGate = gate("out");
        cGate *predecessorOut = inGate ? inGate->getPreviousGate() : nullptr;
        cGate *successorIn = outGate ? outGate->getNextGate() : nullptr;

        // Disconnect this node
        if (predecessorOut && predecessorOut->isConnected()) {
            predecessorOut->disconnect();
        }
        if (outGate && outGate->isConnected()) {
            outGate->disconnect();
        }

        // Reconnect predecessor to successor
        if (predecessorOut && successorIn) {
            predecessorOut->connectTo(successorIn);
            cModule *predecessor = predecessorOut->getOwnerModule();
            cModule *successor = successorIn->getOwnerModule();
            EV << "Reconnected " << predecessor->getName() << " to " << successor->getName() << "\n";
        }
    } else {
        EV << "Last node remaining, no reconnection performed\n";
    }

    // Delete this node
    EV << "Removing node " << getName() << " from the ring\n";
    callFinish();
    deleteModule();
}



