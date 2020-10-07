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
typedef unordered_map<DWORD, string> T_Pid2String;
typedef unordered_map<DWORD, string> T_Pid2Pattern;
typedef unordered_map<DWORD, vector<DWORD>> T_Pid2StrIdVec;
typedef unordered_map<string, DWORD> T_StrPtn2Id;

typedef set<DWORD> T_Result;


class EnHancedWm
{
public:
	EnHancedWm(T_Pid2Pattern* Patterns)
	{
        m_Min = 0xffffffff;
        m_Initialized = false;

        CompilePtnGraph (Patterns);
        
        PtnGraphViz ptnViz ("PtnGraph", &m_PtnGraph);
        ptnViz.WiteGraph ();

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

    
    T_Result m_Result;
    T_Pid2Pattern m_Patterns;
    T_Pid2String m_StrPatterns;
    T_Pid2StrIdVec m_StrId2StrVec;

    PtnGraph m_PtnGraph;


    /* For compiling pattern graph */
    DWORD AddStrPtn (T_StrPtn2Id &StrPtn2Id, DWORD PtdId, string StrPtn);
    VOID LexicalParse (T_Pid2Pattern* Patterns);
    VOID AddOnePtnToGraph (DWORD PtnId, vector <DWORD> *StrVec);
    VOID CompilePtnGraph (T_Pid2Pattern* Patterns);


    /* For compiling string graph */
    VOID CompileStrGraph (T_Pid2String* Patterns);

    

    VOID Compile(T_Pid2Pattern* Patterns);

};


#endif
