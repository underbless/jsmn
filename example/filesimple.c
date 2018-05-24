#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

static const char *JSON_STRING =
	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

char *readJSONFile() {
	FILE *fp = fopen("data.json", "r");
	char *JSON_STRING;
	JSON_STRING = malloc(sizeof(char)*50);
	char line[255];
	int count = 0;

	while(1) {
			fgets(line, sizeof(line), fp);
			if(feof(fp)) break;
			count += strlen(line );
			JSON_STRING = realloc(JSON_STRING, count+1);
			strcat(JSON_STRING, line);
	}

	fclose(fp);
	return JSON_STRING;
}

void jsonNameList(char *JSON_STR, jsmntok_t *t, int tokcount, int *nametokIndex) {
	printf("******* Name List *******\n");
	int count = 1;
	int i;

	for(i = 1; i < tokcount; i++) {
		if(t[i].size >= 1){
			nametokIndex[count] = i;
			printf("[NAME%2d] %.*s\n", count,  t[i].end-t[i].start, JSON_STR + t[i].start);
			count++;
			i++;
		}
	}
}

void printNameList(char *JSON_STR, jsmntok_t *t, int *nametokIndex){
	printf("******* Name List *******\n");
	int count = 1;
	int i = 0;
	int j;
	for(j = 0; j < 100; j++){
		printf("[NAME%2d] %.*s\n", count, t[nametokIndex[j]].end-t[nametokIndex[j]].start, JSON_STR + t[nametokIndex[j]].start);
		count++;
		i++;
	}
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int main() {
	int i;
	int r;
	char *JSON_STR;
	int *nametokIndex;
	JSON_STR = readJSONFile();
	//printf("%s", JSON_STR);

	nametokIndex = malloc(sizeof(int)*100);

	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STR, strlen(JSON_STR), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	//printNameList(JSON_STR, t, nametokIndex);
	jsonNameList(JSON_STR, t, r, nametokIndex);
	return 0;
	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STR, &t[i], "name") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- name: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STR + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STR, &t[i], "keywords") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- keywords: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STR + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STR, &t[i], "description") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STR + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STR, &t[i], "examples") == 0) {
			int j;
			printf("- examples:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, JSON_STR + g->start);
			}
			i += t[i+1].size + 1;
		} /*else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
					JSON_STR + t[i].start);
		}*/
	}
	return EXIT_SUCCESS;
}
