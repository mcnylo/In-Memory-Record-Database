#include "../include/record_avl_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================================================================================

static struct RecordNode *insertInternal(struct RecordNode *root, struct Record *recordToInsert, RecordComparator comparator, enum InsertResult *result)
{
    // Create root if it doesn't exist
    if (root == NULL)
    {
        struct RecordNode *newRecordNode = createNode(recordToInsert);

        if (newRecordNode == NULL)
        {
            *result = INSERT_MEMORY_ERROR;

            return NULL;
        }
        else
        {
            root = newRecordNode;

            *result = INSERT_SUCCESS;

            return root;
        }
    }

    int recordCompareResult = comparator(recordToInsert, root->record);

    // If root does exist, figure out where it belongs
    // Duplicate IDs are not allowed since we want unique IDs
    if (recordCompareResult < 0)
    {
        root->left = insertInternal(root->left, recordToInsert, comparator, result);
    }
    else if (recordCompareResult > 0)
    {
        root->right = insertInternal(root->right, recordToInsert, comparator, result);
    }
    else
    {
        *result = INSERT_DUPLICATE;
    }

    root = rebalanceTree(root);

    return root;
}

static struct Record *findRecordInternal(const struct RecordNode *root, const struct Record *recordToFind, RecordComparator comparator)
{
    struct Record *record;

    if (root == NULL)
    {
        return NULL;
    }

    int compareResult = comparator(recordToFind, root->record);

    if (compareResult < 0)
    {
        record = findRecordInternal(root->left, recordToFind, comparator);
    }
    else if (compareResult > 0)
    {
        record = findRecordInternal(root->right, recordToFind, comparator);
    }
    else
    {
        record = root->record;
    }

    return record;
}

static struct RecordNode *deleteNodeInternal(struct RecordNode *root, const struct Record *recordToDelete, RecordComparator comparator, enum DeleteResult *result)
{
    if (root == NULL)
    {
        *result = DELETE_FAILED;

        return NULL;
    }

    int recordCompareResult = comparator(recordToDelete, root->record);

    if (recordCompareResult < 0)
    {
        root->left = deleteNodeInternal(root->left, recordToDelete, comparator, result);
    }
    else if (recordCompareResult > 0)
    {
        root->right = deleteNodeInternal(root->right, recordToDelete, comparator, result);
    }
    else
    {
        // Record has 1 child or no children records
        if ((root->left == NULL) || (root->right == NULL))
        {
            struct RecordNode *tempNode;

            // Record has a left child
            if (root->left != NULL)
            {
                tempNode = root->left;
            }
            // Record has a right child
            else
            {
                tempNode = root->right;
            }

            // No children were found for the record to delete
            if (tempNode == NULL)
            {
                tempNode = root;
                root = NULL;

                *result = DELETE_SUCCESS;

                free(tempNode);
            }
            // At least 1 child was found for the record to delete, return that child record
            else
            {
                free(root);

                *result = DELETE_SUCCESS;

                return tempNode;
            }
        }
        // Record has 2 children
        else
        {
            // Find minimum value
            struct RecordNode *tempNode = minValueRecordNode(root->right);

            // Overwrite the current root's record
            root->record = tempNode->record;

            *result = DELETE_SUCCESS;

            // Now, look for the duplicate record to delete. We just assigned this to the current root
            root->right = deleteNodeInternal(root->right, tempNode->record, comparator, result);
        }
    }

    root = rebalanceTree(root);

    return root;
}

static void destroyNodesOnly(struct RecordNode *root)
{
    if (root == NULL)
    {
        return;
    }

    destroyNodesOnly(root->left);
    destroyNodesOnly(root->right);

    free(root);
}

static void destroyNodesAndRecords(struct RecordNode *root)
{
    if (root == NULL)
    {
        return;
    }

    destroyNodesAndRecords(root->left);
    destroyNodesAndRecords(root->right);

    free(root->record);
    free(root);
}

// ================================================================================================

int getNodeHeight(struct RecordNode *recordNode)
{
    if (recordNode == NULL)
    {
        return 0;
    }

    return recordNode->nodeHeight;
}

int getBalanceFactor(struct RecordNode *recordNode)
{
    if (recordNode == NULL)
    {
        return 0;
    }

    return getNodeHeight(recordNode->left) - getNodeHeight(recordNode->right);
}

int max(int a, int b)
{
    if (a > b)
    {
        return a;
    }

    return b;
}

struct RecordNode *createNode(struct Record *record)
{
    struct RecordNode *recordNode = malloc(sizeof(*recordNode));

    if (recordNode == NULL)
    {
        return NULL;
    }

    recordNode->record = record;
    recordNode->left = NULL;
    recordNode->right = NULL;
    recordNode->nodeHeight = 1;

    return recordNode;
}

struct RecordNode *rightRotate(struct RecordNode *recordNode)
{
    struct RecordNode *leftRecordNode = recordNode->left;
    struct RecordNode *temp_LeftRecordNodesRightNode = leftRecordNode->right;

    // Perform rotation
    leftRecordNode->right = recordNode;
    recordNode->left = temp_LeftRecordNodesRightNode;

    // Update heights
    recordNode->nodeHeight = max(getNodeHeight(recordNode->left), getNodeHeight(recordNode->right)) + 1;
    leftRecordNode->nodeHeight = max(getNodeHeight(leftRecordNode->left), getNodeHeight(leftRecordNode->right)) + 1;

    return leftRecordNode;
}

struct RecordNode *leftRotate(struct RecordNode *recordNode)
{
    struct RecordNode *rightRecordNode = recordNode->right;
    struct RecordNode *temp_RightRecordNodesLeftNode = rightRecordNode->left;

    // Perform rotation
    rightRecordNode->left = recordNode;
    recordNode->right = temp_RightRecordNodesLeftNode;

    // Update heights
    recordNode->nodeHeight = max(getNodeHeight(recordNode->left), getNodeHeight(recordNode->right)) + 1;
    rightRecordNode->nodeHeight = max(getNodeHeight(rightRecordNode->left), getNodeHeight(rightRecordNode->right)) + 1;

    return rightRecordNode;
}

struct RecordNode *minValueRecordNode(struct RecordNode *rootRecordNode)
{
    struct RecordNode *current = rootRecordNode;

    while (current->left != NULL)
    {
        current = current->left;
    }

    return current;
}

struct RecordNode *rebalanceTree(struct RecordNode *root)
{
    if (root == NULL)
    {
        return root;
    }

    // Update height
    root->nodeHeight = 1 + max(getNodeHeight(root->left), getNodeHeight(root->right));

    // Check if tree is unbalanced
    int balance = getBalanceFactor(root);

    // LEFT HEAVY TREE
    // Left Left case
    if (balance > 1 && getBalanceFactor(root->left) >= 0)
    {
        root = rightRotate(root);
    }

    // Left Right case
    if (balance > 1 && getBalanceFactor(root->left) < 0)
    {
        root->left = leftRotate(root->left);

        root = rightRotate(root);
    }

    // RIGHT HEAVY TREE
    // Right Right case
    if (balance < -1 && getBalanceFactor(root->right) <= 0)
    {
        root = leftRotate(root);
    }

    // Right Left case
    if (balance < -1 && getBalanceFactor(root->right) > 0)
    {
        root->right = rightRotate(root->right);

        root = leftRotate(root);
    }

    return root;
}

struct Record *findRecord(const struct RecordAVLTree *tree, const struct Record *recordToFind)
{
    return findRecordInternal(tree->root, recordToFind, tree->comparator);
}

enum InsertResult insert(struct RecordAVLTree *tree, struct Record *record)
{
    enum InsertResult result = INSERT_SUCCESS;

    tree->root = insertInternal(tree->root, record, tree->comparator, &result);

    return result;
}

enum DeleteResult deleteNode(struct RecordAVLTree *tree, const struct Record *recordToDelete)
{
    enum DeleteResult result = DELETE_FAILED;

    tree->root = deleteNodeInternal(tree->root, recordToDelete, tree->comparator, &result);

    return result;
}

void destroyTreeNodesOnly(struct RecordAVLTree *tree)
{
    destroyNodesOnly(tree->root);
    tree->root = NULL;
}

void destroyTreeNodesAndRecords(struct RecordAVLTree *tree)
{
    destroyNodesAndRecords(tree->root);
    tree->root = NULL;
}

int compareById(const struct Record *recordA, const struct Record *recordB)
{
    if (recordA->id < recordB->id)
    {
        return -1;
    }
    else if (recordA->id == recordB->id)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int compareByFirstName(const struct Record *recordA, const struct Record *recordB)
{
    int result = strcmp(recordA->firstName, recordB->firstName);

    if (result < 0)
    {
        return -1;
    }
    else if (result == 0)
    {
        return compareById(recordA, recordB);
    }
    else
    {
        return 1;
    }
}

int compareByLastName(const struct Record *recordA, const struct Record *recordB)
{
    int result = strcmp(recordA->lastName, recordB->lastName);

    if (result < 0)
    {
        return -1;
    }
    else if (result == 0)
    {
        return compareById(recordA, recordB);
    }
    else
    {
        return 1;
    }
}

int compareByAge(const struct Record *recordA, const struct Record *recordB)
{
    if (recordA->age < recordB->age)
    {
        return -1;
    }
    else if (recordA->age == recordB->age)
    {
        return compareById(recordA, recordB);
    }
    else
    {
        return 1;
    }
}