/***********************************************************
 * Author: Wen Li
 * Date  : 10/07/2019
 * Describe: pattern matching: WM algorighm
 * History:
   <1> 10/07/2019, create
************************************************************/

#ifndef _WM_H_
#define _WM_H_
#include <BasicType.h> 

using namespace std;

typedef unordered_map<DWORD, DWORD> T_String2Lengh;
typedef unordered_map<DWORD, list<DWORD>> T_Hash;

typedef unordered_map<DWORD, string> T_Pid2String;
typedef set<DWORD> T_Result;


class Wm 
{
public:
	Wm(T_Pid2String* Patterns)
	{
        m_Min = 0xffffffff;
        m_Initialized = false;
        
        InitPatterns (Patterns);

        Compile (Patterns);
	}
    
	~Wm()
	{
	}
    
	
	T_Result* Search(const BYTE* Text, const DWORD Length);

    DWORD WmTest ();

private:
    VOID Compile(T_Pid2String* Patterns);
    
private:
	bool m_Initialized;
	DWORD m_Min;

	T_String2Lengh m_ShiftTable;
	T_String2Lengh m_AuxShiftTable;
	T_Hash m_HashTable;

    
    T_Result m_Result;
    T_Pid2String m_Patterns;

    inline VOID InitPatterns (T_Pid2String* Patterns)
    {
        for (auto it = Patterns->begin(), end = Patterns->end(); it != end; it++)
        {
            string Ptn = it->second;
            m_Patterns[it->first] = it->second;

            DWORD Len = Ptn.length ();
            if (m_Min > Len)
            {
                m_Min = Len;
            }
        }

        return;
    }

};


#endif
