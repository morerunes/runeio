/**
 * The header file for runeio.c, the runic file io library.
 */

#ifndef RUNEIO_H
#define RUNEIO_H
#include <stdio.h>

FILE*		fSafeOpen		(const char*, char*);

void		fSafeRead		(void*, size_t, size_t, FILE*);
void		fSafeReadNTS	(void*, long, FILE*);

void		fSafeWrite		(void*, size_t, size_t, FILE*);
void		fSafeWriteNTS	(const char*, FILE*);

void		fSafeRRead		(void*, size_t, size_t, FILE*, long offset);
void		fSafeRReadNTS	(void*, long max, FILE*, long offset);

void		baseNameFromURI	(const char*, long max, char*);
long		filesize	(FILE*);

#endif
