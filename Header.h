/* Included Files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

/* Definition of constant values */
#define MAX_INPUT_SIZE 100
#define       MB       1048576

/* Strucutres Used */
typedef unsigned char BYTE; // 1 Byte can represent 0 ~ 255
typedef struct node{  // Structure for linked list(command history)
    char txt[MAX_INPUT_SIZE];
    struct node *next;
}
node;
typedef struct table{ // Strucutre for linked list(opcode hash table)
    char opcode[20];
    char type[10];
    int value;
    struct table *next;
}
table;

/* Function Declare */
int command_check(char *);
void help(void);
void dir(void);
void quit(node *, table*[]);
void history(node *);
void dump(char *, BYTE [], int *, int *, int *);
void edit(char *, BYTE [], int *);
void fill(char *, BYTE [], int *);
void reset(char *, BYTE []);
void create_hash_table(table*[]);
void opcode_search(char*, table*[],int*);
void print_opcode(table*[]);
int string_compare(char *, char *, int);
int parsing(char *, char *, char *, char *, int, int*);
int hashing(char *);
node* insert(node *, char *);
void ascii(int);
int change_hex(char);
int change_hex_string_to_int(char*, int*);
void print_dump(BYTE [], int);
void print_dump_start_end(BYTE [], int, int);
