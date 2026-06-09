/*
 * Test to verify fam_Data pointer assignment
 */

#include <stdio.h>
#include <string.h>

/* Simulate the struct */
struct file_action_message {
    int fam_Action;
    char *fam_Data;      /* This is char * */
    int fam_Size;
    long long fam_Offset;
};

int main(void) {
    char buffer[20];
    void *void_ptr = buffer;
    
    memset(buffer, 'A', sizeof(buffer));
    buffer[19] = '\0';
    
    struct file_action_message fam;
    
    printf("=== POINTER ASSIGNMENT TEST ===\n\n");
    
    printf("buffer address:    %p\n", buffer);
    printf("void_ptr address:  %p\n", void_ptr);
    printf("sizeof(void *):    %zu\n", sizeof(void *));
    printf("sizeof(char *):    %zu\n", sizeof(char *));
    printf("sizeof(int):       %zu\n", sizeof(int));
    printf("sizeof(long long): %zu\n\n", sizeof(long long));
    
    /* Simulate what read.c does */
    fam.fam_Data = void_ptr;  /* Assigning void * to char * */
    fam.fam_Size = 5;
    
    printf("After assignment:\n");
    printf("fam.fam_Data = %p\n", fam.fam_Data);
    printf("fam.fam_Size = %d\n\n", fam.fam_Size);
    
    /* Verify the pointer is correct */
    if (fam.fam_Data == buffer) {
        printf("+ Pointer assignment is CORRECT\n");
    } else {
        printf("x Pointer assignment is WRONG!\n");
        printf("  Expected: %p\n", buffer);
        printf("  Got:      %p\n", fam.fam_Data);
    }
    
    /* Try to write through the pointer */
    printf("\nWriting 'HELLO' through fam.fam_Data:\n");
    memcpy(fam.fam_Data, "HELLO", 5);
    
    printf("buffer content: '%s'\n", buffer);
    printf("Expected:       'HELLO'\n");
    
    if (memcmp(buffer, "HELLO", 5) == 0) {
        printf("+ Write through pointer WORKS\n");
    } else {
        printf("x Write through pointer FAILED\n");
    }
    
    return 0;
}
