#include "Include.h"

void initShell() {
	initPath();
}

void cmdCD(char* args) {
	int result = chdir(args);

	if (result != 0)
		logError();
}

void cmdPath(char* begin, char* end) {
	char* curr = begin;
	int args = nextToken(&curr, end);

	if (!args) {
		clearPath();
		return;
	}

	do {
		addPath(curr);
	} while (nextToken(&curr, end));
}

void cmdExternal(char* begin, char* end) {
	if (isPathEmpty()) {
		logError();
		return;
	}
	
	char* filePath;
	char* path = NULL;
	char* cmdName = copyString(begin);
	int index = 0;
	while (getPath(&path, index)) {
		filePath = concatenate(path, cmdName);

		if (access(filePath, X_OK) == 0)
			break;

		free(filePath);
		index++;
	}

	if (path == NULL) { // command was not found
		logError();
		return;
	}

	pid_t pID = fork();

	if (pID < 0) {
		logError();
		return;
	}
	

	if (pID == 0) { // child process
		int args = nextToken(&begin, end);
		int argc = (args) ? getNumTokens(begin, end) : 0;

		char** argv = (char**)malloc(argc * sizeof(char*) + 2);

		for (int i = 1; i < argc + 1; i++) {
			argv[i] = copyString(begin);
			nextToken(&begin, end);
		}
		argv[0] = cmdName;
		argv[argc + 1] = NULL;

		execv(filePath, argv);
		logError();
		exit(1);
	}
	else { // Parent process
		int status;
		pID = waitpid(pID, &status, 0);
		//printf("This is the parent process. PID = %d, Child PID = %d\n", getpid(), pID);
	}

}

int runScript(char* cmd, size_t size) {
	tokenize(cmd, size);

	char* curr = cmd;
	int args = nextToken(&curr, &cmd[size]);
	int argCount = (args) ? getNumTokens(curr, &cmd[size]) : 0;

	if (strcmp(cmd, "exit") == 0) {
		if (args) {
			logError();
			return 1;
		}
		exit(0);
	}

	if (strcmp(cmd, "cd") == 0) {
		if (argCount != 1) {
			logError();
			return 1;
		}
		cmdCD(curr);
		return 0;
	}

	if (strcmp(cmd, "path") == 0) {
		cmdPath(cmd, &cmd[size]);
		return 0;
	}

	if (strcmp(cmd, "print") == 0) {
		printf("Printing Path List.\n");
		char* str;
		int index = 0;
		while (getPath(&str, index)) {
			printf("Path %d: %s\n", index, str);
			index++;
		}
		return 0;
	}
	cmdExternal(cmd, &cmd[size]);
	return 0;
}

int runInteractive() {
	printf("witsshell> ");
	char line[BUFFER_SIZE];
	while (fgets(line, sizeof(line), stdin) != NULL) {
		runScript(line, strlen(line));
		printf("witsshell> ");
	}
	return 0;
}


int runBatch(char* filepath) {
	FILE* file;
	char line_buffer[BUFFER_SIZE]; // Buffer to store each line

	file = fopen(filepath, "r");

	if (file == NULL) {
		logError();
		exit(1);
	}

	while (fgets(line_buffer, sizeof(line_buffer), file)) {
		if (runScript(line_buffer, strlen(line_buffer)))
			exit(1);
	}

	fclose(file);
	return 0;
}