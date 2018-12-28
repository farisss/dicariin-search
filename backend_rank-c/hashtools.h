
typedef struct HashDocInfo_t {
	int id;
	int freq;
	struct HashDocInfo_t *nextDocInfo;
} HashDocInfo;

typedef struct HashTableEntry_t {
	int hasEntry;	/**/ 
	int strSize;	/* Panjang string pada strData, termasuk null-byte terminator */
	char *strData;	/**/
	int count;		/**/
	HashDocInfo *firstDocInfo;	/**/
} HashTableEntry;

typedef struct HashTableInfo_t {
	int tableSize;	/* besar memori yang dialokasikan untuk hash table */
	int tableCount;	/* banyak list array yg dialokasikan */
	int entryCount;	/* banyak entry yang masuk dalam tabel */
	int docCount;	/* banyak dokumen yang diindeks */
	HashTableEntry *entry;
} HashTableInfo;

typedef struct HashTableFileHeader_t {
	int tableSize;	/* besar memori yang dialokasikan untuk hash table */
	int tableCount;	/* banyak list array yg dialokasikan */
	int entryCount;	/* banyak entry yang masuk dalam tabel */
	int docCount;	/* banyak dokumen yang diindeks */
} HashTableFileHeader;

typedef struct HashTableFileEntry_t {
	int hashIndex;
	int strSize;
	int count;
} HashTableFileEntry;

typedef struct HashTableFileDocInfo_t {
	int id;
	int freq;
} HashTableFileDocInfo;

/* Beri nama tipe data untuk hash */
/*typedef unsigned long HashInt;*/

HashTableInfo *initHashTable(int tableCount);
HashTableInfo *openHashTable(char *inPath);
int saveHashTable(HashTableInfo *table, char *output);
void freeHashTable(HashTableInfo *table);

void newTableEntry(HashTableInfo *table, HashTableEntry *entry, char *word);

HashDocInfo *newDocInfoNode(int docIdVal);
void deleteDocInfoNode(HashDocInfo *node);
void deleteDocInfoNodes(HashDocInfo *node);
int queryHashIndex(HashTableInfo *table, char *word);

void addWord(HashTableInfo *table, int docId, char *word);

int calculateHash(char *word, int max);
