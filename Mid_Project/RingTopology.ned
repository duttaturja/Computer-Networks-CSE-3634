simple RingNode
{
    parameters:
        int nodeId;
        int numNodes;
        int initialTokens = default(0); // Tokens assigned initially to this node
    gates:
        input in;
        output out;
}

// Network definition with explicit node instances
network RingNetwork
{
    parameters:
        int totalTokens = default(10); // Total tokens in the network
        @display("bgb=522,257");
    submodules:
        node0: RingNode {
            parameters:
                nodeId = 0;
                numNodes = 5;
                initialTokens = totalTokens; // Start with all tokens

        }
        node1: RingNode {
            parameters:
                nodeId = 1;
                numNodes = 5;
                initialTokens = 0; // No tokens initially

        }
        node2: RingNode {
            parameters:
                nodeId = 2;
                numNodes = 5;
                initialTokens = 0; // No tokens initially

        }
        node3: RingNode {
            parameters:
                nodeId = 3;
                numNodes = 5;
                initialTokens = 0; // No tokens initially

        }
        node4: RingNode {
            parameters:
                nodeId = 4;
                numNodes = 5;
                initialTokens = 0; // No tokens initially

        }
    connections:
        node0.out --> {  delay = 10ms; } --> node1.in;
        node1.out --> {  delay = 10ms; } --> node2.in;
        node2.out --> {  delay = 10ms; } --> node3.in;
        node3.out --> {  delay = 10ms; } --> node4.in;
        node4.out --> {  delay = 10ms; } --> node0.in;
}
