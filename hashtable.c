// hashtable.cpp: 定义应用程序的入口点。
//

#include "hashtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_SIZE 10
#define MULTIPLIER 31

// 哈希表节点定义
typedef struct Node {
    char* key;
    int value;
    struct Node* next;
} Node;

// 哈希表定义
typedef struct {
    int size;       // 当前哈希表的大小
    int count;      // 当前哈希表中节点的数量
    Node** table;   // 哈希表数组
} HashTable;

static void resizeHashTable(HashTable* hashtable, int new_size);

// 创建哈希表
HashTable* createHashTable() {
    HashTable* hashtable = (HashTable*)malloc(sizeof(HashTable));
    if (hashtable == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    hashtable->size = INITIAL_SIZE;
    hashtable->count = 0;
    hashtable->table = (Node**)calloc(hashtable->size, sizeof(Node*));
    if (hashtable->table == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return hashtable;
}

// 计算哈希值
unsigned int hash(const char* key) {
    unsigned int hash_value = 0;
    while (*key) {
        hash_value = hash_value * MULTIPLIER + *key++;
    }
    return hash_value;
}

// 获取哈希表中的节点
Node* getFromHashTable(HashTable* hashtable, const char* key) {
    unsigned int index = hash(key) % hashtable->size;
    Node* node = hashtable->table[index];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

// 向哈希表中插入节点
void insertIntoHashTable(HashTable* hashtable, const char* key, int value) {
    unsigned int index = hash(key) % hashtable->size;
    Node* node = getFromHashTable(hashtable, key);
    if (node != NULL) {
        // 如果键已存在，则更新其对应的值
        node->value = value;
        return;
    }

    // 如果键不存在，则插入新节点到哈希链表头部
    node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    node->key = strdup(key);
    node->value = value;
    node->next = hashtable->table[index];
    hashtable->table[index] = node;
    hashtable->count++;

    // 检查是否需要扩展哈希表
    if (hashtable->count > hashtable->size * 2) {
        resizeHashTable(hashtable, hashtable->size * 2);
    }
}

// 从哈希表中移除节点
void removeFromHashTable(HashTable* hashtable, const char* key) {
    unsigned int index = hash(key) % hashtable->size;
    Node* node = hashtable->table[index];
    Node* prev = NULL;
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            if (prev == NULL) {
                hashtable->table[index] = node->next;
            }
            else {
                prev->next = node->next;
            }
            free(node->key);
            free(node);
            hashtable->count--;
            return;
        }
        prev = node;
        node = node->next;
    }
}

// 调整哈希表的大小
static void resizeHashTable(HashTable* hashtable, int new_size) {
    Node** new_table = (Node**)calloc(new_size, sizeof(Node*));
    if (new_table == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // 将原哈希表中的节点重新插入到新哈希表中
    for (int i = 0; i < hashtable->size; ++i) {
        Node* node = hashtable->table[i];
        while (node != NULL) {
            Node* next = node->next;
            unsigned int index = hash(node->key) % new_size;
            node->next = new_table[index];
            new_table[index] = node;
            node = next;
        }
    }

    // 释放原哈希表的内存，更新哈希表的大小和数组
    free(hashtable->table);
    hashtable->table = new_table;
    hashtable->size = new_size;
}

// 销毁哈希表
void destroyHashTable(HashTable* hashtable) {
    for (int i = 0; i < hashtable->size; ++i) {
        Node* node = hashtable->table[i];
        while (node != NULL) {
            Node* next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }
    free(hashtable->table);
    free(hashtable);
}

// 遍历哈希表并执行回调函数
void traverseHashTable(HashTable* hashtable, void (*callback)(const char*, int)) {
    for (int i = 0; i < hashtable->size; ++i) {
        Node* node = hashtable->table[i];
        while (node != NULL) {
            callback(node->key, node->value);
            node = node->next;
        }
    }
}

// 回调函数示例
void printKeyValue(const char* key, int value) {
    printf("Key: %s, Value: %d\n", key, value);
}

int main(void) {
    HashTable* hashtable = createHashTable();

    // 插入节点
    insertIntoHashTable(hashtable, "key1", 10);
    insertIntoHashTable(hashtable, "key2", 20);
    insertIntoHashTable(hashtable, "key3", 30);

    // 遍历哈希表并打印节点
    printf("Hashtable contents:\n");
    traverseHashTable(hashtable, printKeyValue);

    // 从哈希表中移除节点
    removeFromHashTable(hashtable, "key2");

    // 再次遍历哈希表并打印节点
    printf("Hashtable contents after removal:\n");
    traverseHashTable(hashtable, printKeyValue);

    // 销毁哈希表
    destroyHashTable(hashtable);

    return 0;
}
