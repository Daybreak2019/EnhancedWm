#include <EnhancedWm.h>
#include <Log.h>


int main(int argc, char** argv)
{
    T_Pid2Pattern Patterns;
    Patterns[1] = "www.liwen.cn.*liwen";
    Patterns[2] = "www.liwen.cn.*cpts553";
    Patterns[3] = "4567cn.*cpts553.*liwen";
    
	EnHancedWm EnWm(&Patterns);

    string Context = "xxxfdfdfawww.liwen.cn1fasdf4567cnjkadskfsjadfasliwencpts553liwen89sdfkjalsjflkasdgfajg";

    T_Result *Res = EnWm.Search ((const BYTE*)Context.c_str (), Context.length());
    for (auto It = Res->begin (); It != Res->end (); It++)
    {
        DWORD ID = It->ID;
        DebugLog ("Pattern Matching => [%d](%u, %u) \r\n", ID, (DWORD)It->Start, (DWORD)It->End);
    }
	
	return 0;
}


