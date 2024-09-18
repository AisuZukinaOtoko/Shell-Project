#include "Include.h"

static struct StringArray PATH;

void initPath() {
	PATH.paths = NULL;
	PATH.pathCount = 0;
	PATH.listCapacity = 0;

	char path[] = "/bin/";
	addPath(path);
}

int isPathEmpty() {
	return !PATH.pathCount;
}

void clearPath() {
	for (int i = 0; i < PATH.pathCount; i++) {
		char* str = PATH.paths[i];
		free(str);
	}
	free(PATH.paths);
	PATH.paths = NULL;
	PATH.pathCount = 0;
	PATH.listCapacity = 0;
}

void reallocatePathList(int newCapacity) {
	char** tmp = (char**)realloc(PATH.paths, newCapacity * sizeof(char*));
	if (tmp == NULL)
		return;

	PATH.paths = tmp;
	PATH.listCapacity = newCapacity;
}

void addPath(char* path) {
	if (path == NULL)
		return;

	char subStr[] = "/";
	size_t size = strlen(path);
	if (path[size -1] != '/'){
		path = concatenate(path, subStr);
	}

	if (PATH.listCapacity == 0)
		reallocatePathList(1);

	if (PATH.listCapacity <= PATH.pathCount)
		reallocatePathList(PATH.listCapacity * 2);

	PATH.paths[PATH.pathCount] = copyString(path);
	PATH.pathCount++;
}

int getPath(char** path, int index) {
	if (path == NULL) {
		return 0;
	}

	if (index >= PATH.pathCount) {
		*path = NULL;
		return 0;
	}

	*path = PATH.paths[index];
	return 1;
}