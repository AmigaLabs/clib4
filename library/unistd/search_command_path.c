/*
 * $Id: unistd_search_command_path.c,v 1.0 2024-06-30 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

void
do_path_expansion(BPTR path_lock, char *file_name, int buffer_size, char *buffer) {
    int32 success = DevNameFromLock(path_lock, buffer, buffer_size, DN_FULLPATH);
    if (success) AddPart(buffer, file_name, buffer_size);

    SHOWMSG("Result of path expansion :");
    SHOWSTRING(buffer);
}

BOOL
dir_contains(BPTR dir_lock, char *file_name) {

    DECLARE_UTILITYBASE();

    APTR context = ObtainDirContextTags(EX_LockInput, dir_lock, TAG_END);
    struct ExamineData *edata;
    BOOL result = FALSE;

    if (context) {
        while ((edata = ExamineDir(context))) {
            if (!Stricmp(edata->Name, file_name)) {
                result = TRUE;
                break;
            }
        }

        if (IoErr() != ERROR_NO_MORE_ENTRIES && result != TRUE) {
            __set_errno(__translate_access_io_error_to_errno(IoErr()));

            SHOWMSG("Directory scan error\n");
            result = FALSE;
        }
    } else {
        __set_errno(__translate_access_io_error_to_errno(IoErr()));

        SHOWMSG ("Can't create context\n");
        result = FALSE;
    }

    ReleaseDirContext(context);
    return result;
}

int32
__search_command_hook_function(struct Hook *hook, APTR reserved, struct SearchCmdPathListMsg *message) {
    char *buffer = (char *) hook->h_Data;

    if (dir_contains(message->splm_Lock, message->splm_Name)) {
        SHOWMSG("Found a match.\n");

        do_path_expansion(message->splm_Lock, message->splm_Name, 4096, buffer);
        return 1;
    }

    return 0;
}

int
__search_expand_command_path(char const **name_ptr, char *replacement_buffer, size_t replacement_buffer_size) {
    ENTER();

    assert(name_ptr);
    assert(replacement_buffer);

    SHOWMSG("Attempting to path expand command name");
    SHOWSTRING(*name_ptr);

    struct Hook h = {{NULL, NULL}, (HOOKFUNC) __search_command_hook_function, NULL, replacement_buffer};
    struct CommandLineInterface *cli = Cli();

    if (!cli) {
        SHOWMSG("Failed to refer CLI structure");

        return -1;
    }

    int32 found = SearchCmdPathListTags((struct PathNode *) BADDR(cli->cli_PathList), &h, *name_ptr, TAG_END);

    if (!found) {
        SHOWMSG("No such file was found on the system path");

        return -1;
    }

    RETURN(0);
    return 0;
}