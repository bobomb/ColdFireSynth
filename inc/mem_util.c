#include<stdlib.h>
#include"psptypes.h"
void* USB_mem_alloc_zero(uint_32 size);
void* USB_mem_alloc_zero(uint_32 size)
{
    void* p;
    p = malloc(size);
    if(p != NULL)
    {
        memset( (p) , (0) , (size));
    }
    return p;
}