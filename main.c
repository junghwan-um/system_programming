/* Included Files */
#include "Header.h"

/* Program Start */
int main(){
    char cmd[MAX_INPUT_SIZE]; // Command input by User
    node *list = NULL; // Linked list saving command history
    table *code[20]; // Hash table for opcodes
    BYTE memory[MB] = {0}; // Allocating 1mb and intializing it to 0

    int *dump_flag = (int *)malloc(sizeof(int)); // To see if dump\n command is first or not
    int *address = (int *)malloc(sizeof(int)); // Keep track of dump\n command

    int *error_flag = (int *)malloc(sizeof(int)); // If error is found, flag = 1
    *dump_flag = 0;
    *address = 0;
    create_hash_table(code); // Create hash table of opcodes
    while(1) {
        *error_flag = 0; // Before command, reset error flag
        printf("sicsim> ");
        fgets(cmd, MAX_INPUT_SIZE, stdin); // Read command from user
        switch(command_check(cmd)){ // Check command
            case 1:
                help();
                break;
            case 2:
                dir();
                break;
            case 3:
                quit(list,code);
                break;
            case 4:
                history(list);
                break;
            case 5:
                dump(cmd, memory, dump_flag, address, error_flag);
                break;
            case 6:
                edit(cmd, memory, error_flag);
                break;
            case 7:
                fill(cmd, memory, error_flag);
                break;
            case 8:
                reset(cmd, memory);
                break;
            case 9:
                opcode_search(cmd, code, error_flag);
                break;
            case 10:
                print_opcode(code);
                break;
            default:
                printf("ERROR: WRONG COMMAND INPUT\n"); // If none of the above
                *error_flag = 1;
                break;
        }
        if (command_check(cmd) == 3) break;
        if (*error_flag != 1) list = insert(list,cmd); // Update linked list(command history)
    }
    free(dump_flag);
    free(address);
    free(error_flag);
}

/*------------------------------------------------------------------------------------*/
/* Function : command_check(char *cmd) */
/* Object : Check shell command and return corressponding integer */
/* Return Value : 1 ~ 10 */
/*------------------------------------------------------------------------------------*/
int command_check(char *cmd){
    if (strcmp(cmd, "help\n") == 0 || strcmp(cmd, "h\n") == 0) return 1;
    else if (strcmp(cmd, "dir\n") == 0 || strcmp(cmd, "d\n") == 0) return 2;
    else if (strcmp(cmd, "quit\n") == 0 || strcmp(cmd, "q\n") == 0) return 3;
    else if (strcmp(cmd, "history\n") == 0 || strcmp(cmd, "hi\n") == 0) return 4;
    else if (string_compare(cmd, "dump", 4) != 1 || string_compare(cmd, "du", 2) != 1) return 5;
    else if (string_compare(cmd, "edit", 4) != 1 || string_compare(cmd, "e", 1) != 1) return 6;
    else if (string_compare(cmd, "fill", 4) != 1 || string_compare(cmd, "f", 1) != 1) return 7;
    else if (strcmp(cmd, "reset\n") == 0) return 8;
    else if (string_compare(cmd, "opcode", 6) != 1) return 9;
    else if (strcmp(cmd, "opcodelist\n") == 0) return 10;
    else return -1;
}

/*------------------------------------------------------------------------------------*/
/* Function : help(void) */
/* Object : Print available commands */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void help(void){
    printf("h[elp]\n");
    printf("d[ir]\n");
    printf("q[uit]\n");
    printf("hi[story]\n");
    printf("du[mp] [start, end]\n");
    printf("e[dit] address, value\n");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\n");
    printf("opcodelist\n");
}
/*------------------------------------------------------------------------------------*/
/* Function : dir(void) */
/* Object : Print the files and directory in the directory */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void dir(void){
    DIR *d;
     struct dirent *dir;
     d = opendir(".");
     if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_type == DT_DIR) printf("%s/\n", dir->d_name); // If it is directory
            else if(access(dir->d_name, X_OK) != -1) printf("%s*\n", dir->d_name); // If it is executable file
            else printf("%s\n", dir->d_name); // If it is ordinary file
        }
    }
    closedir(d);
}

/*------------------------------------------------------------------------------------*/
/* Function : quit(node *list) */
/* Object : Free the memories of linked lists(command histories & hash table) */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void quit(node *list, table *code[]){
    while(list != NULL) { // Free the memories of command history
        node *tmp = list->next;
        free(list);
        list = tmp;
    }
    for(int i = 0; i < 20; i++) { // Free the memories of hash table
        while(code[i] != NULL) {
            table *tmp = code[i]->next;
            free(code[i]);
            code[i] = tmp;
        }
    }
}

/*------------------------------------------------------------------------------------*/
/* Function : history(node *list) */
/* Object : Print linked list(command history) */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void history(node *list){
    node *tmp = list;
    for(int i = 1; tmp != NULL; tmp = tmp->next) printf("%5d    %s",i++, tmp->txt);
}

/*------------------------------------------------------------------------------------*/
/* Function : dump(char *cmd, BYTE memory[], int *dump_flag, int *address, int *error_flag) */
/* Object : Print dump memory to the corressponding command input
            dump: print 10 lines of dump and update address
            dump start: starting from start, print 160 memories
            dump start,end: print memories from start to end */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void dump(char *cmd, BYTE memory[], int *dump_flag, int *address, int *error_flag){
    int x = 0,y = 0, tmp, comma_flag;
    char parse_a[100] = {0}; // Dump Command before comma (Start Address)
    char parse_b[100] = {0}; // Dump Command after comma (End Address)
    char parse_c[100] = {0}; // Need to create because of parsing function
    tmp = *address; // Temporarily storing address to keep updating
    if (strncmp(cmd, "dump", 4) == 0) { // The input command is "dump"
        comma_flag = parsing(cmd, parse_a, parse_b, parse_c, 4, error_flag);
        switch (comma_flag) { // Categorize by comma count
            case 0: // Dump or dump start
                if (cmd[4] == '\n') { // Dump\n
                    if(*dump_flag == 0){ // First time inputing dump
                        print_dump(memory, tmp);
                        *dump_flag = 1; // Set the flag to 1 so it does not repeat from 0
                    }
                    else{
                        print_dump(memory, tmp);
                    }
                    *address += 160; // Update Address
                }
                else if (cmd[4] == ' ' || cmd[4] == '\t') { // dump start
                    x = change_hex_string_to_int(parse_a, error_flag); // Change hexadecimal string(before comma)to integer
                    if(*error_flag != 1) print_dump_start_end(memory, x, x+159);
                }
                break;
            case 1: // dump start, end
                if (cmd[4] == ' ' || cmd[4] == '\t'){
                    x = change_hex_string_to_int(parse_a, error_flag); // Change hexadecimal string(before comma)to integer
                    y = change_hex_string_to_int(parse_b, error_flag); // Change hexadecimal string(after comma)to integer
                    if (x > y) {
                        printf("ERROR : End Address is lower than Start Address\n");
                        *error_flag = 1;
                    }
                    else {
                        if(*error_flag != 1) {
                            print_dump_start_end(memory,x,y);
                            sprintf(cmd,"dump %X, %X\n", x, y);
                        }
                    }
                }
                break;
            default:
                printf("ERROR: Only One Comma Allowed\n");
                *error_flag = 1;
                break;
        }
    }
    else if (strncmp(cmd, "du", 2) == 0){ // The input command is "du"
        comma_flag = parsing(cmd, parse_a, parse_b, parse_c, 2, error_flag);
        switch (comma_flag) { // Categorize by comma count
            case 0: // dump or dump start
                if (cmd[2] == '\n') { // dump
                    if(*dump_flag == 0){ // First time inputing dump
                        print_dump(memory, tmp);
                        *dump_flag = 1;
                    }
                    else{
                        print_dump(memory, tmp);
                    }
                    *address += 160;
                }
                else if (cmd[2] == ' ' || cmd[2] == '\t') { // dump start
                    x = change_hex_string_to_int(parse_a, error_flag); // Change hexadecimal string(before comma)to integer
                    if(*error_flag != 1) print_dump_start_end(memory, x, x+159);
                }
                break;
            case 1:
                if (cmd[2] == ' ' || cmd[2] == '\t'){ // dump start, end
                    x = change_hex_string_to_int(parse_a, error_flag); // Change hexadecimal string(before comma)to integer
                    y = change_hex_string_to_int(parse_b, error_flag); // Change hexadecimal string(after comma)to integer
                    if (x > y) {
                        printf("ERROR : End Address is lower than Start Address\n");
                        *error_flag = 1;
                    }
                    else {
                        if(*error_flag != 1) {
                            print_dump_start_end(memory,x,y);
                            sprintf(cmd,"du %X, %X\n", x, y);
                        }
                    }
                }
                break;
            default:
                printf("ERROR: Only One Comma Allowed\n");
                *error_flag = 1;
                break;
        }
    }
    else{
        printf("Wrong Dump Commmand\n");
        *error_flag = 1;
    }
}

/*------------------------------------------------------------------------------------*/
/* Function : edit(char *cmd, BYTE memory[], int *error_flag) */
/* Object : Edit memory with input  */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void edit(char *cmd, BYTE memory[], int *error_flag) {
    int x = 0,y = 0, comma_flag;
    char parse_a[100] = {0}; // Edit Command before comma (Address)
    char parse_b[100] = {0}; // Edit Command after comma (Value)
    char parse_c[100] = {0}; // Need to create because of parsing function
    if (strncmp(cmd, "edit", 4) == 0) { // If input is "edit"
        comma_flag = parsing(cmd, parse_a, parse_b, parse_c, 4, error_flag); // Counting comma
        switch (comma_flag) { // Categorize by comma count
            case 1:
                if (cmd[4] == ' ' || cmd[4] == '\t'){ // edit
                    if (strlen(parse_b) > 2) { // To avoid input values higher than 255
                        printf("Input value must be lower than 256\n");
                        *error_flag = 1;
                    }
                    else {
                        x = change_hex_string_to_int(parse_a, error_flag); // Change hexadecimal string to int (Address)
                        y = change_hex_string_to_int(parse_b, error_flag); // Change hexadecimal string to int (Value)
                        if(*error_flag != 1) {
                            memory[x] = y;
                            sprintf(cmd,"edit %X, %X\n", x, y);
                        }
                    }
                }
                break;
            default:
                printf("ERROR: Only One Comma Allowed\n");
                *error_flag = 1;
                break;
        }
    }
    else if (strncmp(cmd, "e", 1) == 0){ // If input is "e"
        comma_flag = parsing(cmd, parse_a, parse_b, parse_c, 1, error_flag);
        switch (comma_flag) { // Categorize by comma count
            case 1:
                if (cmd[1] == ' ' || cmd[1] == '\t'){ // edit
                    if (strlen(parse_b) > 2) { // To avoid input values higher than 255
                        printf("Input value must be lower than 256\n");
                        *error_flag = 1;
                    }
                    else {
                        x = change_hex_string_to_int(parse_a, error_flag); // Change hexadecimal string to int (Address)
                        y = change_hex_string_to_int(parse_b, error_flag); // Change hexadecimal string to int (Value)
                        if(*error_flag != 1) {
                            memory[x] = y;
                            sprintf(cmd,"e %X, %X\n", x, y);
                        }
                    }
                }
                break;
            default:
                printf("ERROR: Only One Comma Allowed\n");
                *error_flag = 1;
                break;
        }
    }
    else{
        printf("Wrong Edit Commmand\n");
        *error_flag = 1;
    }
}

/*------------------------------------------------------------------------------------*/
/* Function : fill(char *cmd, BYTE memory[], int *error_flag) */
/* Object : Fill memory from start address to end address with input  */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void fill(char *cmd, BYTE memory[], int *error_flag) {
    int x = 0, y = 0, z = 0,comma_flag;
    char parse_a[100] = {0}; // Fill Command before first comma (Start Address)
    char parse_b[100] = {0}; // Fill Command before second comma (End Address)
    char parse_c[100] = {0}; // Fill Command after second comma (Value)
    if (strncmp(cmd, "fill", 4) == 0) {
        comma_flag = parsing(cmd, parse_a, parse_b, parse_c, 4, error_flag);
        switch (comma_flag) { // Categorize by comma count
            case 2:
                if (cmd[4] == ' ' || cmd[4] == '\t'){ // fill
                    if (strlen(parse_c) > 2) { // To avoid input values higher than 255
                        printf("ERROR: Input value must be lower than 256\n");
                        *error_flag = 1;
                    }
                    else {
                        x = change_hex_string_to_int(parse_a, error_flag); // Change hexadecimal string to int (Start Address)
                        y = change_hex_string_to_int(parse_b, error_flag); // Change hexadecimal string to int (End Address)
                        z = change_hex_string_to_int(parse_c, error_flag); // Change hexadecimal string to int (Value)
                        if(*error_flag != 1) {
                            for(int i = x; i <= y; i++) {
                                memory[i] = z;
                            }
                            sprintf(cmd,"fill %X, %X, %X\n", x, y, z);
                        }
                    }
                }
                break;
            default:
                printf("ERROR: Only Two Comma Allowed\n");
                *error_flag = 1;
                break;
        }
    }
    else if (strncmp(cmd, "f", 1) == 0){
        comma_flag = parsing(cmd, parse_a, parse_b, parse_c, 1, error_flag);
        switch (comma_flag) { // Categorize by comma count
            case 2:
                if (cmd[1] == ' ' || cmd[1] == '\t'){ // fill
                    if (strlen(parse_c) > 2) { // To avoid input values higher than 255
                        printf("Input value must be lower than 256\n");
                        *error_flag = 1;
                    }
                    else {
                        x = change_hex_string_to_int(parse_a, error_flag); // Change hexadecimal string to int (Start Address)
                        y = change_hex_string_to_int(parse_b, error_flag); // Change hexadecimal string to int (End Address)
                        z = change_hex_string_to_int(parse_c, error_flag); // Change hexadecimal string to int (Value)
                        if(*error_flag != 1) {
                            for(int i = x; i <= y; i++) {
                                memory[i] = z;
                            }
                            sprintf(cmd,"f %X, %X, %X\n", x, y, z);
                        }
                    }
                }
                break;
            default:
                printf("ERROR: Only Two Comma Allowed\n");
                *error_flag = 1;
                break;
        }
    }
    else{
        printf("Wrong Fill Commmand\n");
        *error_flag = 1;
    }
}

/*------------------------------------------------------------------------------------*/
/* Function : reset(char *cmd, BYTE memory[]) */
/* Object : reset all the memory to 0  */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void reset(char *cmd, BYTE memory[]) {
    for(int i = 0; i < MB; i++) {
        memory[i] = 0;
    }
}

/*------------------------------------------------------------------------------------*/
/* Function : create_hash_table(table *code[]) */
/* Object : create hash table of linked lists using hash function  */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void create_hash_table(table *code[]) {
    FILE *ifile;
    int *error_flag = 0;
    char string_value[30], string_opcode[30], string_type[30]; // Input operand value, opcode, type
    int input, hash_result;
    for(int i = 0; i < 20; i++) { // Initializing the table
        code[i] = NULL;
    }
    ifile = fopen("opcode.txt","r");
    if (ifile == NULL){ // If memory is full
        printf("File can not be found!!\n");
    }
    else {
        while(1) {
            input = fscanf(ifile,"%s %s %s", string_value, string_opcode, string_type);
            if (input == EOF) break;
            table *n = (table*)malloc(sizeof(table));
            table *temp = NULL;
            n->value = change_hex_string_to_int(string_value, error_flag); // Change hexadecimal string(value) to n's value
            strcpy(n->opcode, string_opcode);
            strcpy(n->type, string_type);
            n->next = NULL;
            hash_result = hashing(n->opcode); // Hash function to decrypt opcode
            if(code[hash_result] == NULL) code[hash_result] = n; // if the head of the table is empty
            else {
                temp = code[hash_result];
                while(temp->next != NULL) temp = temp->next;
                temp->next = n;
            }
        }
    }
}

/*------------------------------------------------------------------------------------*/
/* Function : opcode_search(char *cmd, table *code[], int *error_flag) */
/* Object : Search the opcode and return the value of it  */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void opcode_search(char *cmd, table *code[], int *error_flag) {
    char string[100] = {0}; // opcode command before '\n'[
    char a[100] = {0}; // Need to create because of parsing function
    char b[100] = {0}; // Need to create because of parsing function
    int comma_count = parsing(cmd, string, a, b, 6, error_flag);
    int flag = 0;
    if(comma_count != 0) { // If any comma found
        printf("ERROR: Can not input comma\n");
        *error_flag = 1;
    }
    else{
        for (int i = 0; i < 20; i++) {
            table *tmp = code[i];
            if(tmp == NULL) continue;
            for(; tmp != NULL; tmp = tmp->next) {
                if(strcmp(string, tmp->opcode) == 0) {
                    printf("opcode is %X\n", tmp->value);
                    flag = 1;
                }
            }
        }
        if(flag != 1) {
            printf("ERROR: Wrong opcode command\n");
            *error_flag = 1;
        }
        sprintf(cmd,"opcode %s\n", string);
    }
}

/*------------------------------------------------------------------------------------*/
/* Function : print_opcode(table *code[]) */
/* Object : Print opcode list  */
/* Return Value : None */
/*------------------------------------------------------------------------------------*/
void print_opcode(table *code[]) {
    for (int i = 0; i < 20; i++) {
        table *tmp = code[i];
        printf("%.2d : ",i);
        if(tmp == NULL) printf("\n");
        for(; tmp != NULL; tmp = tmp->next) {
            if(tmp->next == NULL) printf("[%s,%X]\n",tmp->opcode, tmp->value); // If no more to print
            else printf("[%s,%X] -> ",tmp->opcode, tmp->value);
        }
    }
}

/*------------------------------------------------------------------------------------*/
/* Function : string_compare(char *cmd, char *str, int size) */
/* Object : Comparing strings by size  */
/* Return Value : error_flag */
/*------------------------------------------------------------------------------------*/
int string_compare(char *cmd, char *str, int size){
    int error_flag = 0;
    if(cmd[size] != ' ' && cmd[size] != '\n' && cmd[size] != '\t') error_flag = 1;
    else {
        for(int i = 0; i < size; i++) {
            if(cmd[i] == str[i]) continue;
            else error_flag = 1;
        }
    }
    return error_flag;
}

/*------------------------------------------------------------------------------------*/
/* Function : parsing(char *cmd, char *parse_a, char *parse_b, char *parse_c, int size) */
/* Object : parse the string after shell input command and return the count of comma  */
/* Return Value : comma_flag */
/*------------------------------------------------------------------------------------*/
int parsing(char *cmd, char *parse_a, char *parse_b, char *parse_c, int size, int *error_flag) {
    int comma_flag = 0, index = 0, index_a = 0;
    int j = 0;
    long length = strlen(cmd) - 1; // To get rid of '\n'
    for(int i = 0; i < length; i++) {
        if (cmd[i] == ','){
            comma_flag++; // Increase the comma count
            if (comma_flag == 1) index = i; // Index of first comma
            else index_a = i; // Index of second comma
        }
    }
    if (comma_flag == 0) {// only parse_a(before first comma) is saved
        for(int i = size + 1; i < length; i++){
            if(i == size + 1 && (cmd[i] == ' ' || cmd[i] == '\t')) { // If consecutive space input
                continue;
            }
            else if(cmd[i] == ' ' || cmd[i] == '\t') {
                printf("Error : Can not place more than one space when no comma\n");
                *error_flag = 1;
            }
            else{
                parse_a[j++] = cmd[i];
            }
        }
    }
    else if (comma_flag == 1) { // parse_a and parse_b(after first comma) saved
        for(int i = size + 1; i < index; i++){
            if(i == size + 1 && (cmd[i] == ' ' || cmd[i] == '\t')) {
                continue;
            }
            else if(cmd[i] == ' ' || cmd[i] == '\t') {
                continue;
            }
            else{
                parse_a[j++] = cmd[i];
            }
        }

        for(int i = index + 1; i < length; i++){
            if(cmd[i] == ' ' || cmd[i] ==  '\t') {
                index++;
                continue;
            }
            parse_b[i- (index + 1)] = cmd[i];
        }
    }
    else if(comma_flag == 2){ // parse_a, parse_b(after first comma and before second comma), and parse_c (after second comma) saved
        for(int i = size + 1; i < index; i++){
            if(i == size + 1 && (cmd[i] == ' ' || cmd[i] == '\t')) {
                continue;
            }
            else if(cmd[i] == ' ' || cmd[i] == '\t') {
                continue;
            }
            else{
                parse_a[j++] = cmd[i];
            }
        }
        for(int i = index + 1; i < index_a; i++){
            if(cmd[i] == ' ' || cmd[i] ==  '\t') {
                index++;
                continue;
            }
            parse_b[i- (index + 1)] = cmd[i];
        }
        for(int i = index_a + 1; i < length; i++){
            if(cmd[i] == ' ' || cmd[i] ==  '\t') {
                index_a++;
                continue;
            }
            parse_c[i- (index_a + 1)] = cmd[i];
        }
    }
    else{
        printf("ERROR : Maximum 2 comma allowed\n");
        return -1;
    }
    return comma_flag;
}

/*------------------------------------------------------------------------------------*/
/* Function : hashing(char *string) */
/* Object : Hash function to decrypt string  */
/* Return Value : result */
/*------------------------------------------------------------------------------------*/
int hashing(char *string) {
    int sum = 0, result;
    for(int i = 0; i < strlen(string); i++) {
        sum += string[i];
    }
    result = sum % 20; // Inorder to make hastable size of 20
    return result;
}

/*------------------------------------------------------------------------------------*/
/* Function : insert(node *list, char *cmd) */
/* Object : Inserting the node to history command linked list  */
/* Return Value : node* */
/*------------------------------------------------------------------------------------*/
node* insert(node *list, char *cmd){
    node *n = (node*)malloc(sizeof(node));
    node *temp = NULL;
    strcpy(n->txt, cmd);
    n->next = NULL;
    if(list == NULL) {
      list = n;
    }
    else{
      temp = list;
      while(temp->next != NULL){
        temp = temp->next;
      }
      temp->next = n;
    }
    return list;
}

/*------------------------------------------------------------------------------------*/
/* Function : ascii(int key) */
/* Object : print ascii code(32~126), else print "."  */
/* Return Value : node* */
/*------------------------------------------------------------------------------------*/
void ascii(int key){
    if(key >= 32 && key <= 126) printf("%c", key);
    else printf(".");
}

/*------------------------------------------------------------------------------------*/
/* Function : change_hex(char hex)*/
/* Object : Change hexadecimal char into int */
/* Return Value : node* */
/*------------------------------------------------------------------------------------*/
int change_hex(char hex){
    int x;
    if (hex >= '0' && hex <= '9') x = hex - '0';
    else if(hex >= 'a' && hex <= 'f') x = hex - 87;
    else if(hex >= 'A' && hex <= 'F') x = hex - 55;
    else {
        return -1;
    }
    return x;
}

/*------------------------------------------------------------------------------------*/
/* Function : change_hex_string_to_int(char *string, int *error_flag) */
/* Object : Change hexadecimal string into integer */
/* Return Value : node* */
/*------------------------------------------------------------------------------------*/
int change_hex_string_to_int(char *string, int *error_flag) {
    int result = 0;
    switch (strlen(string)) {
        case 1: // 0 ~ 15
            if (change_hex(string[0]) != -1)
                result = change_hex(string[0]);
            else {
                printf("Wrong Input\n");
                *error_flag = 1;
            }
            break;
        case 2: // 16 ~ 255
            if (change_hex(string[0]) != -1 && change_hex(string[1]) != -1)
                result = 16 * change_hex(string[0]) + change_hex(string[1]);
            else {
                printf("Wrong Input\n");
                *error_flag = 1;
            }
            break;
        case 3: // 256 ~ 16^3-1
            if (change_hex(string[0]) != -1 && change_hex(string[1]) != -1 && change_hex(string[2]) != -1)
                result = 16 * 16 * change_hex(string[0]) + 16 * change_hex(string[1]) + change_hex(string[2]);
            else {
                printf("Wrong Input\n");
                *error_flag = 1;
            }
            break;
        case 4: // 16^3 ~ 16^4-1
            if (change_hex(string[0]) != -1 && change_hex(string[1]) != -1 && change_hex(string[2]) != -1 && change_hex(string[3]) != -1)
                result = 16 * 16 * 16 * change_hex(string[0]) + 16 * 16 * change_hex(string[1]) + 16 * change_hex(string[2]) + change_hex(string[3]);
            else {
                printf("Wrong Input\n");
                *error_flag = 1;
            }
            break;
        case 5: // 16^4 ~ 16^5 -1
            if (change_hex(string[0]) != -1 && change_hex(string[1]) != -1 && change_hex(string[2]) != -1 && change_hex(string[3]) != -1 && change_hex(string[4]) != -1)
                result = 16 * 16 * 16 * 16 * change_hex(string[0]) + 16 * 16 * 16 * change_hex(string[1]) + 16 * 16 * change_hex(string[2]) + 16 * change_hex(string[3]) + change_hex(string[4]);
            else {
                printf("Wrong Input\n");
                *error_flag = 1;
            }
            break;
        default:
            printf("WRONG HEXADECIMAL INPUT\n");
            *error_flag = 1;
            break;
    }
    return result;
}

/*------------------------------------------------------------------------------------*/
/* Function : print_dump(BYTE memory[], int address) */
/* Object : print memory line of 10 */
/* Return Value : node* */
/*------------------------------------------------------------------------------------*/
void print_dump(BYTE memory[], int address){
    for(int i = address; i < address + 160; i+=16){
        if( i >= MB) break;
        printf("%.5X ", i); // Printing the address
        for(int j = 0 ; j < 16; j++){
            printf("%.2X ", memory[i + j]);
        }
        printf("; ");
        for(int j = 0 ; j < 16; j++){
            if(i + j >= MB) break;
            ascii(memory[i + j]);
        }
        printf("\n");
    }
}

/*------------------------------------------------------------------------------------*/
/* Function : print_dump_start_end(BYTE memory[], int start, int end) */
/* Object : print memory from start to end*/
/* Return Value : node* */
/*------------------------------------------------------------------------------------*/
void print_dump_start_end(BYTE memory[], int start, int end){
    int quotient_start, quotient_end;
    quotient_start = start / 16;
    quotient_end = end / 16;

    for(int i = quotient_start * 16; i < (quotient_end + 1) * 16; i++){
        if (i >= MB) break;
        if (i % 16 == 0) printf("%.5X ", i); // Printing the address
        if (i < start || i > end) printf("   "); // Fill the empty with space
        else printf("%.2X ", memory[i]);
        if ((i + 1) % 16 == 0) {
            printf("; ");
            for(int j = 15 ; j >= 0; j--){
                if(i >= MB) break;
                ascii(memory[i - j]);
            }
            printf("\n");
        }
    }
}
