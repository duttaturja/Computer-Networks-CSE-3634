simple Node
{
    parameters:
        @display("i=block/routing");
        string receiverName @prompt("Receiver name") = default("");
        int limit @prompt("Counter limit") = default(0);
        bool sendMsgOnInit = default(false);
    gates:
        input in;
        output out;
}

network SentinelsLabReport2
{
    @display("bgb=688,263");
    submodules:
        Rahat: Node {
            @display("p=344,65");
        }
        Rizve: Node {
            @display("p=448,117");
        }
        Turja: Node {
            @display("p=409,207");
        }
        Eju: Node {
            @display("p=294,207");
        }
        Avishek: Node {
            @display("p=239,117");
        }
    connections:
        Rahat.out --> Rizve.in;
        Rizve.out --> Turja.in;
        Turja.out --> Eju.in;
        Eju.out --> Avishek.in;
        Avishek.out --> Rahat.in;
}
