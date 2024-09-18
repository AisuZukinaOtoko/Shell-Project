#include "Include.h"


#ifdef _WIN32
void logError() {
	char error_message[30] = "An error has occurred\n";
	printf("%s", error_message);
}
#else
void logError() {
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}
#endif //_WIN32

int isWhiteSpace(char chr) {
	return chr <= ' ';
}

void tokenize(char* string, size_t size) {
	if (string == NULL)
		return;

	for (size_t i = 0; i < size; i++) {
		string[i] = (isWhiteSpace(string[i])) ? DELIMETER : string[i];
	}
}

char nextToken(char** currentToken, char* end) {
	if (!*currentToken)
		return 0;

	char br = 0;
	while (((*currentToken < end) && !br) || (**currentToken == DELIMETER)) {
		if (**currentToken == DELIMETER)
			br = 1;
		(*currentToken)++;
	}
	return *currentToken < end;
}

int getNumTokens(char* begin, char* end) {
	if (!begin)
		return 0;

	if (*begin == '\0')
		return 0;

	int count = 1;
	char* temp = begin;
	while (nextToken(&temp, end)) {
		count++;
	}
	return count;
}

char* copyString(char* str) {
	if (str == NULL)
		return NULL;

	size_t stringSize = strlen(str) + 1; // extra null terminator character
	char* tmp = (char*)malloc(stringSize);

	if (tmp == NULL)
		return NULL;

	memcpy(tmp, str, stringSize);
	return tmp;
}

char* concatenate(char* str1, char* str2) {
	if (str1 == NULL || str2 == NULL)
		return NULL;

	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	char* result = malloc(len1 + len2 + 1);

	if (result == NULL)
		return NULL;

	strcpy(result, str1);
	strcat(result, str2);

	return result;
}

int stringEmpty(char* str){
	size_t length = strlen(str);
	for (int i = 0; i < length; i++){
		if (!isWhiteSpace(str[i]))
			return 0;
	}
	return 1;
}