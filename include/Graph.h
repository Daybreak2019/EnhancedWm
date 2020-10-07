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

using namespace std;

struct Pattern
{
	DWORD TokenId;
	DWORD MinPattLen;

	DWORD MatchBeg;
	DWORD MatchEnd;
};

struct Node 
{
    DWORD MinInput;
	DWORD MaxInput;

	WORD*  NxtTable;
	Pattern* PtnHdr;
};

struct Graph
{
    set <Node *> NodeSet;
    Node *Root;    
};

#endif 
