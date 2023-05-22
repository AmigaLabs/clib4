/*
 * $Id: stdio_iobhookentry.c,v 1.7 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int64_t
__iob_hook_entry(struct _clib2 *__clib2, struct iob *file_iob, struct file_action_message *fam) {
	struct fd *fd;
	int64_t result;

	assert(fam != NULL && file_iob != NULL);

	switch (fam->fam_Action) {
        case file_action_read:
        case file_action_write:
        case file_action_seek:
        case file_action_close:
            SHOWVALUE(file_iob->iob_Descriptor);

            assert(file_iob->iob_Descriptor >= 0 && file_iob->iob_Descriptor < __clib2->__num_fd);
            assert(__clib2->__fd[file_iob->iob_Descriptor] != NULL);
            assert(FLAG_IS_SET(__clib2->__fd[file_iob->iob_Descriptor]->fd_Flags, FDF_IN_USE));

            if (__clib2->__fd[file_iob->iob_Descriptor] == NULL) {
                fam->fam_Error = EBADF;
                result = EOF;
                break;
            }
            /* When closing, we want to affect this very file descriptor
                   and not the original one associated with an alias of it. */
            if (fam->fam_Action == file_action_close)
                fd = __get_file_descriptor_dont_resolve(file_iob->iob_Descriptor);
            else
                fd = __get_file_descriptor(file_iob->iob_Descriptor);

            if (fd == NULL) {
                fam->fam_Error = EBADF;
                result = EOF;
                break;
            }

            assert(fd->fd_Action != NULL);

            result = (*fd->fd_Action)(__clib2, fd, fam);

            break;

        default:

            SHOWVALUE(fam->fam_Action);

            fam->fam_Error = EBADF;
            result = EOF;
            break;
	}

	RETURN(result);
	return (result);
}
