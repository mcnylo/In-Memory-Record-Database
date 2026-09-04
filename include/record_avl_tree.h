#ifndef RECORD_AVL_TREE_H
#define RECORD_AVL_TREE_H

#include "record.h"

// ================================================================================================

typedef int (*RecordComparator)(const struct Record *, const struct Record *);

// ================================================================================================

enum InsertResult
{
    INSERT_SUCCESS,
    INSERT_MEMORY_ERROR,
    INSERT_DUPLICATE,
    INSERT_FAILED_GENERIC,
    INSERT_FAILED_CORRUPTED,
    INSERT_ROLLBACK_SUCCESS
};

enum DeleteResult
{
    DELETE_SUCCESS,
    DELETE_FAILED,
    DELETE_CORRUPTED,
    DELETE_CANCELED
};

enum EditResult
{
    EDIT_SUCCESS,
    EDIT_FAILED,
    EDIT_CORRUPTED,
    EDIT_CANCELED
};

// ================================================================================================

struct RecordNode
{
    struct Record *record;
    struct RecordNode *left;
    struct RecordNode *right;
    int nodeHeight;
};

struct RecordAVLTree
{
    struct RecordNode *root;
    RecordComparator comparator;
};

// ================================================================================================

int getNodeHeight(struct RecordNode *recordNode);
int getBalanceFactor(struct RecordNode *recordNode);
int max(int a, int b);

struct RecordNode *createNode(struct Record *record);
struct RecordNode *rightRotate(struct RecordNode *recordNode);
struct RecordNode *leftRotate(struct RecordNode *recordNode);
struct RecordNode *minValueRecordNode(struct RecordNode *rootRecordNode);
struct RecordNode *rebalanceTree(struct RecordNode *root);

struct Record *findRecord(const struct RecordAVLTree *tree, const struct Record *recordToFind);

enum InsertResult insert(struct RecordAVLTree *tree, struct Record *record);
enum DeleteResult deleteNode(struct RecordAVLTree *tree, const struct Record *recordToDelete);

void destroyTreeNodesOnly(struct RecordAVLTree *tree);
void destroyTreeNodesAndRecords(struct RecordAVLTree *tree);

int compareById(const struct Record *recordA, const struct Record *recordB);
int compareByFirstName(const struct Record *recordA, const struct Record *recordB);
int compareByLastName(const struct Record *recordA, const struct Record *recordB);
int compareByAge(const struct Record *recordA, const struct Record *recordB);

// ================================================================================================

#endif