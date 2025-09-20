#ifndef RAND_H
#define RAND_H

void set_seed(unsigned long long seed);
unsigned char rand_uchar(unsigned int max);
unsigned short rand_ushort(unsigned int max);
unsigned int rand_uint(unsigned int max);

#endif