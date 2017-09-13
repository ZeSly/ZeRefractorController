#ifndef RESPONSES_H
#define RESPONSES_H

#include "zerefractorcontroller.h"

class Response
{
protected:
    ZeRefractorController *Zrc;
    char *String;

public:
    Response(ZeRefractorController *Zrc_L);
    virtual void Func(char *Responses);
};

class Response_fpr : Response
{
    Response_fpr(ZeRefractorController *Zrc_L);
    void Func(char *Responses);
};


#endif // RESPONSES_H
