#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//============= hw0 原始程式碼(讀取自身檔案並計算字元次數) =============//
typedef struct Node {
    char character;
    int count;
    struct Node *next;
} Node;

Node* addNode(Node *head, char c) {
    Node *temp = head;
    while (temp) {
        if (temp->character == c) {
            temp->count++;
            return head;
        }
        temp = temp->next;
    }
    
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->character = c;
    newNode->count = 1;
    newNode->next = head;
    return newNode;
}

void freeList(Node *head) {
    Node *temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

//============= Scanner 相關函式/巨集定義 =============//

// 判斷是否為英文字母(不使用 ctype.h，自己寫)
int isAlpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

// 判斷是否為數字
int isDigit(char c) {
    return (c >= '0' && c <= '9');
}

// 判斷是否為英數或底線(可用來處理識別字)
int isAlnumOrUnderscore(char c) {
    return (isAlpha(c) || isDigit(c) || c == '_');
}

// 輸出 Token
void printToken(const char *lexeme, const char *tokenType) {
    // 輸出格式：lexeme: TOKEN_TYPE
    printf("%s: %s\n", lexeme, tokenType);
}

int main() {
    //============ 第一階段：hw0：計算字元出現次數 ============//
    FILE *file = fopen(__FILE__, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file.\n");
        return 1;
    }
    
    Node *head = NULL;
    char c;
    while ((c = fgetc(file)) != EOF) {
        head = addNode(head, c);
    }
    fclose(file);
    
    // 列印所有字元出現次數
    Node *temp = head;
    while (temp) {
        printf("%c : %d\n", temp->character, temp->count);
        temp = temp->next;
    }
    
    // 釋放鏈結串列
    freeList(head);

    //============ 第二階段：Scanner：辨識 Token ============//
    file = fopen(__FILE__, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file (second pass).\n");
        return 1;
    }

    while ((c = fgetc(file)) != EOF) {
        // 1. 忽略空白字元
        if (c == ' ' || c == '\n' || c == '\t') {
            continue;
        }

        // 2. 如果是英文字母或底線開頭，則可能是關鍵字或 ID
        if (isAlpha(c) || c == '_') {
            char buffer[256];
            int idx = 0;
            buffer[idx++] = c;

            // 讀取後續英數或底線
            while ((c = fgetc(file)) != EOF && isAlnumOrUnderscore(c)) {
                buffer[idx++] = c;
            }
            // 回退多讀的字元
            if (c != EOF) {
                ungetc(c, file);
            }
            buffer[idx] = '\0';

            // 判斷是否為關鍵字
            if (strcmp(buffer, "int") == 0) {
                printToken(buffer, "TYPE_TOKEN");
            } else if (strcmp(buffer, "main") == 0) {
                printToken(buffer, "MAIN_TOKEN");
            } else if (strcmp(buffer, "if") == 0) {
                printToken(buffer, "IF_TOKEN");
            } else if (strcmp(buffer, "else") == 0) {
                printToken(buffer, "ELSE_TOKEN");
            } else if (strcmp(buffer, "while") == 0) {
                printToken(buffer, "WHILE_TOKEN");
            } else {
                // 其他皆視為 ID
                printToken(buffer, "ID_TOKEN");
            }
        }
        // 3. 如果是數字開頭，則視為整數常數
        else if (isDigit(c)) {
            char buffer[256];
            int idx = 0;
            buffer[idx++] = c;

            // 連續讀取所有數字
            while ((c = fgetc(file)) != EOF && isDigit(c)) {
                buffer[idx++] = c;
            }
            if (c != EOF) {
                ungetc(c, file);
            }
            buffer[idx] = '\0';

            printToken(buffer, "LITERAL_TOKEN");
        }
        // 4. 否則判斷各種運算子與界符
        else {
            // 先處理可能的兩字元運算子(==, >=, <=)
            if (c == '=') {
                char nxt = fgetc(file);
                if (nxt == '=') {
                    printToken("==", "EQUAL_TOKEN");
                } else {
                    if (nxt != EOF) ungetc(nxt, file);
                    printToken("=", "ASSIGN_TOKEN");
                }
            }
            else if (c == '>') {
                char nxt = fgetc(file);
                if (nxt == '=') {
                    printToken(">=", "GREATEREQUAL_TOKEN");
                } else {
                    if (nxt != EOF) ungetc(nxt, file);
                    printToken(">", "GREATER_TOKEN");
                }
            }
            else if (c == '<') {
                char nxt = fgetc(file);
                if (nxt == '=') {
                    printToken("<=", "LESSEQUAL_TOKEN");
                } else {
                    if (nxt != EOF) ungetc(nxt, file);
                    printToken("<", "LESS_TOKEN");
                }
            }
            else if (c == '+') {
                printToken("+", "PLUS_TOKEN");
            }
            else if (c == '-') {
                printToken("-", "MINUS_TOKEN");
            }
            else if (c == '(') {
                printToken("(", "LEFTPAREN_TOKEN");
            }
            else if (c == ')') {
                printToken(")", "RIGHTPAREN_TOKEN");
            }
            else if (c == '{') {
                printToken("{", "LEFTBRACE_TOKEN");
            }
            else if (c == '}') {
                printToken("}", "RIGHTBRACE_TOKEN");
            }
            else if (c == ';') {
                printToken(";", "SEMICOLON_TOKEN");
            }
            else {
                // 其他字元(不在規格內)就忽略或自行處理
            }
        }
    }

    fclose(file);
    return 0;
}
