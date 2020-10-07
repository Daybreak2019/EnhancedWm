/***********************************************************
 * Author: Wen Li
 * Date  : 10/07/2019
 * Describe: graph
 * History:
   <1> 10/07/2019, create
************************************************************/

#ifndef _GRAPH_H_
#define _GRAPH_H_
#include <BasicType.h>
#include <GraphViz.h>

using namespace std;

class StrNode
{
public:
    DWORD NId;

};



class PtnNode
{
public:
    DWORD NId;
    map <DWORD, PtnNode*> NxtTable;
    set <DWORD> OutPut;

    PtnNode (DWORD Id)
    {
        NId = Id; 
    }
};

template<class NodeTy> class Graph
{
public:
    set <NodeTy *> NodeSet;
    NodeTy *Root;

    Graph ()
    {
        Root = AddNode ();
    }

    inline NodeTy *AddNode ()
    {
        DWORD Id = NodeSet.size ();
        NodeTy *N = new NodeTy (Id);
        assert (N != NULL);

        NodeSet.insert (N);
        return N;
    }
};



class PtnGraph: public Graph<PtnNode> 
{
};

class PtnGraphViz: public GraphViz <PtnNode, PtnGraph>
{

public:
    PtnGraphViz(string GraphName, PtnGraph* Graph):GraphViz<PtnNode, PtnGraph>(GraphName, Graph)
    {
    }

    ~PtnGraphViz ()
    {
    }
};


#endif 
