/* setlocale example */
#include <stdio.h>  /* printf */
#include <time.h>   /* time_t, struct tm, time, localtime, strftime */
#include <locale.h> /* struct lconv, setlocale, localeconv */

int main()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    struct lconv *lc;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    int twice = 0;

    do
    {
        printf("Locale is: %s\n", setlocale(LC_ALL, NULL));

        strftime(buffer, 80, "%c", timeinfo);
        printf("Date is: %s\n", buffer);

        lc = localeconv();
        printf("Currency symbol is: %s\n-\n", lc->currency_symbol);

        setlocale(LC_ALL, "");
    } while (!twice++);

    return 0;
}
