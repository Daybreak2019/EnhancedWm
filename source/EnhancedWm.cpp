/***********************************************************
 * Author: Wen Li
 * Date  : 10/07/2019
 * Describe: pattern matching: WM algorighm
 * History:
   <1> 10/07/2019, create
************************************************************/
#include <EnhancedWm.h>
#include <Log.h>

DWORD EnHancedWm::AddStrPtn (T_StrPtn2Id &StrPtn2Id, DWORD PtdId, string StrPtn)
{
    DWORD StrId;
            
    auto StrIt = StrPtn2Id.find (StrPtn);
    if (StrIt == StrPtn2Id.end ())
    {
        StrId = StrPtn2Id.size () + 1;
        StrPtn2Id [StrPtn]   = StrId;
        m_StrPatterns[StrId] = StrPtn;
    }
    else
    {
        StrId = StrIt->second;
    }

    m_StrId2StrVec[PtdId].push_back (StrId);
    cout<<StrId<<" ";
            
    m_Min = (m_Min > StrPtn.length ())?StrPtn.length ():m_Min;

    return StrId;
}


/* currently support: A.*B */
VOID EnHancedWm::LexicalParse (T_Pid2Pattern* Patterns)
{
    unordered_map<string, DWORD> StrPtn2Id;

    for (auto it = Patterns->begin(), end = Patterns->end(); it != end; it++)
    {
        string Ptn = it->second;
        cout<<"Lexical Parse: ["<<it->first<<"]"<<Ptn<<": ";

        char *Strc = new char[strlen(Ptn.c_str())+1];
        strcpy(Strc, Ptn.c_str());

        string Temp = Ptn + ".*";
        size_t Pos = Temp.find(".*");
        while(Pos != Temp.npos)
        {
            string StrPtn = Temp.substr(0, Pos);
            AddStrPtn (StrPtn2Id, it->first, StrPtn);
            
            Temp = Temp.substr(Pos+2, Temp.size());
            Pos = Temp.find(".*");
        }
        cout<<"\r\n";
    }

    DebugLog ("Update MinLen: %u\r\n", m_Min);
}

VOID EnHancedWm::AddOnePtnToGraph (DWORD PtnId, vector <DWORD> *StrVec)
{
    PtnNode* PNode;

	PNode = m_PtnGraph.Root;

    /* query along the graph */
    auto It = StrVec->begin ();
	for (; It != StrVec->end (); It++)
	{
        DWORD StrId = *It;
		auto ItN = PNode->NxtTable.find (*It);
        if (ItN == PNode->NxtTable.end ())
        {
            break;
        }

        PNode = ItN->second;
	}

	while(It != StrVec->end ())
	{
        PtnNode *NewNode = m_PtnGraph.AddNode ();
		PNode->NxtTable[*It] = NewNode;
		PNode = NewNode;

        It++;
	}

	/* add output */
    if (PNode->OutPut == NULL)
    {
        PNode->OutPut = new set<DWORD>;
        assert (PNode->OutPut != NULL);
    }
	PNode->OutPut->insert (PtnId);

    return;
}


VOID EnHancedWm::CompilePtnGraph (T_Pid2Pattern* Patterns)
{
    LexicalParse (Patterns);

    for (auto ItPtn = m_StrId2StrVec.begin (); ItPtn != m_StrId2StrVec.end (); ItPtn++)
	{		
        vector <DWORD> *StrVec = &(ItPtn->second);

        AddOnePtnToGraph(ItPtn->first, StrVec);
	}

    return;
}


void EnHancedWm::CompileWm() 
{
	for (auto ItP = m_StrPatterns.begin(), EndP = m_StrPatterns.end(); ItP != EndP; ItP++) 
    {
        BYTE *Ptn = (BYTE*)((ItP->second).c_str());
        
		for (DWORD Ch = 0; Ch < m_Min - BLOCK_SIZE + 1; ++Ch) 
        {
			DWORD Block = Ptn[Ch]<<16 | Ptn[Ch+1]<<8 | Ptn[Ch+2];
            
			DWORD BlockPos = m_Min - Ch - BLOCK_SIZE;
			DWORD Shift = (BlockPos == 0) ? (m_Min - BLOCK_SIZE + 1) : BlockPos;

			/* Shift table */
			if (m_ShiftTable.count(Block)) 
            {
				Shift = m_ShiftTable[Block];
				m_ShiftTable[Block] = min(m_ShiftTable[Block], BlockPos);
			}
			else
			{
				m_ShiftTable[Block] = BlockPos;
			}

			/* HASH table */
			if (BlockPos != 0) 
            {
                continue;
			}

            m_HashTable[Block].push_back(ItP->first);
		    if (Shift != 0) 
            {
			    if (m_AuxShiftTable.count(Block))
			    {
				    m_AuxShiftTable[Block] = min(m_AuxShiftTable[Block], Shift);
			    }
			    else
			    {
				    m_AuxShiftTable[Block] = Shift;
			    }
			}
		}
	}

	m_Initialized = true;
}


void EnHancedWm::CompileStrGraph() 
{
    for (auto It = m_HashTable.begin (); It != m_HashTable.end (); It++)
    {
        list<DWORD> *StrIdList = &It->second;
        for (auto StrIt = StrIdList->begin (), StrEnd = StrIdList->end(); StrIt != StrEnd; ++StrIt) 
        {
            DWORD StrI = *StrIt;
            string Pattern = m_StrPatterns[StrI];

            cout<<StrI<<" -> "<<Pattern<<"\r\n";
        }               
    }
}


void EnHancedWm::Compile (T_Pid2Pattern *Patterns)
{
    /* COmpile pattern graph */
    CompilePtnGraph (Patterns);
    PtnGraphViz ptnViz ("PtnGraph", &m_PtnGraph, &m_StrPatterns);
    ptnViz.WiteGraph ();

    /* Compile Wm */
    CompileWm ();

    /* COmpile string graph */
    CompileStrGraph ();

    return;
}


T_Result* EnHancedWm::SearchStrResult(const BYTE* Text, const DWORD Length)
{
    if (m_Initialized == false)
    {
        return NULL;
    }
    
    m_StrResult.clear ();
    
    for (DWORD Pos = m_Min - BLOCK_SIZE; Pos < Length; ++Pos) 
    {
        DWORD block = Text[Pos]<<16 | Text[Pos+1]<<8 |Text[Pos+2];
        
        auto shift_value = m_ShiftTable.find(block);
        if (shift_value == m_ShiftTable.end()) 
        {
            Pos += m_Min - BLOCK_SIZE;
            continue;
        }

        if (shift_value->second != 0)
        {
            Pos += shift_value->second - 1;
            continue;
        }


        /* hit */
        auto PtSet = m_HashTable.at(block);
        for (auto PIt = PtSet.begin(), PEnd = PtSet.end(); PIt != PEnd; ++PIt) 
        {
            DWORD Pid = *PIt;
            string *Pattern = &(m_Patterns[Pid]);

            DWORD CurPos = Pos - m_Min + BLOCK_SIZE;
            DWORD Index = 0;

            BYTE* Pt = (BYTE*)Pattern->c_str();
            DWORD Length = Pattern->length();

            const BYTE* Tg = Text + CurPos;
            while (Index < Length) 
            {
                if (*Pt++ != *Tg++) 
                {
                    break;
                }
                
                ++Index;
            }

            if (Index == Length) 
            {
                m_StrResult.push_back (Pid);
            }
        }
                
        Pos += m_AuxShiftTable.at(block) - 1;
    }


    for (auto it = m_StrResult.begin(); it != m_StrResult.end(); it++)
    {
        DWORD Id = *it;
        DebugLog ("Pattern Matching => [%d]%s\r\n", Id, m_Patterns[Id].c_str());
    }


	return &m_StrResult;
}

T_Result* EnHancedWm::SearchPtnResult (T_Result* StrResult)
{
    return NULL;
}


T_Result* EnHancedWm::Search (const BYTE* Text, const DWORD Length)
{
    T_Result* StrResult = SearchStrResult(Text, Length);
    if (!StrResult->size ())
    {
        return &m_PtnResult;
    }

    return SearchPtnResult(StrResult);
}



