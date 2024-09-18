#ifndef INCLUDE_H
#define INCLUDE_H

#define DELIMETER '\0'
#define BUFFER_SIZE 256
#define MAX_PARALLEL_CMD 60

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS // disable warnings
#else
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif // _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct storing an array of strings.
struct StringArray {
	char** paths;
	int pathCount;
	int listCapacity;
};

struct Command {
	char* cmd;
	int redirect;
	char* outFile;
};

/*
* @brief Initialises the shell.
*/
void initShell();

/*
* @brief Initialises the path list.
*/
void initPath();

/*
* @brief Executes one entire command.
* @param Command to run. Size/Length of command string.
* @return 0 on success. 1 if error occured.
*/
int runScript(char* script, size_t size);

/*
* @brief Runs the shell in batch mode.
* @param Path to batch file to run.
* @return 0 opn success. Exits if error occured.
*/
int runBatch(char* filepath);

/*
* @brief Runs the shell in interactive mode.
* @return 0 on success. 1 if error occured.
*/
int runInteractive();


/*
* @brief Clears all the paths in the path list.
*/
void clearPath();

/*
* @brief Adds exactly one path to the path list. 'path' command not to be included.
* @param Path to add
*/
void addPath(char* path);

/*
* @brief Gets the path at the specified index. Saves string in path parameter.
* @param Address of string pointer.
* @return 1 if index is valid. 0 otherwise
*/
int getPath(char** path, int index);

/*
* @brief Checks if path is empty.
* @return 1 if path list is empty. 0 otherwise.
*/
int isPathEmpty();

/*
* @brief Logs error message to standard error.
*/
void logError();

/*
* @brief Checks whether character is white space. i.e space, tabs and newline
* @param character to query
* @return 1 if character is white space. 0 if character is not white space
*/
int isWhiteSpace(char chr);

/*
* @brief Replaces white space with DELIMETER across entire string.
* @param String to tokenize. Size of string/Number of characters incl NULL terminator.
*/
void tokenize(char* string, size_t size);

/*
* @brief Finds next string in string array. Strings separated by DELIMETER. Sets currentToken to beginning of next string.
* @param Address of string pointer. Pointer to end of string array. Usually &charArray[charCount] ('\0').
* @return 0 if end of string array is reached. 1 if end has not been reached.
*/
char nextToken(char** currentToken, char* end);

/*
* @brief Finds the number of tokens separated by white space. String should be tokenized beforehand.
* @param Address of string beginning. Pointer to end of string array. Usually &charArray[charCount].
* @return The number of tokens in the string.
*/
int getNumTokens(char* begin, char* end);

/*
* @brief Copies a string
* @param String to be copied.
* @return The new string
*/
char* copyString(char* str);

/*
* @brief Concatenates two strings together.
* @param Strings to be concatenated.
* @return The new concatenated string
*/
char* concatenate(char* str1, char* str2);


/*
* @brief Checks if string only contains whitespace
* @param Strings to be checked.
* @return 1 if string is empty. 0 otherwise.
*/
int stringEmpty(char* str);

#endif // INCLUDE_H