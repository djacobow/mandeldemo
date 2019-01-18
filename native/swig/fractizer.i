%module fractizer
%{
#include "fractizer.h"
%}

%include "fractizer.h"

%typemap(out) unsigned short [ANY] {
    int i = 0, len = 0;
    len = $1_dim0;
    for (i=0;i<len;i++) {
        SV *pv = newSV(0);
        sv_setuv(pv, (UV)$1[i]);
        av_push(av,pv);
    }
    $result = newRV_noinc((SV *)av);
    sv_2mortal($result);
    argvi++;
}

%inline %{
    unsigned short *make_obuf(size_t s) {
        unsigned short *p = malloc(sizeof(unsigned short) * s);
        return p; 
    }
    void free_obuf(unsigned short *p) {
        if (p) free(p);
    }
    SV *bufToArray(unsigned short *buf, size_t len) {
        AV *av = newAV();
        for (size_t i=0; i<len; i++) {
            av_push(av, newSVuv(buf[i]));
        }
        return newRV_noinc((SV*)av);
    }
%}
