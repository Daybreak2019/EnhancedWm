/***********************************************************
 * Author: Wen Li
 * Date  : 10/07/2019
 * Describe: graph virtualization
 * History:
   <1> 10/07/2019 , create
************************************************************/
#ifndef _GRAPHVIZ_H_
#define _GRAPHVIZ_H_
#include <fstream>

using namespace std;

template<class NodeTy, class GraphType> class GraphViz 
{
protected:
    FILE  *m_File;
    GraphType *m_Graph;
    string m_GraphName;
    unordered_map<DWORD, string> *m_StrPtn;

protected:
    inline VOID WriteHeader (string GraphName) 
    {
        fprintf(m_File, "digraph \"%s\"{\n", GraphName.c_str());
        fprintf(m_File, "\tlabel=\"%s\";\n", GraphName.c_str()); 

        return;
    }

    virtual inline string GetNodeLabel(NodeTy *Node) 
    { 
        string str = "N" + to_string (Node->NId);

        if (Node->OutPut != NULL)
        {
            str += ", Out: ";
            for (auto It = Node->OutPut->begin (); It != Node->OutPut->end (); It++)
            {
                str += to_string (*It) + " ";
            }
        }     
        
        return str;
    }

    
    virtual inline string GetNodeAttributes(NodeTy *Node) 
    {
        if (Node->OutPut != NULL)
        {
            return "color=red";
        }
        else
        {
            return "color=black";
        }
    }

    virtual inline string GetEdgeLabel(DWORD Label) 
    {
        auto It = m_StrPtn->find (Label);
        if (It == m_StrPtn->end ())
        {
            return "";
        }
        
        return It->second;
    }

    virtual inline string GetEdgeAttributes(NodeTy *Src, NodeTy *Dst) 
    {
        return "color=black";
    }
 
    inline VOID WriteNodes(NodeTy *Node) 
    {
        /* NodeID [color=grey,label="{NodeID: 0}"]; */
        string str;

        str = "N" + to_string (Node->NId) + "[" + GetNodeAttributes (Node) + ",label=\"{" +
              GetNodeLabel (Node) + "}\"];";

        fprintf(m_File, "\t%s\n", str.c_str());
        return;        
    }
 

    inline VOID WriteEdge(NodeTy *Src, NodeTy *Dst, DWORD Label) 
    {
        DWORD SrcId = Src->NId;
        DWORD DstId = Dst->NId;
        
        /* NodeId -> NodeId[style=solid,color=black]; */
        string str;

        str = "\tN" + to_string (SrcId) + " -> " + "N" + to_string (DstId) +
              "[" + GetEdgeAttributes (Src, Dst) + ",label=\"{" +
              GetEdgeLabel (Label) + "}\"];";           

        fprintf(m_File, "%s\n", str.c_str());
        return; 
     
    }

public:
    GraphViz(string GraphName, GraphType   * Graph, unordered_map<DWORD, string> *StrPtn) 
    {
        m_GraphName = GraphName;
        
        GraphName = GraphName + ".dot";
        m_File  =fopen (GraphName.c_str(), "w");
        assert (m_File != NULL);

        m_Graph  = Graph;
        m_StrPtn = StrPtn;
    }

    ~GraphViz()
    {
        fclose (m_File);
    }

    VOID WiteGraph () 
    {
        WriteHeader(m_GraphName);

        for (auto It = m_Graph->NodeSet.begin (), End = m_Graph->NodeSet.end (); It != End; It++)
        {
            NodeTy *Node = *It;
            WriteNodes (Node);

            for (auto It = Node->NxtTable.begin (); It != Node->NxtTable.end (); It++)
            {
                NodeTy *Dst = It->second;
                
                WriteEdge (Node, Dst, It->first);
            }
        }

        fprintf(m_File, "}\n");
    }   
};




#endif 
