#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============ hw0 原始程式碼(以鏈結串列紀錄字元出現次數) ============ //
typedef struct Node {
    char character;
    int count;
    struct Node *next;
} Node;

// 新增或更新字元計數的節點
Node* addNode(Node *head, char c) {
    Node *temp = head;
    while (temp) {
        if (temp->character == c) {
            temp->count++;
            return head;
        }
        temp = temp->next;
    }
    
    // 若目前串列沒有該字元，則新建一個節點並插入串列開頭
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->character = c;
    newNode->count = 1;
    newNode->next = head;
    return newNode;
}

// 釋放整個鏈結串列
void freeList(Node *head) {
    Node *temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// ============ Scanner 相關函式 ============ //

// (不使用 ctype.h) 自行判斷是否為英文字母
int isAlpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

// 判斷是否為數字
int isDigit(char c) {
    return (c >= '0' && c <= '9');
}

// 判斷是否為英數或底線
int isAlnumOrUnderscore(char c) {
    return (isAlpha(c) || isDigit(c) || c == '_');
}

// Token 輸出函式
void printToken(const char *lexeme, const char *tokenType) {
    // 例如：int: TYPE_TOKEN
    printf("%s: %s\n", lexeme, tokenType);
}

int main() {
    const char *targetFile = "Token.c";

    // ============== 第一階段：hw0 功能 ============== //
    // 打開 Token.c 並計算每個字元的出現次數
    FILE *file = fopen(targetFile, "r");
    if (!file) {
        fprintf(stderr, "Failed to open %s.\n", targetFile);
        return 1;
    }

    Node *head = NULL;
    char c;
    while ((c = fgetc(file)) != EOF) {
        head = addNode(head, c);
    }
    fclose(file);

    // 印出 Token.c 裡每個字元的出現次數
    Node *temp = head;
    while (temp) {
        // 若要忽略換行或空白等字元的印出，可自行加判斷
        printf("%c : %d\n", temp->character, temp->count);
        temp = temp->next;
    }
    freeList(head);

    // ============== 第二階段：Scanner 辨識 Token ============== //
    // 再次打開 Token.c
    file = fopen(targetFile, "r");
    if (!file) {
        fprintf(stderr, "Failed to open %s on second pass.\n", targetFile);
        return 1;
    }

    while ((c = fgetc(file)) != EOF) {
        // 1. 忽略空白字元
        if (c == ' ' || c == '\n' || c == '\t') {
            continue;
        }

        // 2. 英文字母或底線開頭 => 可能是關鍵字或 ID
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
                // 其他情況皆視為 ID
                printToken(buffer, "ID_TOKEN");
            }
        }
        // 3. 數字 => 讀取為整數常數
        else if (isDigit(c)) {
            char buffer[256];
            int idx = 0;
            buffer[idx++] = c;

            while ((c = fgetc(file)) != EOF && isDigit(c)) {
                buffer[idx++] = c;
            }
            if (c != EOF) {
                ungetc(c, file);
            }
            buffer[idx] = '\0';

            printToken(buffer, "LITERAL_TOKEN");
        }
        // 4. 其他符號 (包含 relop 與其餘單字元 Token)
        else {
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
                // 不在規格內的字元 => 忽略或自行處理
            }
        }
    }

    fclose(file);
    return 0;
}
