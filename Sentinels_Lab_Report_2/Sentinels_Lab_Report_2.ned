simple SentinelsL2
{
    parameters:
        bool sendMsgOnInit = default(false); // whether the module should send out a message on initialization
        int limit = default(2);   // another parameter with a default value
        @display("i=block/routing");
    gates:
        input in;
        output out;
}

network SentinelsE2
{
    @display("bgb=912,541");
    submodules:
        Rizve: SentinelsL2 {
            parameters:
                sendMsgOnInit = true;
                @display("i=device/mainframe,#FF80FF;p=228,417;is=vl");
        }
        Rahat: SentinelsL2 {
            parameters:
                sendMsgOnInit = false;
                @display("i=device/mainframe,gold;p=228,196;is=vl");
        }
        Turja: SentinelsL2 {
            parameters:
                sendMsgOnInit = false;
                @display("i=device/antennatower,red;p=469,77;is=vl");
        }
        Ejaj: SentinelsL2 {
            parameters:
                sendMsgOnInit = false;
                @display("i=device/mainframe,#0080FF;p=689,417;is=vl");
        }
        Avishek: SentinelsL2 {
            parameters:
                sendMsgOnInit = false;
                @display("i=device/mainframe,#80FF80;p=688,196;is=vl");
        }
    connections:
        Turja.out --> {  delay = 100ms; } --> Rahat.in;
        Rahat.out --> {  delay = 100ms; } --> Rizve.in;
        Rizve.out --> {  delay = 100ms; } --> Ejaj.in;
        Ejaj.out --> {  delay = 100ms; } --> Avishek.in;
        Avishek.out --> {  delay = 100ms; } --> Turja.in;
}
