/*#define DEBUG*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
// #include <stdio.h>
//struct SearchCmdPathListMsg {
//    int32  splm_Size;
//    BPTR   splm_Lock;
//    STRPTR splm_Name;
//};
void do_path_expansion(BPTR path_lock, char *file_name, int buffer_size, char *buffer) {
    int32 success = DevNameFromLock(path_lock, buffer, buffer_size, DN_FULLPATH);
    if(success) AddPart(buffer, file_name, buffer_size);
    // printf("After expansion : %s\n", buffer);
}
BOOL dir_contains(BPTR dir_lock, char *file_name) {
    // struct ExamineData *ed = ExamineObjectTags(EX_FileLockInput, dirLock, TAG_DONE);
    // if(ed) { Printf("Directory <%s> ...\n", ed->Name);
    // FreeDosObject(DOS_EXAMINEDATA, ed); }

    // struct Library *__utilitybase = OpenLibrary("utility.library", 0);
    // struct UtilityIFace *__iutility = (struct UtilityIFace *)GetInterface(__utilitybase, "main", 1, TAG_DONE);
    // if(!__iutility) return FALSE;
    DECLARE_UTILITYBASE();
    APTR context = ObtainDirContextTags(EX_LockInput, dir_lock,
//                                EX_DoCurrentDir,TRUE,
                               EX_DataFields, EXF_ALL,
                                TAG_END);
    struct ExamineData *edata;
    BOOL result = FALSE;
    if(context) {
        while((edata = ExamineDir(context))) {
            if(!Stricmp(edata->Name, file_name)) { result = TRUE; break; }
            // if(EXD_IS_LINK(edata)) {
            // //    IDOS->Printf("%s [link]\n",edata->Name);
            // //    IDOS->Printf("   File is linked to <%s>\n", edata->Link);
            //     if(!Stricmp(edata->Name, fileName)) { result = TRUE; break; }
            // } else if (EXD_IS_DIRECTORY(edata)) {
            // //    IDOS->Printf("%s [directory]\n", edata->Name);
            // } else if(EXD_IS_FILE(edata)) {
            // //    IDOS->Printf("%s [file]\n", edata->Name);
            //     if(!Stricmp(edata->Name, fileName)) { result = TRUE; break; }
            // }
        }

        if(IoErr() != ERROR_NO_MORE_ENTRIES && result != TRUE) {
            __set_errno(__translate_access_io_error_to_errno(IoErr()));
            /*Printf("Directory scan error\n");*/ result = FALSE;
        }
    }
     else {
                    __set_errno(__translate_access_io_error_to_errno(IoErr()));

/*Printf("Can't create context\n");*/ result = FALSE; }

    ReleaseDirContext(context);
    // DropInterface(__iutility);
    // CloseLibrary(__utilitybase);
    return result;
}
int32 __search_command_hook_function(struct Hook *hook, APTR reserved, struct SearchCmdPathListMsg *message) {
    char *buffer = (char *)hook->h_Data;
    if(dir_contains(message->splm_Lock, message->splm_Name)) {
        // Printf("Found a match.\n");
        do_path_expansion(message->splm_Lock, message->splm_Name, 4096, buffer);
        return 1;
    }
    return 0;
}
int
__search_expand_command_path(char const **name_ptr, char *replacement_buffer, size_t replacement_buffer_size) {
    struct Hook h = {{NULL, NULL}, (HOOKFUNC) __search_command_hook_function, NULL, replacement_buffer};
    struct CommandLineInterface *cli = Cli();
    if(!cli) return -1;
    int32 found = SearchCmdPathListTags((struct PathNode *)BADDR(cli->cli_PathList), &h, *name_ptr, TAG_END);
    if(!found) return -1;
    //*name_ptr = replacement_buffer;
    return 0;
}