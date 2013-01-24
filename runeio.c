/**
 * RuneIO - A tiny library for performing common binary file operations
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "runeio.h"

#define NDEBUG

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

FILE* fSafeOpen(const char *filename, char *mode) {
	FILE *file;
	if (strlen(filename) > FILENAME_MAX) {
		printf("The file name you provided was too long!\nFile Name: %s has too many characters! (max is %i)\n",
				filename, FILENAME_MAX);
		exit(1);
	}

	if ((file = fopen(filename, mode)) == NULL ) {
		fputs("Could not open file! Are you sure the file exists?\n", stderr);
		exit(1);
	}

	return file;
}

long filesize(FILE *stream) {
	long pos = ftell(stream);
	fseek(stream, 0, SEEK_END);
	long endpos = ftell(stream);
	fseek(stream, pos, SEEK_SET);
	return endpos;
}

// -- READING FUNCTIONS -- //

void fSafeRead(void *data, size_t size, size_t count, FILE *stream) {
	if (data == NULL ) {
		printf("fSafeRead was given NULL data pointer! Aborting.\n");
		exit(1);
	}

	if (stream == NULL ) {
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

char fSafeReadNTS(void *data, long max, FILE *stream) {
	char *buffer;
	long i = 0;
	int8_t finished = 0;

	if (data == NULL ) {
		printf("fSafeReadNTS was given NULL data pointer! Aborting.\n");
		exit(1);
	}

	if (stream == NULL ) {
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

	char result = !finished;

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
	return result;
}

char fSafeRReadNTS(void *data, long max, FILE *stream, long offset) {
	char result = fSafeReadNTS(data, max, stream);
	fseek(stream, offset, SEEK_SET);
	return result;
}

void fSafeRRead(void *data, size_t size, size_t count, FILE *stream,
		long offset) {
	fSafeRead(data, size, count, stream);
	fseek(stream, offset, SEEK_SET);
}

// -- WRITING FUNCTIONS -- //

void fSafeWriteNTS(const char *string, FILE *stream) {
	if (stream == NULL ) {
		printf("fSafeWriteNTS was given NULL file stream. Did you forget to use fSafeOpen?\n");
		exit(1);
	}

	if (string == NULL ) {
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

void fSafeWrite(void *data, size_t size, size_t count, FILE *stream) {
	if (data == NULL ) {
		printf("fSafeWrite was given a null pointer for data. Aborting.\n");
		exit(1);
	}

	if (stream == NULL ) {
		printf("fSafeWrite was given a null FILE pointer. Did you remember to use fSafeOpen?\n");
		exit(1);
	}

	if (fwrite(data, size, count, stream) != count) {
		printf("Error writing file data within fSafeWrite. Aborting!\n");
		exit(1);
	}
}

void filenameFromURI(const char *uri, long max, char *destination) {
	if (uri == NULL ) {
		printf("baseNameFromURI received null pointer for input string! Aborting.\n");
		exit(1);
	}

	if (destination == NULL ) {
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

void baseNameFromFilename(const char *filename, char *destination) {
	char *lastDot = strrchr(filename, '.');

	if (filename == destination) {
		if (lastDot != NULL ) {
			*lastDot = '\0';
		} // else nothing needs to be done
	} else {
		if (lastDot == NULL ) {
			strcpy(destination, filename);
		} else {
			strncpy(destination, filename, lastDot - filename);
			destination[lastDot - filename + 1] = '\0';
		}
	}
}

long nullTermStringSize(FILE *file) {
	long origPos = ftell(file);
	char testChar = '0';

	while (testChar != '\0') {
		fSafeRead(&testChar, 1, 1, file);
	}

	origPos = (ftell(file) - origPos);
	fseek(file, origPos, SEEK_SET);

	return origPos;
}
