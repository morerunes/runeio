/**
 * The header file for runeio.c, the runic file io library.
 */

#ifndef RUNEIO_H
#define RUNEIO_H
#include <stdio.h>

FILE*		fSafeOpen		(char*, char*);
void		fSafeClose		(FILE*);

void		fSafeRead		(void*, size_t, size_t, FILE*);
void		fSafeRead8		(void*, FILE*);
void		fSafeRead16		(void*, FILE*);
void		fSafeRead32		(void*, FILE*);
void		fSafeRead64		(void*, FILE*);
void		fSafeReadNTS	(void*, FILE*);

void		fSafeWrite		(void*, size_t, size_t, FILE*);
void		fSafeWrite8		(void*, FILE*);
void		fSafeWrite16	(void*, FILE*);
void		fSafeWrite32	(void*, FILE*);
void		fSafeWrite64	(void*, FILE*);
void		fSafeWriteNTS	(void*, FILE*);

void		fSafeRRead		(void*, size_t, size_t, FILE*);
void		fSafeRRead8		(void*, FILE*);
void		fSafeRRead16	(void*, FILE*);
void		fSafeRRead32	(void*, FILE*);
void		fSafeRRead64	(void*, FILE*);
void		fSafeRReadNTS	(void*, FILE*);

void		fSafeRWrite		(void*, size_t, size_t, FILE*);
void		fSafeRWrite8	(void*, FILE*);
void		fSafeRWrite16	(void*, FILE*);
void		fSafeRWrite32	(void*, FILE*);
void		fSafeRWrite64	(void*, FILE*);
void		fSafeRWriteNTS	(void*, FILE*);

void		baseNameFromURI	(const char*, char*);

#endif
