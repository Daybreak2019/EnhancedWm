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
    map <BYTE, StrNode*> NxtTable;
    set <DWORD> *OutPut;

    StrNode ()
    {
        OutPut = NULL; 
    }

    ~StrNode ()
    {
        if (OutPut)
        {
            delete OutPut;
        }
    }

};



class PtnNode
{
public:
    DWORD NId;
    map <DWORD, PtnNode*> NxtTable;
    set <DWORD> *OutPut;

    PtnNode (DWORD Id)
    {
        NId = Id;
        OutPut = NULL;
    }

    ~PtnNode ()
    {
        if (OutPut)
        {
            delete OutPut;
        }
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

    ~Graph ()
    {
        for (auto It = NodeSet.begin (); It != NodeSet.end (); It++)
        {
            delete (*It);
        }
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

class StrGraph: public Graph<StrNode> 
{
};


class PtnGraphViz: public GraphViz <PtnNode, PtnGraph>
{

public:
    PtnGraphViz(string GraphName, PtnGraph* Graph, unordered_map<DWORD, string> *StrPtn):
        GraphViz<PtnNode, PtnGraph>(GraphName, Graph, StrPtn)
    {
    }

    ~PtnGraphViz ()
    {
    }
};


#endif 
