#include <errno.h>
#include <linux/sysinfo.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

struct ASCIIArt {
	char **content;
	size_t lines;
};

size_t getFileSize(FILE *file);
size_t getFileNumberOfLines(FILE *file);
struct ASCIIArt *getASCIIArt();
char *getOSName();
char *getKernel();
char *getSysArch();
char *getUpTime();
char *getCPU();
char *getMaxMemory();
char *getFreeMemory();

void cleanup_resources(char *hostname,
					   char *user,
					   char *OSName,
					   char *kernelVersion,
					   char *sysArch,
					   char *cpu,
					   char *maxMemory,
					   char *freeMemory,
					   char *upTime,
					   struct ASCIIArt *ASCII) {
	if (hostname)
		free(hostname);
	if (user && user != "unknown")
		free(user);
	if (OSName)
		free(OSName);
	if (kernelVersion)
		free(kernelVersion);
	if (sysArch)
		free(sysArch);
	if (cpu)
		free(cpu);
	if (maxMemory)
		free(maxMemory);
	if (freeMemory)
		free(freeMemory);
	if (upTime)
		free(upTime);

	if (ASCII) {
		if (ASCII->content) {
			for (size_t i = 0; i < ASCII->lines; i++) {
				if (ASCII->content[i])
					free(ASCII->content[i]);
			}
			free(ASCII->content);
		}
		free(ASCII);
	}
}

int main(int argc, char **argv) {
	char *hostname = malloc(sizeof(char) * 256);
	if (!hostname) {
		fprintf(stderr, "Error: Failed to allocate memory for hostname\n");
		exit(EXIT_FAILURE);
	}

	if (gethostname(hostname, 256) != 0) {
		fprintf(stderr, "Error: Failed to get hostname: %s\n", strerror(errno));
		free(hostname);
		exit(EXIT_FAILURE);
	}

	char *user = getenv("USER");
	if (!user)
		user = "unknown";
	else
		user = strdup(user);

	if (!user) {
		fprintf(stderr, "Error: Failed to allocate memory for user\n");
		free(hostname);
		exit(EXIT_FAILURE);
	}

	char *OSName = getOSName();
	if (!OSName) {
		fprintf(stderr, "Error: Failed to get OS name\n");
		cleanup_resources(
			hostname, user, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		exit(EXIT_FAILURE);
	}

	char *kernelVersion = getKernel();
	if (!kernelVersion) {
		fprintf(stderr, "Error: Failed to get kernel version\n");
		cleanup_resources(
			hostname, user, OSName, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		exit(EXIT_FAILURE);
	}

	char *sysArch = getSysArch();
	if (!sysArch) {
		fprintf(stderr, "Error: Failed to get system architecture\n");
		cleanup_resources(hostname,
						  user,
						  OSName,
						  kernelVersion,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL);
		exit(EXIT_FAILURE);
	}

	char *cpu = getCPU();
	if (!cpu) {
		fprintf(stderr, "Error: Failed to get CPU information\n");
		cleanup_resources(hostname,
						  user,
						  OSName,
						  kernelVersion,
						  sysArch,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL);
		exit(EXIT_FAILURE);
	}

	char *maxMemory = getMaxMemory();
	if (!maxMemory) {
		fprintf(stderr, "Error: Failed to get total memory information\n");
		cleanup_resources(hostname,
						  user,
						  OSName,
						  kernelVersion,
						  sysArch,
						  cpu,
						  NULL,
						  NULL,
						  NULL,
						  NULL);
		exit(EXIT_FAILURE);
	}

	char *freeMemory = getFreeMemory();
	if (!freeMemory) {
		fprintf(stderr, "Error: Failed to get free memory information\n");
		cleanup_resources(hostname,
						  user,
						  OSName,
						  kernelVersion,
						  sysArch,
						  cpu,
						  maxMemory,
						  NULL,
						  NULL,
						  NULL);
		exit(EXIT_FAILURE);
	}

	char *upTime = getUpTime();
	if (!upTime) {
		fprintf(stderr, "Error: Failed to get uptime information\n");
		cleanup_resources(hostname,
						  user,
						  OSName,
						  kernelVersion,
						  sysArch,
						  cpu,
						  maxMemory,
						  freeMemory,
						  NULL,
						  NULL);
		exit(EXIT_FAILURE);
	}

	struct ASCIIArt *ASCII = getASCIIArt();
	if (!ASCII) {
		fprintf(stderr, "Error: Failed to load ASCII art\n");
		cleanup_resources(hostname,
						  user,
						  OSName,
						  kernelVersion,
						  sysArch,
						  cpu,
						  maxMemory,
						  freeMemory,
						  upTime,
						  NULL);
		exit(EXIT_FAILURE);
	}

	size_t largeLine = 0;

	for (size_t i = 0; i < ASCII->lines; i++) {
		size_t currentLen = strlen(ASCII->content[i]);
		if (currentLen > largeLine) {
			largeLine = currentLen;
		}
	}

	printf("%s@%s\n", user, hostname);
	printf(
		"----------------------------------------------------------------\n");
	for (size_t i = 0; i < ASCII->lines; i++) {
		printf("%s", ASCII->content[i]);
		int spacesNeeded = largeLine - strlen(ASCII->content[i]) + 4;
		if (spacesNeeded > 0) {
			printf("%*s", spacesNeeded, "");
		}
		switch (i) {
			case 0:
				printf("OS Name: %s", OSName);
				break;
			case 1:
				printf("Kernel: %s %s", sysArch, kernelVersion);
				break;
			case 2:
				printf("CPU: %s", cpu);
				break;
			case 3:
				printf("Memory: %s GB/ %s GB", maxMemory, freeMemory);
				break;
			case 4:
				printf("Up time: %s", upTime);
				break;
		}
		printf("\n");
	}

	cleanup_resources(hostname,
					  user,
					  OSName,
					  kernelVersion,
					  sysArch,
					  cpu,
					  maxMemory,
					  freeMemory,
					  upTime,
					  ASCII);
	exit(EXIT_SUCCESS);
}

size_t getFileSize(FILE *file) {
	if (fseek(file, 0, SEEK_END) != 0) {
		fprintf(stderr,
				"Error: Failed to seek to end of file: %s\n",
				strerror(errno));
		return 0;
	}

	long size = ftell(file);
	if (size == -1) {
		fprintf(stderr,
				"Error: Failed to get file position: %s\n",
				strerror(errno));
		return 0;
	}

	if (fseek(file, 0, SEEK_SET) != 0) {
		fprintf(stderr,
				"Error: Failed to seek to start of file: %s\n",
				strerror(errno));
		return 0;
	}

	return (size_t)size;
}

size_t getFileNumberOfLines(FILE *file) {
	if (fseek(file, 0, SEEK_SET) != 0) {
		fprintf(stderr,
				"Error: Failed to seek to start of file: %s\n",
				strerror(errno));
		return 0;
	}

	size_t fileSize = getFileSize(file);
	if (fileSize == 0)
		return 0;

	char *fileContent = malloc(fileSize + 1);
	if (!fileContent) {
		fprintf(stderr, "Error: Failed to allocate memory for file content\n");
		return 0;
	}

	size_t bytesRead = fread(fileContent, 1, fileSize, file);
	if (bytesRead != fileSize) {
		fprintf(stderr, "Error: Failed to read file content\n");
		free(fileContent);
		return 0;
	}
	fileContent[fileSize] = '\0';

	size_t nLines = 0;
	for (size_t i = 0; i < fileSize; i++) {
		if (fileContent[i] == '\n') {
			nLines++;
		}
	}

	free(fileContent);
	return nLines;
}

struct ASCIIArt *getASCIIArt() {
	char *OSName = getOSName();
	if (!OSName) {
		fprintf(stderr, "Error: Cannot get ASCII art without OS name\n");
		return NULL;
	}

	struct ASCIIArt *ASCIIStr = malloc(sizeof(struct ASCIIArt));
	if (!ASCIIStr) {
		fprintf(stderr,
				"Error: Failed to allocate memory for ASCII art structure\n");
		free(OSName);
		return NULL;
	}
	ASCIIStr->content = NULL;
	ASCIIStr->lines = 0;

	FILE *file = NULL;
	const char *filename = RES_PATH "/void.txt";

	if (strstr(OSName, "Void Linux")) {
		file = fopen(filename, "r");
	}

	if (!file) {
		fprintf(stderr,
				"Error: Failed to open ASCII art file '%s': %s\n",
				filename,
				strerror(errno));
		free(OSName);
		free(ASCIIStr);
		return NULL;
	}

	size_t fileLines = getFileNumberOfLines(file);
	if (fileLines == 0) {
		fprintf(stderr, "Error: ASCII art file is empty\n");
		fclose(file);
		free(OSName);
		free(ASCIIStr);
		return NULL;
	}

	ASCIIStr->content = malloc(sizeof(char *) * (fileLines + 1));
	if (!ASCIIStr->content) {
		fprintf(stderr,
				"Error: Failed to allocate memory for ASCII art lines\n");
		fclose(file);
		free(OSName);
		free(ASCIIStr);
		return NULL;
	}

	rewind(file);
	for (size_t i = 0; i < fileLines; i++) {
		char buffer[256];
		if (!fgets(buffer, sizeof(buffer), file)) {
			fprintf(stderr,
					"Error: Failed to read line %zu from ASCII art file\n",
					i + 1);
			for (size_t j = 0; j < i; j++) {
				free(ASCIIStr->content[j]);
			}
			free(ASCIIStr->content);
			fclose(file);
			free(OSName);
			free(ASCIIStr);
			return NULL;
		}

		buffer[strcspn(buffer, "\n")] = '\0';

		ASCIIStr->content[i] = strdup(buffer);
		if (!ASCIIStr->content[i]) {
			fprintf(stderr,
					"Error: Failed to duplicate ASCII art line %zu\n",
					i + 1);
			for (size_t j = 0; j < i; j++) {
				free(ASCIIStr->content[j]);
			}
			free(ASCIIStr->content);
			fclose(file);
			free(OSName);
			free(ASCIIStr);
			return NULL;
		}
	}

	ASCIIStr->lines = fileLines;
	ASCIIStr->content[fileLines] = NULL;
	fclose(file);
	free(OSName);
	return ASCIIStr;
}

char *getOSName() {
	FILE *f = fopen("/etc/os-release", "r");
	if (!f) {
		fprintf(stderr,
				"Error: Failed to open /etc/os-release: %s\n",
				strerror(errno));
		return NULL;
	}

	char *buffer = malloc(256);
	if (!buffer) {
		fprintf(stderr, "Error: Failed to allocate memory for OS name\n");
		fclose(f);
		return NULL;
	}
	buffer[0] = '\0';

	char line[256];
	int found = 0;

	while (fgets(line, sizeof(line), f)) {
		if (strstr(line, "PRETTY_NAME=")) {
			if (sscanf(line, "PRETTY_NAME=\"%255[^\"]", buffer) == 1) {
				found = 1;
				break;
			}
		}
	}

	fclose(f);

	if (!found) {
		fprintf(stderr, "Error: PRETTY_NAME not found in /etc/os-release\n");
		free(buffer);
		return NULL;
	}

	return buffer;
}

char *getKernel() {
	FILE *f = fopen("/proc/version", "r");
	if (!f) {
		fprintf(stderr,
				"Error: Failed to open /proc/version: %s\n",
				strerror(errno));
		return NULL;
	}

	char *buffer = malloc(256);
	if (!buffer) {
		fprintf(stderr,
				"Error: Failed to allocate memory for kernel version\n");
		fclose(f);
		return NULL;
	}

	if (fscanf(f, "%*s %*s %255s", buffer) != 1) {
		fprintf(stderr, "Error: Failed to parse kernel version\n");
		fclose(f);
		free(buffer);
		return NULL;
	}

	fclose(f);
	return buffer;
}

char *getSysArch() {
	struct utsname info;
	if (uname(&info) == -1) {
		fprintf(stderr,
				"Error: Failed to get system information: %s\n",
				strerror(errno));
		return NULL;
	}

	char *buffer = malloc(10);
	if (!buffer) {
		fprintf(stderr,
				"Error: Failed to allocate memory for system architecture\n");
		return NULL;
	}

	strncpy(buffer, info.machine, 9);
	buffer[9] = '\0';

	return buffer;
}

char *getUpTime() {
	struct sysinfo info;
	if (sysinfo(&info)) {
		fprintf(
			stderr, "Error: Failed to get system info: %s\n", strerror(errno));
		return NULL;
	}

	char *buffer = malloc(256);
	if (!buffer) {
		fprintf(stderr, "Error: Failed to allocate memory for uptime string\n");
		return NULL;
	}

	snprintf(buffer,
			 256,
			 "%ld d %ld h %ld m",
			 info.uptime / 86400,
			 (info.uptime / 3600) % 24,
			 (info.uptime / 60) % 60);

	return buffer;
}

char *getCPU() {
	FILE *f = fopen("/proc/cpuinfo", "r");
	if (!f) {
		fprintf(stderr,
				"Error: Failed to open /proc/cpuinfo: %s\n",
				strerror(errno));
		return NULL;
	}

	char *buffer = malloc(256);
	if (!buffer) {
		fprintf(stderr, "Error: Failed to allocate memory for CPU info\n");
		fclose(f);
		return NULL;
	}
	buffer[0] = '\0';

	char line[256];
	int found = 0;

	while (fgets(line, sizeof(line), f)) {
		if (strstr(line, "model name")) {
			char *colon = strchr(line, ':');
			if (colon) {
				char *start = colon + 2;
				while (*start == ' ')
					start++;
				strncpy(buffer, start, 255);
				buffer[255] = '\0';
				char *newline = strchr(buffer, '\n');
				if (newline)
					*newline = '\0';
				found = 1;
				break;
			}
		}
	}

	fclose(f);

	if (!found) {
		fprintf(stderr, "Error: CPU model name not found in /proc/cpuinfo\n");
		free(buffer);
		return NULL;
	}

	return buffer;
}

char *getMaxMemory() {
	struct sysinfo info;
	if (sysinfo(&info)) {
		fprintf(
			stderr, "Error: Failed to get system info: %s\n", strerror(errno));
		return NULL;
	}

	char *buffer = malloc(10);
	if (!buffer) {
		fprintf(stderr,
				"Error: Failed to allocate memory for total RAM string\n");
		return NULL;
	}

	double totalRam = (double)(info.totalram) / (1024 * 1024 * 1024);
	snprintf(buffer, 10, "%.1f", totalRam);

	return buffer;
}

char *getFreeMemory() {
	struct sysinfo info;
	if (sysinfo(&info)) {
		fprintf(
			stderr, "Error: Failed to get system info: %s\n", strerror(errno));
		return NULL;
	}

	char *buffer = malloc(10);
	if (!buffer) {
		fprintf(stderr,
				"Error: Failed to allocate memory for free RAM string\n");
		return NULL;
	}

	double freeMemory = (double)(info.freeram) / (1024 * 1024 * 1024);
	snprintf(buffer, 10, "%.1f", freeMemory);

	return buffer;
}
