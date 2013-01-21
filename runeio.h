/**
 * @file runeio.h
 * @brief RuneIO - A small, safe file IO library.
 *
 * RuneIO is a small library for performing common file IO operations safely.
 * @author morerunes
 * @date January 2013
 */

#ifndef RUNEIO_H
#define RUNEIO_H
#include <stdio.h>

/**
 * Opens a file safely, stopping execution upon failure.
 * Will return null pointer if you have opened too many files already.
 *
 * @param filename - The name of the file to be opened
 * @param mode - The file access mode to be used
 * @see fopen
 * @return FILE* - a pointer to the opened file
 */
FILE*		fSafeOpen		(const char*, char*);

/**
 * Reads data safely, stopping execution upon failure or unexpected EOF
 *
 * @param data - A pointer to preallocated memory to store the data
 * @param size - The size in bytes of each element to read
 * @param count - The number of elements to read
 * @param stream - A pointer to a FILE object to read from
 */
void		fSafeRead		(void*, size_t, size_t, FILE*);

/**
 * Reads a null-terminated string from a binary file
 *
 * If the string in the file is longer than (max - 1) it will continue to read
 * until a null is reached, but will not place any more characters into the
 * given data pointer. Will ensure the string is null terminated at (data).
 *
 * @param data - A pointer to the location location to store the read string
 * @param max (long) - The number of bytes available at (data).
 * @param stream (FILE*) - A pointer to a FILE object to read from.
 */
void		fSafeReadNTS	(void*, long, FILE*);

/**
 * Safely writes data to a file
 *
 * @param data - A pointer to the block of data you wish to write from
 * @param size (size_t) - The size of each element to write in bytes
 * @param count (size_t) - The number of elements to write
 * @param stream (FILE*) - A pointer to a FILE object to write to
 */
void		fSafeWrite		(void*, size_t, size_t, FILE*);

/**
 * Writes a null terminated string to a file safely
 *
 * @param string (const char*) - The null terminated string to write to the file
 * @param stream (FILE*) - A pointer to a FILE object to write to
 */
void		fSafeWriteNTS	(const char*, FILE*);

/**
 * Safely reads a file using fSafeRead, then uses fseek to move the file pointer to the offset specified
 *
 * @param data - Preallocated memory to hold the data read
 * @param size (size_t) - The size of each element to read
 * @param count (size_t) - The number of elements to read
 * @param stream (FILE*) - A pointer to a FILE object to read from
 * @param offset (long) - The offset in the file to move to when finished reading
 */
void		fSafeRRead		(void*, size_t, size_t, FILE*, long offset);

/**
 * Safely reads a null terminated string from a file using fSafeReadNTS, then uses fseek to move to the offset specified
 *
 * @param data - A pointer to the location in memory to store the read string
 * @param max (long) - The space available for the string (including null at end)
 * @param stream (FILE*) - A pointer to a FILE object to read from
 * @param offset (long) - The offset in the file to move to when finished reading
 */
void		fSafeRReadNTS	(void*, long max, FILE*, long offset);

/**
 * Takes any string of a URI/URL and puts only the last portion (removing any directory info) into the provided char*
 *
 * @param uri (const char*) - The URI/URL to parse
 * @param max (long) - The space available to store the resultant string. This should be at least as long as the input string
 * @param destination (char*) - The place to put the resultant string.
 */
void		baseNameFromURI	(const char*, long max, char*);

/**
 * Returns to size of the file in bytes
 *
 * @param stream - A pointer to a FILE object to read from.
 * @return long - The size of the file in bytes
 */
long		filesize	(FILE*);

#endif
