#include "Include.h"

static struct Command cmdList[MAX_PARALLEL_CMD]; // commands to be executed in parallel
static int cmdCount = 0;

void emptyCmdList(){
	for (int i = 0; i < MAX_PARALLEL_CMD; i++){
		if (cmdList[i].cmd != NULL){
			free(cmdList[i].cmd);
			cmdList[i].cmd = NULL;
		}

		cmdList[i].redirect = 0;

		if (cmdList[i].outFile != NULL){
			free(cmdList[i].outFile);
			cmdList[i].outFile = NULL;
		}
	}
	cmdCount = 0;
}

void createCmdList(char* input){
	emptyCmdList();

	if (input == NULL) return;

	//if (input[0] == '&'){
		//logError();
		//exit(0);
	//}
	
    char *token = strtok(input, "&");
    while (token != NULL) {
        while (*token == ' ') token++;

		cmdList[cmdCount].cmd = copyString(token);
		cmdCount++;

        token = strtok(NULL, "&");
    }
}

// returns -1 if error. 0 if no output file. 1 if there is an output file.
int getOutputFile(char** output, char** cmdEnd, char* begin, char* end){
	if (output == NULL || begin == NULL || end == NULL || cmdEnd == NULL){
		return -1;
	}
	char* temp = begin;
	char* tempOutput = NULL;
	int redirectCount = 0;

	// check the number of redirect operators
	while ((temp < end)){
		if (*temp == '>'){
			redirectCount++;
			tempOutput = temp;
			*temp = '\0';
			*cmdEnd = temp;
		}
		temp++;
	}

	if (redirectCount == 0){
		return 0;
	}
		

	if (redirectCount > 1){
		return -1;
	}

	char args = nextToken(&tempOutput, end);
	if (!args){
		return -1;
	}
	
	int argc = getNumTokens(tempOutput, end);
	if (argc != 1){
		return -1;
	}

	*output = tempOutput;
	return 1;
}

void initShell() {
	initPath();
	emptyCmdList();
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

pid_t cmdExternal(char* begin, char* end) {
	if (isPathEmpty()) {
		logError();
		return -1;
	}
	
	char* filePath;
	char* path = NULL;
	char* cmdName = copyString(begin);
	int index = 0;
	while (getPath(&path, index)) { //search through paths.
		filePath = concatenate(path, cmdName);
		if (access(filePath, X_OK) == 0)
			break;
		free(filePath);
		index++;
	}

	if (path == NULL) { // command was not found
		logError();
		return -1;
	}

	// Open file for redirection
	char* outputFileName = NULL;
	char* cmdEnd = end;
	int redirectStatus = getOutputFile(&outputFileName, &cmdEnd, begin, end);
	end = cmdEnd;
	if (redirectStatus == -1){ // An error occured whilst trying to find the redirect file
		logError();
		return -1;
	}

	int file_fd = -1;
	if (redirectStatus == 1){ // need to redirect output
		file_fd = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (file_fd < 0) {// could not open file
			return -1;
		}
	}

	pid_t pID = fork();
	if (pID < 0) {
		logError();
		return -1;
	}
	

	if (pID == 0) { // child process
		// Redirect stdout
		if (redirectStatus == 1){
			if (dup2(file_fd, STDOUT_FILENO) < 0) {
				close(file_fd);
				return -1;
			}        
			close(file_fd);
		}

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
		if (redirectStatus == 1)
			close(file_fd);
	}
	return pID;
}

pid_t runScript(char* cmd, size_t size) {
	if (stringEmpty(cmd)){
		return -1;
	}
	tokenize(cmd, size);

	char* curr = cmd;
	int args = nextToken(&curr, &cmd[size]);
	int argCount = (args) ? getNumTokens(curr, &cmd[size]) : 0;

	if (strcmp(cmd, "exit") == 0) {
		if (args) {
			logError();
			return -1;
		}
		exit(0);
	}

	if (strcmp(cmd, "cd") == 0) {
		if (argCount != 1) {
			logError();
			return -1;
		}
		cmdCD(curr);
		return -1;
	}

	if (strcmp(cmd, "path") == 0) {
		cmdPath(cmd, &cmd[size]);
		return -1;
	}

	if (strcmp(cmd, "print") == 0) {
		printf("Printing Path List.\n");
		char* str;
		int index = 0;
		while (getPath(&str, index)) {
			printf("Path %d: %s\n", index, str);
			index++;
		}
		return -1;
	}
	pid_t pID = cmdExternal(cmd, &cmd[size]);
	return pID;
}

void runCommands(){
	pid_t proccessList[MAX_PARALLEL_CMD];

	// Run each command in parallel
	for (int i = 0; i < cmdCount; i++){
		proccessList[i] = runScript(cmdList[i].cmd, strlen(cmdList[i].cmd));
	}

	// Wait for each command to finish
	for (int i = 0; i < cmdCount; i++){
		if (proccessList[i] < 0){
			continue;
		}

		int status;
		proccessList[i] = waitpid(proccessList[i], &status, 0);
	}
}

int runInteractive() {
	printf("witsshell> ");
	char line[BUFFER_SIZE];
	while (fgets(line, sizeof(line), stdin) != NULL) {
		createCmdList(line);
		runCommands();
		//runScript(line, strlen(line));
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
		if (stringEmpty(line_buffer)){
			continue;
		}
		createCmdList(line_buffer);
		runCommands();
	}

	fclose(file);
	return 0;
}