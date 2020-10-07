#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <EnhancedWm.h>
#include <GraphViz.h>


int main(int argc, char** argv)
{
    T_Pid2Pattern Patterns;
    Patterns[1] = "www.liwen.cn.*liwen";
    Patterns[2] = "www.liwen.cn.*cpts553";
    Patterns[3] = "www.liwen.cn.*cpts553.*liwen";
    
	EnHancedWm EnWm(&Patterns);
	
	return 0;
}


