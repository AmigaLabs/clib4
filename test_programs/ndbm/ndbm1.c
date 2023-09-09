#include <fcntl.h>              /* UNIX file ctrl  UNIX  */
#include <ndbm.h>               /* ndbm header     BSD   */
#include <string.h>             /* Strings         ISOC  */
#include <stdlib.h>             /* Standard Lib    ISOC  */
#include <stdio.h>              /* I/O lib         ISOC  */
#include <unistd.h>             /* UNIX std stf    POSIX */
#include <errno.h>              /* error stf       POSIX */

#define NUMELE 4

int main(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    int i, dbRet;
    DBM *myDB;
    datum daKey, daVal;
    char *keys[NUMELE] = {"foo", "bar", "foobar", "foo"};
    char *vals[NUMELE] = {"FOO", "BAR", "FOOBAR", "FOOZAM"};

    /* Open the database (create) */
    myDB = dbm_open("ndbmTest", O_RDWR | O_CREAT, 0660);
    if (myDB == NULL) {
        printf("ERROR: Could not open the DB file.  Error number: %d.\n", errno);
        exit(1);
    } else {
        printf("DB created.\n");
        printf("DB opened.\n");
    }

    /* Put stuff in the DB... */
    for (i = 0; i < NUMELE; i++) {
        daKey.dptr = keys[i];
        daKey.dsize = strlen(keys[i]) + 1;
        daVal.dptr = vals[i];
        daVal.dsize = strlen(vals[i]) + 1;
        dbRet = dbm_store(myDB, daKey, daVal, DBM_INSERT);  // DBM_REPLACE for replace behaviour
        switch (dbRet) {
            case 0:
                printf("Store: '%s' ==> '%s'\n", (char *) daKey.dptr, (char *) daVal.dptr);
                break;
            case 1: // Only can happen with DBM_INSERT
                printf("Could not store '%s' because it was already in the DB\n", (char *) daKey.dptr);
                break;
            default:
                printf("ERROR: Could not insert item (%s,%s). \n", (char *) daKey.dptr, (char *) daVal.dptr);
                exit(1);
        } /* end switch */
    } /* end for */


    /* Lookup the second one.. */
    daKey.dptr = keys[1];
    daKey.dsize = strlen(keys[1]) + 1;
    daVal = dbm_fetch(myDB, daKey);
    if (daVal.dptr == NULL) {
        printf("ERROR: Could not look up %s\n", (char *) daKey.dptr);
    } else {
        printf("Got record: '%s' ==> '%s'\n", (char *) daKey.dptr, (char *) daVal.dptr);
    }

    /* Delete an element */
    daKey.dptr = keys[1];
    daKey.dsize = strlen(keys[1]) + 1;
    if (dbm_delete(myDB, daKey) < 0) {
        printf("ERROR: Could not delete item with key %s\n", daKey.dptr);
    } else {
        printf("Deleted: '%s'\n", (char *) daKey.dptr);
    }/* end if */

    /* Close the DB (flush everything to the file) */
    dbm_close(myDB);
    printf("DB closed... Bye!\n");

    return 0;
} /* end func main */