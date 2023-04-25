#ifndef _COMMON_H
#define _COMMON_H

typedef struct
{
  unsigned int numElmt;
  union mydata{
    float dElmt;
    unsigned int iElmt;
  } a[10];
} MyMsg_t;

#endif
