all:
	ppc-amigaos-gcc -mcrt=clib2 common.c res_query.c -o res_query -lresolv -lpthread
	ppc-amigaos-gcc -mcrt=clib2 common.c res_mkquery.c -o res_mkquery -lresolv -lpthread