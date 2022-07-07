#ifndef __UI_H_
#define __UI_H_

#include "main.h"

typedef struct
{
    void (*Init)(void);
    void (*UpdateTask)(void);
    void (*Reflash)(void);
} UI_Type, *pUI_Type;

extern pUI_Type gpUI;

#endif
