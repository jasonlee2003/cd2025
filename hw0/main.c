#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main() {
    FILE *file = fopen(__FILE__, "r"); 
    if (!file) {
        return 1;
    }
    
    Node *head = NULL;
    char c;
    while ((c = fgetc(file)) != EOF) {
        head = addNode(head, c);
    }
    fclose(file);
    
    Node *temp = head;
    while (temp) {
        printf("%c : %d\n", temp->character, temp->count);
        temp = temp->next;
    }
    
    freeList(head);
    return 0;
}
