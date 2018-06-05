#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../productlist.h"

#define JSMN_PARENT_LINKS
/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

static const char *JSON_STRING =
	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

char *readJSONFile() {
	FILE *fp = fopen("data4.json", "r");
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

static int jsoneq(const char *json, jsmntok_t *tok, jsmntok_t *tok1) {
	if (tok->type == JSMN_STRING && (int) tok1->end - tok1->start == tok->end - tok->start &&
			strncmp(json + tok->start, json + tok1->start, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int jsonNameList(char *JSON_STR, jsmntok_t *t, int tokcount, NameTokenInfo *nameTokenInfo) {
	//printf("******* Name List *******\n");
	int count = 1;
	int i;
	if(t[0].type == JSMN_ARRAY){ //array로 시작할 때. Object가 여러개일 때.
			for(i = 0; i < tokcount; i++) {
				if(t[i].size >= 1 && t[i].type == JSMN_STRING && t[t[i].parent].type == JSMN_OBJECT) {
					nametokIndex[count] = i;
					count++;
				}
			}
	}
	else{ // 시작이 array가 아닌 object일 때.
		i = 0;
		if(t[1].type == JSMN_STRING && t[2].type != JSMN_STRING){
			i = 3;
			for(i; i < tokcount; i++) {
				if(t[i].size >= 1 && t[i].type == JSMN_STRING && t[t[i].parent].type == JSMN_OBJECT) {
					nametokIndex[count] = i;
					count++;
				}
			}
			return;
		}
		for(i; i < tokcount; i++) {
			if(t[i].size >= 1 && t[i].type == JSMN_STRING) {
				nametokIndex[count] = i;
				count++;
				break;
			}

		}
		int a = nametokIndex[1];
		for(i = a + 1; i < tokcount; i++){
				if(t[i].parent == t[a].parent && t[i].size >= 1){
					nametokIndex[count] = i;
					count++;
					i++;
				}
		}
	}
}

void printNameList(char *JSON_STR, jsmntok_t *t, int *nametokIndex){
	printf("******* Name List *******\n");
	int count = 1;
	int j;
	//int countObject = 0;
	for(j = 1; j <= 100; j++){
	//for(int a  = 0; a <= 100; a++){   //실험용 돌리
		int a = nametokIndex[j];
		if(a == 0) break;
		//if(t[a].type == JSMN_OBJECT)
		//	countObject++;
		//if(JSON_STR[t[a].end +1] == '}')
		//	countObject--;
		//printf("%d, %d, %d   [NAME%2d] %.*s\n",t[a].type, t[a].parent, countObject, count, t[a].end-t[a].start, JSON_STR + t[a].start);
		printf("[NAME%2d] %.*s\n", count, t[a].end-t[a].start, JSON_STR + t[a].start);
		count++;
	}
}

void selectNameList(char *JSON_STR, jsmntok_t *t, int *nametokIndex){
	int ans = -1;
	while(ans != 0) {
			printf("Select Name's No. (exit:0) >> ");
			scanf("%d", &ans);
			if(ans == 0) break;
			int a = nametokIndex[ans];
			printf("[NAME%2d] %.*s\n", ans, t[a].end-t[a].start, JSON_STR + t[a].start);
			printf("%.*s\n", t[a+1].end-t[a+1].start, JSON_STR + t[a+1].start);
	}
}

void objectnameList(char *JSON_STR, jsmntok_t *t, int tokcount, int *objectCount, int *nametokIndex) {
	int a = nametokIndex[1]; //첫 name
	int count = 1;
	int i;
	for(i = 1; i < tokcount; i++){
		if(t[0].type == JSMN_ARRAY || t[1].type == JSMN_STRING && t[2].type != JSMN_STRING){
			if(t[t[a].parent].parent == t[t[i].parent].parent && jsoneq(JSON_STR, &t[i], &t[a]) == 0){
				objectCount[count] = i;
				i++; count++;
			}
		}
		else{
			if(t[a].parent == t[i].parent && jsoneq(JSON_STR, &t[i], &t[a]) == 0){
				objectCount[count] = i;
				i++; count++;
			}
		}
	}
}

void objectprintList(char *JSON_STR, jsmntok_t *t, int *objectCount) {
	int count = 1;
	int a;
	int i = 1;
	printf("******* Object List *******\n");
	for(;; i++){
		a = objectCount[i];
		if(a == 0) break;
		printf("[NAME%2d] %.*s\n", count, t[a+1].end - t[a+1].start,
																		JSON_STR + t[a+1].start);
		count++;
	}
}

void printbyNum(char *JSON_STR, jsmntok_t *t, int *objectCount, int *nametokIndex){
	int ans;
	printf("원하는 번호를 입력 (Exit:0) : ");
	if(ans == 0) return;
	scanf("%d", &ans);
	int a = nametokIndex[ans];
	if( a == 0 ){
		printf("Undefined number\n");
		return;
	}
	int b = nametokIndex[ans+1];
	if(b != 0 ) {
		printf("%.*s : %.*s\n", t[a].end - t[a].start, JSON_STR + t[a].start, t[a+1].end - t[a+1].start, JSON_STR + t[a+1].start);
		for(int i = a+2 ; i < b /2; i+=2){
			printf("\t[%.*s] %.*s\n", t[i].end - t[i].start, JSON_STR + t[i].start, t[i+1].end - t[i+1].start, JSON_STR + t[i+1].start);
			if(t[i+1].type == JSMN_ARRAY)
				i++;
		}
	}
	else {
		b = nametokIndex[ans-1];
		int c = (a - b) / 2 - 2;
		printf("%.*s : %.*s\n", t[a].end - t[a].start, JSON_STR + t[a].start, t[a+1].end - t[a+1].start, JSON_STR + t[a+1].start);
		a += 2;
		for(int i = 0; i < c; i++){
			printf("\t[%.*s] %.*s\n", t[a].end - t[a].start, JSON_STR + t[a].start, t[a+1].end - t[a+1].start, JSON_STR + t[a+1].start);
			if(t[a+1].type == JSMN_ARRAY)
				a++;
			a+=2;
		}
	}
}

int main() {
	int i;
	int r;
	char *JSON_STR;
	int objectCount[100] = {0};
	NameTokenInfo nameTokenInfo[100] = {0};
	JSON_STR = readJSONFile();
	//printf("%s", JSON_STR);

	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STR, strlen(JSON_STR), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}
	jsonNameList(JSON_STR, t, r, nametokIndex);
	//printNameList(JSON_STR, t, nametokIndex);
	//objectnameList(JSON_STR, t, r, objectCount, nametokIndex);
	//objectprintList(JSON_STR, t, objectCount);
	printbyNum(JSON_STR, t, objectCount, nametokIndex);
	//selectNameList(JSON_STR, t, nametokIndex);

	return 0;
	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STR, &t[i], "name") == 0) {
			/* We may use strndup() to fetch string value
			printf("- name: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STR + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STR, &t[i], "keywords") == 0) {
			/* We may additionally check if the value is either "true" or "false"
			printf("- keywords: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STR + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STR, &t[i], "description") == 0) {
			/* We may want to do strtol() here to get numeric value
			printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STR + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STR, &t[i], "examples") == 0) {
			int j;
			printf("- examples:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, JSON_STR + g->start);
			}
			i += t[i+1].size + 1;
		} /*else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
					JSON_STR + t[i].start);
		}
	}
	*/
	return EXIT_SUCCESS;
}
