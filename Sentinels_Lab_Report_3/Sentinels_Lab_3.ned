simple SentinelsL3
{
    parameters:
        @display("i=block/routing");
        bool sendMsgOnInit = default(false);

        int sender = default(3);  // Default sender index
        int receiver = default(0);

    gates:
        inout gate[];  // declare two way connections
}

// using two way connections to further simplify the network definition
network SentinelsR3
{
    @display("bgb=718,425");
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100ms;
        }
    submodules:
        turja[10]: SentinelsL3;
    connections:
        turja[3].gate++ <--> Channel <--> turja[7].gate++;
        turja[6].gate++ <--> Channel <--> turja[4].gate++;
        turja[8].gate++ <--> Channel <--> turja[7].gate++;
        turja[0].gate++ <--> Channel <--> turja[5].gate++;
        turja[1].gate++ <--> Channel <--> turja[3].gate++;
        turja[2].gate++ <--> Channel <--> turja[3].gate++;
        turja[4].gate++ <--> Channel <--> turja[5].gate++;
        turja[4].gate++ <--> Channel <--> turja[0].gate++;
        turja[7].gate++ <--> Channel <--> turja[4].gate++;
        turja[8].gate++ <--> Channel <--> turja[9].gate++;
}
