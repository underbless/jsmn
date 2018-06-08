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

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

/*
static int jsoneq(const char *json, jsmntok_t *tok, jsmntok_t *tok1) {
	if (tok->type == JSMN_STRING && (int) tok1->end - tok1->start == tok->end - tok->start &&
			strncmp(json + tok->start, json + tok1->start, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}
*/

int jsonNameList(char *JSON_STR, jsmntok_t *t, int tokcount,
									NameTokenInfo *nameTokenInfo) {
	//printf("******* Name List *******\n");
	int count = 1;
	int i = 0;
	int j = 1;
	if(t[1].type == JSMN_STRING && t[2].type != JSMN_STRING){
		for(i = 3; i < tokcount; i++) {  //이건 data4의 포멧에 맞춰서,,,하는,,
			if(t[i].size >= 1 && t[i].type == JSMN_STRING &&
				t[t[i].parent].type == JSMN_OBJECT) {
				nameTokenInfo[count].tokindex = i;
				nameTokenInfo[count].objectindex = j;
				count++;
				if(t[i].parent!=t[i+2].parent) {
					j++;
				}
			}

		}
		return 0;
	}
}

int tkNameReturn(char* JSON_STR, jsmntok_t *t, int tokcount, int objectNum,
									const char *STR, NameTokenInfo *nameTokenInfo) {
	for(int i = 1; i < tokcount; i++){
		if (nameTokenInfo[i].objectindex == objectNum &&
			jsoneq(JSON_STR, &t[nameTokenInfo[i].tokindex], STR) == 0) {
			return nameTokenInfo[i].tokindex;
		}
	}
	return 0;
}

void printProductList(char* JSON_STR, jsmntok_t *t, int tokcount,
											NameTokenInfo *nameTokenInfo){
	printf("\t***********************************\n");
	printf("\t번호\t제품명\t제조사\t가격\t수량\t\n");
	printf("\t***********************************\n");
	for(int i = 1; i < 5; i++){
		int name = tkNameReturn(JSON_STR, t, tokcount, i, "name", nameTokenInfo);
		int company = tkNameReturn(JSON_STR, t, tokcount, i, "company", nameTokenInfo);
		int price = tkNameReturn(JSON_STR, t, tokcount, i, "price", nameTokenInfo);
		int count = tkNameReturn(JSON_STR, t, tokcount, i, "count", nameTokenInfo);
		printf("\t%d\t%.*s\t%.*s\t%.*s\t%.*s\t\n",
		i, t[company+1].end - t[company+1].start, JSON_STR + t[company+1].start,
						t[name+1].end - t[name+1].start, JSON_STR + t[name+1].start,
					t[price+1].end - t[price+1].start, JSON_STR + t[price+1].start,
					 t[count+1].end - t[count+1].start, JSON_STR + t[count+1].start);
	}
}

/*
void printbyNum(char *JSON_STR, jsmntok_t *t, int *objectCount, NameTokenInfo *nameTokenInfo){
	int ans;
	printf("원하는 번호를 입력 (Exit:0) : ");
	if(ans == 0) return;
	scanf("%d", &ans);
	int a = nameTokenInfo[ans].tokindex;;
	if( a == 0 ){
		printf("Undefined number\n");
		return;
	}
	int b = nameTokenInfo[ans+1].tokindex;
	if(b != 0 ) {
		printf("%.*s : %.*s\n", t[a].end - t[a].start, JSON_STR + t[a].start, t[a+1].end - t[a+1].start, JSON_STR + t[a+1].start);
		for(int i = a+2 ; i < b /2; i+=2){
			printf("\t[%.*s] %.*s\n", t[i].end - t[i].start, JSON_STR + t[i].start, t[i+1].end - t[i+1].start, JSON_STR + t[i+1].start);
			if(t[i+1].type == JSMN_ARRAY)
				i++;
		}
	}
	else {
		b = nameTokenInfo[ans-1].tokindex;
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
*/

int main() {
	int i;
	int r;
	char *JSON_STR;
	int objectNum = 0;
	char name[20];

	NameTokenInfo nameTokenInfo[100];
	JSON_STR = readJSONFile();
	//printf("%s", JSON_STR);
	/*printf("Please Enter a ObjectIndex to print : ");
	scanf("%d", &objectNum);
	printf("Please Enter a name to print : ");
	scanf("%s", &name);*/

	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STR, strlen(JSON_STR), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}
	jsonNameList(JSON_STR, t, r, nameTokenInfo);
	tkNameReturn(JSON_STR, t, r, objectNum, name, nameTokenInfo);
	printProductList(JSON_STR, t, r, nameTokenInfo);

	return 0;
	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}
	return EXIT_SUCCESS;
}
