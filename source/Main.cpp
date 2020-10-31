#include <EnhancedWm.h>
#include <Wm.h>
#include <Log.h>
#include <time.h>
#include <sys/time.h>
#include <regex.h>


using namespace std;

T_Pid2Pattern wmPattern;
DWORD Seed = 0;

static DWORD GetPhyMemUse ()
{
    pid_t pid = getpid();

    std::string FileName = "/proc/" + std::to_string(pid) + "/status";
    FILE *F = fopen (FileName.c_str(), "r");

    char Buf[256] = {0};
    while (!feof(F))
    {
        assert (fgets (Buf, sizeof(Buf), F) != NULL);
        if (strstr(Buf, "VmRSS"))
        {
            break;
        }
    }
    fclose(F);

    DWORD MemSize = 0;
    char ItemName[128];
    sscanf (Buf, "%s %u", ItemName, &MemSize);

    return MemSize;
}


long getTimeUsec()
{
    struct timeval t;
    gettimeofday(&t, 0);
    return (long)((long)t.tv_sec * 1000 * 1000 + t.tv_usec);
}

char *RandStr(int len)
{
    static char str[512];
    if (len >= sizeof (str))
    {
        len = 511;
    }
    
    memset (str, 0, sizeof (str));
    srand(Seed++);
    int i;
    for (i = 0; i < len; ++i)
    {
        switch ((rand() % 3))
        {
        case 1:
            str[i] = 'A' + rand() % 26;
            break;
        case 2:
            str[i] = 'a' + rand() % 26;
            break;
        default:
            str[i] = '0' + rand() % 10;
            break;
        }
    }
    str[++i] = '\0';

    if (strlen (str) <= 3)
    {
        strcpy (str, "abcxyz");
    }
    return str;
}


void GenetateWmPattern (DWORD Num)
{
    DWORD Pid = 0;
    wmPattern.clear ();

    DWORD unit = Num/5;
    while (unit > 0)
    {
        string Base = string (RandStr (16));
        //cout<<"Base"<<Base<<"\r\n";

        DWORD n = 0;
        while (n < 5)
        {
            string Suffix = string (RandStr (rand ()%32));
            string Pattern = Base + Suffix;

            wmPattern[Pid++] = Pattern;
            //cout<<"Pattern"<<Pid-1<<Pattern<<"\r\n";
            n++;
        }

        unit--;
    }
}

void GenetateRegex (DWORD Num)
{
    DWORD Pid = 0;
    wmPattern.clear ();

    while (Num > 0)
    {
        string Base = string (RandStr (16));
        //cout<<"Base"<<Base<<"\r\n";

        string Regex = Base + ".*" + string (RandStr (rand ()%16));

        wmPattern[Pid++] = Regex;
        //cout<<Regex<<endl;

        Num--;
    }
}



char *GenerateContext (DWORD *Length)
{
    DWORD PLen = 0;
    static string Context = "";

    Context = "";
    DWORD PNum = wmPattern.size ();
    for (DWORD pi = 0; pi < PNum; pi++)
    {
        string Noise = to_string (pi) + "aaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaabaa789877888877aaafaaa" +
                       "cccccccccccccbddddddddddddddddddddddvvvvvvvvvvvvvvvvvvvffffffffffff909909090909ffffss" +
                       "gggggggggggggggggggggjjjjjjjjjjjjjjjjjjkkkkkkkkkkkkkkk5678tyy5y455kkktttttttttttttttt" +
                       "wwwwwwwwwwwwwwwwwwwwppppppppppppppppppppppppptt33edc4rfv5tgttttttttttttttttjjjjjjjjjj" +
                       "qqqqqqqqqqqqqqqqaaaaaaaaaaaaaaaaawwwwwwww5tgb6yhn7ujmwwwwwwrrrrrrrrrrrrrrddddddddxxxx" +
                       "eeeeeeeeeeeedddddddddddddddddddfffffffffffffffff123456yhn5tgfffkkkkkkkkkkkkkkkkkbbbbb" +
                       "333333333344444444444444666666666666666123456754edc6666666666688888888888888888888888" +
                       to_string (pi);
        
        for (DWORD i = 0; i < 15; i++)
        {
            Context += Noise;
            Context += wmPattern[pi];
            Context += Noise;
        }

        PLen += wmPattern[pi].length() * 15;
    }

    printf ("Match Rate = %f \r\n", PLen*1.0/Context.length());

    *Length = Context.length();
    return (char*)Context.c_str ();
}

void TestWmandEnWm (DWORD Pnum)
{
    GenetateWmPattern (Pnum);
    DWORD Length;
    char *Context = GenerateContext (&Length);

    long S = getTimeUsec ();
    DWORD Mem = GetPhyMemUse ()/1024;
    printf ("Start Wm, PatternSize = %u TxtLength = %u\r\n", (DWORD)wmPattern.size(), Length);
    Wm wm(&wmPattern);
    //T_WmResult* Wmr = wm.Search ((const BYTE* )Context, Length);
    //for (auto It = Wmr->begin (); It != Wmr->end (); It++)
    //{
    //    cout<<*It<<" ";
    //}
    printf ("\r\nWM time cost = %u ms, MemoryUsage = %u MB\r\n", 
            (DWORD)(getTimeUsec ()-S)/1000, GetPhyMemUse ()/1024 - Mem);

    
    S = getTimeUsec ();
    Mem = GetPhyMemUse ()/1024;
    printf ("Start EnHancedWm, PatternSize = %u TxtLength = %u\r\n", (DWORD)wmPattern.size(), Length);
    EnHancedWm EnWm (&wmPattern);
    T_Result* ENWmr = EnWm.Search ((const BYTE* )Context, Length);
    //for (auto It = ENWmr->begin (); It != ENWmr->end (); It++)
    //{
    //    M_Result *MR = &(*It);
    //    cout<<MR->ID<<" ";
    //}
    printf ("\r\nEnHancedWm time cost = %u ms, MemoryUsage = %u MB\r\n", 
           (DWORD)(getTimeUsec ()-S)/1000, GetPhyMemUse ()/1024 - Mem);
}

void TestEnWmRegex (DWORD Pnum)
{
    GenetateRegex (Pnum);
    DWORD Length;
    char *Context = GenerateContext (&Length);

    long S = getTimeUsec ();
    DWORD Mem = GetPhyMemUse ()/1024;

    printf ("Start compile EnWm ...\r\n");
    EnHancedWm EnWm (&wmPattern);
    printf ("\r\nCompile EnWm Time = %u, Pattern Size = %u, Memory Usage = %u MB\r\n", 
            (DWORD)(getTimeUsec ()-S)/1000, Pnum, GetPhyMemUse ()/1024-Mem);

    //////////////////////////////////////////////////////////
    // regex
    string Regex = "";
    for (auto It = wmPattern.begin (); It != wmPattern.end(); It++)
    {
        Regex += It->second + "|";
    }
    
    printf ("Start compile regex ...\r\n");
    S = getTimeUsec ();
    Mem = GetPhyMemUse ()/1024;

    regex_t reg;
    regcomp(&reg, Regex.c_str(), REG_EXTENDED|REG_NOSUB);
    printf ("\r\nCompile Regex Time = %u, Pattern Size = %u, Memory Usage = %u MB\r\n", 
                (DWORD)(getTimeUsec ()-S)/1000, Pnum, GetPhyMemUse ()/1024-Mem);
 
}



int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf ("./EnWm t n\r\n");
        return 0;
    }

    DWORD Type = atoi (argv[1]);
    
    DWORD PatternNum = atoi (argv[2]);

    if (Type == 0)
    {
        TestWmandEnWm (PatternNum);
    }
    else
    {
        TestEnWmRegex (PatternNum);
    }
	
	return 0;
}


