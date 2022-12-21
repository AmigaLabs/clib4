#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SUCCESSFUL 0
#define FAIL 20

#define TMP_FILE "tmp.file"
#define DPRINTF(f,s,e)         \
{                              \
    ret = dprintf(f,"%s\n",s); \
    if((int)strlen(s)!=ret-1)  \
    {                          \
        ret = FAIL+e;          \
        goto ENDER;            \
    }                          \
}

char *testStr = "test stdout fd: 1";

int main()
{
    int ret=SUCCESSFUL;

    /* TEST 1: stdout */
    DPRINTF(1,testStr,1);

    /* TEST 2: stderr */
    DPRINTF(2,"test stderr fd: 2",2);

    /* TEST 3: arbitrary file */
    {
        int fd = open(TMP_FILE, O_WRONLY);
        if(fd>-1) /* should be at least 3! */
        {
            char fdStr[8], str[32] = "test "TMP_FILE" fd: ";
            itoa(fd,&fdStr[0],10);
            strncat(&str[0], &fdStr[0],8);
            DPRINTF(fd,str,3);
            ret = close(fd);
            if(ret)
            {
                ret = FAIL+4;
                goto ENDER;
            }
        }
        else
        {
            ret = FAIL+5;
        }
    }

    /* TEST 4: try this with a very large buffer; tests vdprintf.c's heap allocation conditional branch */
    {
        char buffer[520];
        for(int i=0; i<520; i++)
        {
            buffer[i]='0'+i%10;
        }
        DPRINTF(1,buffer,6);
    }
    
    /* TEST 5: try a slightly more complicated format */
    {
        if(29!=dprintf(2,"Here %c%c the value %f",'i','s',3.141592F))
        {
            ret = FAIL+7; goto ENDER;
        }
    }
ENDER:
    exit(ret);
}
