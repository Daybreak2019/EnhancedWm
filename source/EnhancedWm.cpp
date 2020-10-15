/***********************************************************
 * Author: Wen Li
 * Date  : 10/07/2019
 * Describe: pattern matching: WM algorighm
 * History:
   <1> 10/07/2019, create
************************************************************/
#include <EnhancedWm.h>
#include <Log.h>

DWORD EnHancedWm::AddStrPtn (T_StrPtn2Id &StrPtn2Id, DWORD PtdId, string StrPtn, DWORD Depth)
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
    auto Dit = m_StrId2Depth.find (StrId);
    if (Dit == m_StrId2Depth.end ())
    {
        m_StrId2Depth[StrId] = (1 << Depth);
    }
    else
    {
        Dit->second |= (1 << Depth);
    }
    cout<<StrId<<"(D "<<m_StrId2Depth[StrId]<<") ";
            
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
        DWORD Depth = 0;
        while(Pos != Temp.npos)
        {
            string StrPtn = Temp.substr(0, Pos);
            AddStrPtn (StrPtn2Id, it->first, StrPtn, Depth);
            
            Temp = Temp.substr(Pos+2, Temp.size());
            Pos = Temp.find(".*");
            Depth++;
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

VOID EnHancedWm::AddOneStrToGraph (StrGraph *SG, DWORD StrId, char *String)
{
    StrNode* SNode;

	SNode = SG->Root;

    /* query along the graph */
    BYTE C;
    DWORD Index = 0;
	while ((C = String[Index]) != 0)
	{
		auto ItN = SNode->NxtTable.find (C);
        if (ItN == SNode->NxtTable.end ())
        {
            break;
        }

        SNode = ItN->second;
        Index++;
	}

	while((C = String[Index]) != 0)
	{
        StrNode *NewNode = SG->AddNode ();
		SNode->NxtTable[C] = NewNode;
		SNode = NewNode;

        Index++;
	}

	/* add output */
    if (SNode->OutPut == NULL)
    {
        SNode->OutPut = new set<DWORD>;
        assert (SNode->OutPut != NULL);
    }
	SNode->OutPut->insert (StrId);

    return;
}


void EnHancedWm::CompileStrGraph() 
{
    for (auto It = m_HashTable.begin (); It != m_HashTable.end (); It++)
    {
        list<DWORD> *StrIdList = &It->second;
        StrGraph *SG = new StrGraph ();
        assert (SG != NULL);
        
        for (auto StrIt = StrIdList->begin (), StrEnd = StrIdList->end(); StrIt != StrEnd; ++StrIt) 
        {
            DWORD StrId = *StrIt;
            string Pattern = m_StrPatterns[StrId];

            cout<<StrId<<" -> "<<Pattern<<"\r\n";
            AddOneStrToGraph (SG, StrId, (char *)Pattern.c_str());
        }

        /* hash to graph */
        m_HashGraph[It->first] = SG;
    }

    m_HashTable.clear ();
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


        /* hit, match in graph */
        StrGraph *SG = m_HashGraph.at(block);
        StrNode *SN  = SG->Root;

        WORD Start = (WORD)(Pos - (m_Min - BLOCK_SIZE));
        WORD End   = Start;
        BYTE* Tg = (BYTE *)(Text + Start);
        cout<<Tg<<"\r\n";
        auto ItN = SN->NxtTable.find (*Tg);
    	while (ItN != SN->NxtTable.end ())
    	{
    		SN = ItN->second;
            if (SN->OutPut)
            {
                m_StrResult.push_back (M_Result (*(SN->OutPut->begin()), Start, End));
            }
            Tg++;
            End++;

            ItN = SN->NxtTable.find (*Tg);
    	}
                
        Pos += m_AuxShiftTable.at(block) - 1;
    }

	return &m_StrResult;
}

T_Result* EnHancedWm::SearchPtnResult (T_Result* StrResult)
{
    T_MatchQueue MQ;
    MQ[0].push_back (QueueNode (m_PtnGraph.Root, 0, 0));

    for (auto It = StrResult->begin (); It != StrResult->end (); It++)
    {
        M_Result *MR = &(*It);
        DWORD StrId  = MR->ID;
        DWORD Depth  = m_StrId2Depth[StrId];
        DebugLog ("String Matching => [%d][D%X](%u, %u)%s\r\n", StrId, Depth, MR->Start, MR->End, m_StrPatterns[StrId].c_str());

        DWORD Qno = 0;
        for (; Depth != 0; Qno++, Depth = Depth>>1)
        {
            if (!(Depth & 1))
            {
                continue;
            }

            auto Mqit = MQ.find (Qno);
            if (Mqit == MQ.end())
            {
                continue;
            }

            vector<QueueNode> *Queue = &(Mqit->second);
            for (auto Qt = Queue->begin (); Qt != Queue->end(); Qt++)
            {
                /* check position */
                if (MR->Start < Qt->End)
                {
                    continue;
                }
                
                PtnNode *PN = Qt->PN;
                auto NxtIt = PN->NxtTable.find (StrId);
                if (NxtIt != PN->NxtTable.end ())
                {
                    PtnNode *NxtN = NxtIt->second;
                    if (NxtN->OutPut != NULL)
                    {
                        m_PtnResult.push_back (M_Result(*(NxtN->OutPut->begin()), Qt->Start, MR->End));
                    }

                    if (Qno == 0)
                    {
                        MQ[Qno+1].push_back (QueueNode(NxtN, MR->Start, MR->End));
                    }
                    else
                    {
                        MQ[Qno+1].push_back (QueueNode(NxtN, Qt->Start, MR->End));
                    }         
                }
            }
        }    
    }
    
    return &m_PtnResult;
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



