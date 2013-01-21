/**
 * RuneIO - A tiny library for performing common binary file operations
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "runeio.h"

#ifndef NDEBUG
#include <assert.h>
int main(void) {
	// Test runeio functions
	FILE *debugfile = fSafeOpen("debug.test.file", "wb");
	char *debugdata = "This is a test file \n\n\n\t\t\t!!!!!!!!\n\n\t test::";
	fwrite(debugdata, 1, strlen(debugdata) + 1, debugfile);
	fflush(debugfile);

	freopen("debug.test.file", "rb", debugfile);

	char *debugdatacopy = malloc(1000);
	fSafeRead(debugdatacopy, 1, strlen(debugdata) + 1, debugfile);

	for (int i = 0; i < strlen(debugdata); i++) {
		assert(debugdata[i] == debugdatacopy[i]);
	}

	rewind(debugfile);

	fSafeRReadNTS(debugdatacopy, strlen(debugdata) + 1, debugfile, 0);
	fSafeReadNTS(debugdatacopy, 8, debugfile);

	assert(strlen(debugdatacopy) == 7);
	assert(strcmp(debugdata, debugdatacopy));
	assert(!strncmp(debugdata, debugdatacopy, 7));

	fclose(debugfile);
	remove("debug.test.file");
	fopen("debug.test.file", "wb+");

	debugdata = "Hey!\n\0 This is text that won't show up writing a string to a file.";

	fSafeWriteNTS(debugdata, debugfile);
	fSafeWriteNTS(debugdata + 6, debugfile);
	rewind(debugfile);
	fSafeRead(debugdatacopy, 6, 1, debugfile);
	assert(!strcmp("Hey!\n", debugdatacopy));
	fSafeReadNTS(debugdatacopy, 300, debugfile);
	assert(!strcmp(" This is text that won't show up writing a string to a file.", debugdatacopy));

	rewind(debugfile);
	for (int g = 0; g < 5; g++) {
		fSafeWrite(debugdata, strlen(debugdata) + 1, 1, debugfile);
	}

	debugdata = "C:\\Program Files (x86)\\Users\\default\\test.exe";
	debugdatacopy = realloc(debugdatacopy, 1000);

	baseNameFromURI(debugdata, 300l, debugdatacopy);
	assert(!strcmp("test.exe", debugdatacopy));

	free(debugdatacopy);

	printf("Tests passed!\n");

	return 0;
}
#endif

// -- MISC FUNCTIONS -- //

/**
 * Opens a file safely, stopping execution upon failure.
 * Will return null pointer if you have opened too many files already.
 *
 * @param filename - The name of the file to be opened
 * @param mode - The file access mode to be used (see fopen)
 * @return FILE* - a pointer to the opened file
 */
FILE* fSafeOpen(const char *filename, char *mode) {
	FILE *file;
	if (strlen(filename) > FILENAME_MAX) {
		printf("The file name you provided was too long!\nFile Name: %s has too many characters! (max is %i)\n",
				filename, FILENAME_MAX);
		exit(1);
	}

	if ((file = fopen(filename, mode)) == NULL) {
		fputs("Could not open file! Are you sure the file exists?\n", stderr);
		exit(1);
	}

	return file;
}

/**
 * Returns to size of the file in bytes
 *
 * @param stream - A pointer to a FILE object to read from.
 * @return long - The size of the file in bytes
 */
long filesize(FILE *stream) {
	long pos = ftell(stream);
	fseek(stream, 0, SEEK_END);
	long endpos = ftell(stream);
	fseek(stream, pos, SEEK_SET);
	return endpos;
}

// -- READING FUNCTIONS -- //

/**
 * Reads data safely, stopping execution upon failure or unexpected EOF
 *
 * @param data - A pointer to preallocated memory to store the data
 * @param size - The size in bytes of each element to read
 * @param count - The number of elements to read
 * @param stream - A pointer to a FILE object to read from
 */
void fSafeRead(void *data, size_t size, size_t count, FILE *stream) {
	if (data == NULL) {
		printf("fSafeRead was given NULL data pointer! Aborting.\n");
		exit(1);
	}

	if (stream == NULL) {
		printf("fSafeRead was given NULL file pointer! Did you forget to use fSafeOpen? Aborting.\n");
		exit(1);
	}

	long pos = ftell(stream);

	if ((filesize(stream) - pos - (size * count)) < 0) {
		printf("You cannot read past the end of the file! Tried to read %i bytes out of remaining %li!\n",
				(size * count), (filesize(stream) - pos));
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
void fSafeReadNTS(void *data, long max, FILE *stream) {
	char *buffer;
	long i = 0;
	int8_t finished = 0;

	if (data == NULL) {
		printf("fSafeReadNTS was given NULL data pointer! Aborting.\n");
		exit(1);
	}

	if (stream == NULL) {
		printf("fSafeReadNTS was given NULL file stream. Did you forget to use fSafeOpen?\n");
		exit(1);
	}

	buffer = malloc(max - 1);

	do {
		fread(buffer + i, 1, 1, stream);

		if (feof(stream)) {
			printf("Unexpected end of file while reading null terminated string!\n");
			exit(1);
		}

		if (*(buffer + i) == '\0') {
			strcpy(data, buffer);
			finished = 1;
			break;
		}

		i++;
	} while (i < (max - 1));

	if (!finished) {
		*(buffer + i + 1) = '\0';
		strcpy(data, buffer);
	}

	while (!finished) {
		fread(buffer, 1, 1, stream);

		if (feof(stream)) {
			printf("Unexpected end of file while reading null terminated string!\n");
			exit(1);
		}

		if (*buffer == '\0')
			finished = 1;
	}

	free(buffer);
}

/**
 * Safely reads a null terminated string from a file using fSafeReadNTS, then uses fseek to move to the offset specified
 *
 * @param data - A pointer to the location in memory to store the read string
 * @param max (long) - The space available for the string (including null at end)
 * @param stream (FILE*) - A pointer to a FILE object to read from
 * @param offset (long) - The offset in the file to move to when finished reading
 */
void fSafeRReadNTS(void *data, long max, FILE *stream, long offset) {
	fSafeReadNTS(data, max, stream);
	fseek(stream, offset, SEEK_SET);
}

/**
 * Safely reads a file using fSafeRead, then uses fseek to move the file pointer to the offset specified
 *
 * @param data - Preallocated memory to hold the data read
 * @param size (size_t) - The size of each element to read
 * @param count (size_t) - The number of elements to read
 * @param stream (FILE*) - A pointer to a FILE object to read from
 * @param offset (long) - The offset in the file to move to when finished reading
 */
void fSafeRRead(void *data, size_t size, size_t count, FILE *stream, long offset) {
	fSafeRead(data, size, count, stream);
	fseek(stream, offset, SEEK_SET);
}

// -- WRITING FUNCTIONS -- //

/**
 * Writes a null terminated string to a file safely
 *
 * @param string (const char*) - The null terminated string to write to the file
 * @param stream (FILE*) - A pointer to a FILE object to write to
 */
void fSafeWriteNTS(const char *string, FILE *stream) {
	if (stream == NULL) {
		printf("fSafeWriteNTS was given NULL file stream. Did you forget to use fSafeOpen?\n");
		exit(1);
	}

	if (string == NULL) {
		printf("fSafeWriteNTS was given NULL pointer for string. Did you forget to initialize your char*?\n");
		exit(1);
	}

	int i = -1;
	do {
		i++;
		if (fwrite(string + i, 1, 1, stream) != 1) {
			printf("Error writing data during fSafeWriteNTS, exiting!\n");
			exit(1);
		}
	} while (string[i] != '\0');
}

/**
 * Safely writes data to a file
 *
 * @param data - A pointer to the block of data you wish to write from
 * @param size (size_t) - The size of each element to write in bytes
 * @param count (size_t) - The number of elements to write
 * @param stream (FILE*) - A pointer to a FILE object to write to
 */
void fSafeWrite(void *data, size_t size, size_t count, FILE *stream) {
	if (data == NULL) {
		printf("fSafeWrite was given a null pointer for data. Aborting.\n");
		exit(1);
	}

	if (stream == NULL) {
		printf("fSafeWrite was given a null FILE pointer. Did you remember to use fSafeOpen?\n");
		exit(1);
	}

	if (fwrite(data, size, count, stream) != count) {
		printf("Error writing file data within fSafeWrite. Aborting!\n");
		exit(1);
	}
}

/**
 * Takes any string of a URI/URL and puts only the last portion (removing any directory info) into the provided char*
 *
 * @param uri (const char*) - The URI/URL to parse
 * @param max (long) - The space available to store the resultant string. This should be at least as long as the input string
 * @param destination (char*) - The place to put the resultant string.
 */
void baseNameFromURI(const char *uri, long max, char *destination) {
	if (uri == NULL) {
		printf("baseNameFromURI received null pointer for input string! Aborting.\n");
		exit(1);
	}

	if (destination == NULL) {
		printf("baseNameFromURI received null pointer for destination! Aborting.\n");
		exit(1);
	}

	if (strrchr(uri, '\\') == strrchr(uri, '/')) {
		// There aren't any slashes
		strcpy(destination, uri);
	} else if (strrchr(uri, '\\') > strrchr(uri, '/')) {
		strcpy(destination, strrchr(uri, '\\') + 1);
	} else {
		strcpy(destination, strrchr(uri, '/') + 1);
	}
}







