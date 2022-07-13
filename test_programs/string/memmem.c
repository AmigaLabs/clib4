#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

int main() {
    /* http://www.randomtext.me/#/lorem/p-5/20-35 */
    const char *haystack =
            "Lorem ipsum ante nostra cras pretium suscipit dictumst curabitur aptent justo pretium molestie cubilia mi cubilia neque risus nisl tortor convallis iaculis, eros vivamus suscipit elit elementum conubia. "
            "Nullam eros vehicula aliquam ad fringilla euismod ut semper malesuada a, class elit condimentum tempus faucibus nibh sodales leo tellus, diam malesuada tincidunt sodales rhoncus ornare rutrum habitasse nisl fermentum sed massa mauris duis rutrum. "
            "Torquent mollis inceptos mollis accumsan nisl consequat lorem tincidunt, at donec curabitur duis etiam consectetur id vel, class ultricies platea arcu venenatis luctus diam. "
            "Dictumst velit viverra maecenas ante diam quis tortor nostra per at libero, turpis mollis diam varius scelerisque faucibus donec consectetur pulvinar lobortis non dapibus, mauris dictum faucibus nulla mi rhoncus quis feugiat et mi. "
            "Odio egestas potenti curabitur massa lorem nulla hac justo posuere, urna mauris aliquam primis quisque aliquam conubia fringilla, habitasse vitae euismod tempus curae vestibulum nisl lectus. ";

    char needle[1000];
    const char *found;
    size_t needlelen;

    printf("Haystack:\n%s\n", haystack);

    printf("\nEnter a needle: ");
    fflush(stdout);
    do fgets(needle, 1000, stdin);
    while (needle[strlen(needle) - 1] != '\n');
    needlelen = strlen(needle) - 1;
    needle[needlelen] = '\0';

    found = memmem(haystack, strlen(haystack), needle, needlelen);

    if (found)
        printf("Found at position %u\n", (unsigned char *) found - (unsigned char *) haystack);
    else
        puts("Not found.");

    return 0;
}