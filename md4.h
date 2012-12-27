
#ifndef __MD4_H__
#define __MD4_H__

unsigned Com_BlockChecksum (void *buffer, int length);
void Com_BlockFullChecksum (void *buffer, int len, unsigned char *outbuf);

#endif