/**
 * RuneIO - A tiny library for performing common binary file operations
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "runeio.h"

static long filesize[FOPEN_MAX];
static long numOpenFiles = 0;

/**
 * Opens a file safely, stopping execution upon failure.
 * Will return null pointer if you have opened too many files already.
 *
 * @param filename - The name of the file to be opened
 * @param mode - The file access mode to be used (see fopen)
 * @return FILE* - a pointer to the opened file
 */
FILE* fSafeOpen(char* filename, char* mode) {
	FILE *file;
	if (strlen(filename) > FILENAME_MAX) {
		printf("The file name you provided was too long!\nFile Name: %s has too many characters! (max is %i)\n",
				filename, FILENAME_MAX);
		exit(1);
	}

	if ((file = fopen(filename, mode)) == NULL) {
		fputs("Could not open file! Are you sure the file exists?\n", stderr);
		exit(1);
	} else {
		numOpenFiles++;
		if (numOpenFiles > FOPEN_MAX) {
			fputs("You cannot open that many files at once! Max \n", stderr);
			numOpenFiles--;
			return 0;
		} else {

		}
		return file;
	}
}

/**
 * Closes a file, resetting internal settings to be ready for another file.
 */

/**
 * Reads data safely, stopping execution upon failure or unexpected EOF
 *
 * @param data - A pointer to preallocated memory to store the data
 * @param size - The size in bytes of each element to read
 * @param count - The number of elements to read
 * @param stream - A pointer to a FILE object to read from
 */

void fSafeRead(void* data, size_t size, size_t count, FILE* stream) {
	long pos = ftell(stream);

	if ((filesize - pos - (size * count)) < 0) {
		printf("You cannot read past the end of the file! Tried to read %i bytes out of remaining %i!\n",
				(size * count), (filesize - pos));
		exit(1);
	} else {
		fseek(stream, pos, SEEK_SET);

		if (fread(data, size, count, stream) != count) {
			printf("Attempt to read %i elements from file failed!\n", count);
			exit(1);
		}

		if (feof(stream)) {
			fputs("Unexpected end of file encountered! Terminating.\n", stderr);
			exit(1);
		}
	}
}

/**
 * Reads a single byte using fSafeRead
 *
 * @param data - Preallocated memory to store the read value
 */
void fSafeRead8(void* data, FILE* stream) {

}










