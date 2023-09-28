/*
 On Linux:

 ppc-amigaos-g++ -mcrt=clib4 -o hellogt hellogt.cxx -athread=native
 xgettext --package-name hellogt --package-version 1.2 --default-domain hellogt --output hellogt.pot hellogt.cxx
 msginit --no-translator --locale es_MX --output-file hellogt_spanish.po --input hellogt.pot
 sed --in-place hellogt_spanish.po --expression='/"hello, world!"/,/#: / s/""/"hola mundo"/'
 mkdir --parents ./es_MX.utf8/LC_MESSAGES
 msgfmt --check --verbose --output-file ./es_MX.utf8/LC_MESSAGES/hellogt.mo hellogt_spanish.po

 On AmigaOS4:

 setenv LANGUAGE es_MX.utf8
 hellogt
*/

#include <libintl.h>
#include <locale.h>
#include <iostream>

int main (){
    setlocale(LC_ALL, "");
    bindtextdomain("hellogt", ".");
    textdomain( "hellogt");
    std::cout << gettext("hello, world!") << std::endl;
}