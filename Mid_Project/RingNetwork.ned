network RingNetwork
{
    parameters:
        int numNodes = default(5);
        int totalTokens = default(10); // Total tokens in the network
    submodules:
        node[numNodes]: RingNode {
            parameters:
                nodeId = index;
                numNodes = parent.numNodes;
                initialTokens = index == 0 ? parent.totalTokens : 0; // Assign all tokens to node 0 initially
        }
    connections:
        for i=0..numNodes-1 {
            node[i].out --> node[(i+1)%numNodes].in;
        }
}