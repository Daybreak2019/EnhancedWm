/***********************************************************
 * Author: Wen Li
 * Date  : 10/07/2019
 * Describe: pattern matching: Enhanced WM algorighm
 * History:
   <1> 10/07/2019 , create
************************************************************/

#ifndef _ENHANCEDWM_H_
#define _ENHANCEDWM_H_
#include <BasicType.h> 
#include <Graph.h> 


using namespace std;

typedef unordered_map<DWORD, DWORD> T_String2Lengh;
typedef unordered_map<DWORD, list<DWORD>> T_Hash;
typedef unordered_map<DWORD, StrGraph*> T_HashG;

typedef unordered_map<DWORD, string> T_Pid2String;
typedef unordered_map<DWORD, string> T_Pid2Pattern;
typedef unordered_map<DWORD, vector<DWORD>> T_Pid2StrIdVec;
typedef unordered_map<string, DWORD> T_StrPtn2Id;
typedef unordered_map<DWORD, DWORD> T_StrPtnDepth;



struct M_Result
{
    WORD Start;
    WORD End;
    DWORD ID;

    M_Result (DWORD id, WORD s, WORD e)
    {
        ID    = id;
        Start = s;
        End   = e;        
    }
};
typedef vector <M_Result> T_Result;


struct QueueNode
{
    PtnNode* PN;
    WORD Start;
    WORD End;

    QueueNode (PtnNode* n, WORD s, WORD e)
    {
        PN    = n;
        Start = s;
        End   = e;
    }
};

typedef unordered_map<DWORD, vector<QueueNode>> T_MatchQueue;



class EnHancedWm
{
public:
	EnHancedWm(T_Pid2Pattern* Patterns)
	{
        m_Min = 0xffffffff;
        m_Initialized = false;

        Compile (Patterns);
	}
    
	~EnHancedWm()
	{
	}
    
	
	T_Result* Search(const BYTE* Text, const DWORD Length);
    
private:
	bool m_Initialized;
	DWORD m_Min;

	T_String2Lengh m_ShiftTable;
	T_String2Lengh m_AuxShiftTable;
	T_Hash m_HashTable;
    T_HashG m_HashGraph;

    
    T_Result m_PtnResult;
    T_Result m_StrResult;
    
    T_Pid2Pattern m_Patterns;
    T_Pid2String m_StrPatterns;
    T_Pid2StrIdVec m_StrId2StrVec;
    T_StrPtnDepth m_StrId2Depth;

    PtnGraph m_PtnGraph;

private:

    /********************************************************************************
    *
    *   Compile
    *
    *********************************************************************************/

    /* For compiling pattern graph */
    DWORD AddStrPtn (T_StrPtn2Id &StrPtn2Id, DWORD PtdId, string StrPtn, DWORD Depth);
    VOID LexicalParse (T_Pid2Pattern* Patterns);
    VOID AddOnePtnToGraph (DWORD PtnId, vector <DWORD> *StrVec);
    VOID CompilePtnGraph (T_Pid2Pattern* Patterns);


    /* For compiling string graph */
    VOID AddOneStrToGraph (StrGraph *SG, DWORD StrId, char *String);
    VOID CompileStrGraph ();
   
    /* For compiling Wm */
    VOID CompileWm();

    /* Compile Entry */
    VOID Compile(T_Pid2Pattern* Patterns);


    /********************************************************************************
    *
    *   Search
    *
    *********************************************************************************/
    T_Result* SearchStrResult (const BYTE* Text, const DWORD Length);
    T_Result* SearchPtnResult (T_Result* StrResult);

};


#endif
