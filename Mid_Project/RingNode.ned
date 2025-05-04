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