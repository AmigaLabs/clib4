#undef __USE_INLINE__

/*#define DEBUG*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <stdio.h>
//struct SearchCmdPathListMsg {
//    int32  splm_Size;
//    BPTR   splm_Lock;
//    STRPTR splm_Name;
//};
void doPathExpansion(BPTR pathLock, char *fileName, int bufferSize, char *buffer) {
    int32 success = IDOS->DevNameFromLock(pathLock, buffer, bufferSize, DN_FULLPATH);
    if(success) IDOS->AddPart(buffer, fileName, bufferSize);
    printf("After expansion : %s\n", buffer);
}
BOOL dirContains(BPTR dirLock, char *fileName) {
    struct ExamineData *ed = IDOS->ExamineObjectTags(EX_FileLockInput, dirLock, TAG_DONE);
    if(ed) { IDOS->Printf("Directory <%s> ...\n", ed->Name);
    IDOS->FreeDosObject(DOS_EXAMINEDATA, ed); }

    APTR context = IDOS->ObtainDirContextTags(EX_LockInput, dirLock,
//                                EX_DoCurrentDir,TRUE,
                               EX_DataFields, EXF_ALL,
                                TAG_END);
    struct ExamineData *edata;
    BOOL result = FALSE;
    if(context) {
        while((edata = IDOS->ExamineDir(context))) {
            // if(!__IUtility->Stricmp(edata->Name, fileName)) { result = TRUE; break; }
            if(EXD_IS_LINK(edata)) {
               IDOS->Printf("%s [link]\n",edata->Name);
               IDOS->Printf("   File is linked to <%s>\n", edata->Link);
                if(!__IUtility->Stricmp(edata->Name, fileName)) { result = TRUE; break; }
            } else if (EXD_IS_DIRECTORY(edata)) {
               IDOS->Printf("%s [directory]\n", edata->Name);
            } else if(EXD_IS_FILE(edata)) {
               IDOS->Printf("%s [file]\n", edata->Name);
                if(!__IUtility->Stricmp(edata->Name, fileName)) { result = TRUE; break; }
            }
        }

        if(IDOS->IoErr() != ERROR_NO_MORE_ENTRIES && result != TRUE) {
            IDOS->Printf("Directory scan error\n");
        }
    }
     else IDOS->Printf("Can't create context\n");

    IDOS->ReleaseDirContext(context);
    return result;
}
int32 __search_command_hook_function(struct Hook *hook, APTR reserved, struct SearchCmdPathListMsg *message) {
    char *buffer = (char *)hook->h_Data;
    if(dirContains(message->splm_Lock, message->splm_Name)) {
        IDOS->Printf("Found a match.\n");
        doPathExpansion(message->splm_Lock, message->splm_Name, 4096, buffer);
        return 1;
    }
    return 0;
}
int
__search_expand_command_path(char const **name_ptr, char *replacement_buffer, size_t replacement_buffer_size) {
    struct Hook h = {{NULL, NULL}, (HOOKFUNC) __search_command_hook_function, NULL, replacement_buffer};
    struct CommandLineInterface *cli = IDOS->Cli();
    if(!cli) return -1;
    int32 found = IDOS->SearchCmdPathListTags((struct PathNode *)BADDR(cli->cli_PathList), &h, *name_ptr, TAG_END);
    if(!found) return -1;
    //*name_ptr = replacement_buffer;
    return 0;
}