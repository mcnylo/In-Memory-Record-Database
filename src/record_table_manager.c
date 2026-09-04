#include "../include/record_table_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================================================================================

static enum InsertResult treeRollback(struct RecordAVLTree *tree, struct Record *record)
{
    enum DeleteResult treeRollback = deleteNode(tree, record);

    if (treeRollback != DELETE_SUCCESS)
    {
        printf("*** ERROR: DATABASE TREES CORRUPTED ***\n");

        return INSERT_FAILED_CORRUPTED;
    }

    return INSERT_ROLLBACK_SUCCESS;
}

static int countNodes(const struct RecordNode *root)
{
    if (root == NULL)
    {
        return 0;
    }

    int leftCount = countNodes(root->left);
    int rightCount = countNodes(root->right);

    return leftCount + rightCount + 1;
}

static void collectNodesInOrder(const struct RecordNode *root, struct SearchResults *results, int *index)
{
    if (root == NULL)
    {
        return;
    }

    collectNodesInOrder(root->left, results, index);

    results->records[*index] = root->record;
    (*index)++;

    collectNodesInOrder(root->right, results, index);
}

static void countFirstNameMatches(const struct RecordNode *root, const char *firstName, int *count)
{
    if (root == NULL)
    {
        return;
    }

    int comparison = strcmp(firstName, root->record->firstName);

    if (comparison < 0)
    {
        countFirstNameMatches(root->left, firstName, count);
    }
    else if (comparison > 0)
    {
        countFirstNameMatches(root->right, firstName, count);
    }
    else
    {
        // Pre-order traversal since we're only getting a count
        (*count)++;

        countFirstNameMatches(root->left, firstName, count);
        countFirstNameMatches(root->right, firstName, count);
    }
}

static void countLastNameMatches(const struct RecordNode *root, const char *lastName, int *count)
{
    if (root == NULL)
    {
        return;
    }

    int comparison = strcmp(lastName, root->record->lastName);

    if (comparison < 0)
    {
        countLastNameMatches(root->left, lastName, count);
    }
    else if (comparison > 0)
    {
        countLastNameMatches(root->right, lastName, count);
    }
    else
    {
        (*count)++;

        countLastNameMatches(root->left, lastName, count);
        countLastNameMatches(root->right, lastName, count);
    }
}

static void countAgeMatches(const struct RecordNode *root, const int age, int *count)
{
    if (root == NULL)
    {
        return;
    }

    if (age < root->record->age)
    {
        countAgeMatches(root->left, age, count);
    }
    else if (age > root->record->age)
    {
        countAgeMatches(root->right, age, count);
    }
    else
    {
        (*count)++;

        countAgeMatches(root->left, age, count);
        countAgeMatches(root->right, age, count);
    }
}

static void collectFirstNameMatches(const struct RecordNode *root, const char *firstName, struct SearchResults *results, int *index)
{
    if (root == NULL)
    {
        return;
    }

    int comparison = strcmp(firstName, root->record->firstName);

    if (comparison < 0)
    {
        collectFirstNameMatches(root->left, firstName, results, index);
    }
    else if (comparison > 0)
    {
        collectFirstNameMatches(root->right, firstName, results, index);
    }
    else
    {
        // In-order traversal so the IDs can be in order
        collectFirstNameMatches(root->left, firstName, results, index);

        results->records[*index] = root->record;

        (*index)++;

        collectFirstNameMatches(root->right, firstName, results, index);
    }
}

static void collectLastNameMatches(const struct RecordNode *root, const char *lastName, struct SearchResults *results, int *index)
{
    if (root == NULL)
    {
        return;
    }

    int comparison = strcmp(lastName, root->record->lastName);

    if (comparison < 0)
    {
        collectLastNameMatches(root->left, lastName, results, index);
    }
    else if (comparison > 0)
    {
        collectLastNameMatches(root->right, lastName, results, index);
    }
    else
    {
        // In-order traversal so the IDs can be in order
        collectLastNameMatches(root->left, lastName, results, index);

        results->records[*index] = root->record;

        (*index)++;

        collectLastNameMatches(root->right, lastName, results, index);
    }
}

static void collectAgeMatches(const struct RecordNode *root, const int age, struct SearchResults *results, int *index)
{
    if (root == NULL)
    {
        return;
    }

    if (age < root->record->age)
    {
        collectAgeMatches(root->left, age, results, index);
    }
    else if (age > root->record->age)
    {
        collectAgeMatches(root->right, age, results, index);
    }
    else
    {
        // In-order traversal so the IDs can be in order
        collectAgeMatches(root->left, age, results, index);

        results->records[*index] = root->record;

        (*index)++;

        collectAgeMatches(root->right, age, results, index);
    }
}

// ================================================================================================

void initializeRecordTable(struct RecordTable *table)
{
    table->idTree.root = NULL;
    table->idTree.comparator = compareById;

    table->firstNameTree.root = NULL;
    table->firstNameTree.comparator = compareByFirstName;

    table->lastNameTree.root = NULL;
    table->lastNameTree.comparator = compareByLastName;

    table->ageTree.root = NULL;
    table->ageTree.comparator = compareByAge;
}

void destroyRecordTable(struct RecordTable *table)
{
    destroyTreeNodesOnly(&table->ageTree);
    destroyTreeNodesOnly(&table->lastNameTree);
    destroyTreeNodesOnly(&table->firstNameTree);

    destroyTreeNodesAndRecords(&table->idTree);
}

void printSortedRecords(const struct SearchResults *results)
{
    if (results->records == NULL)
    {
        printf(">>> No records to view <<<\n");

        return;
    }

    printf("\n%-10s | %-10s | %-10s | %-10s\n", "ID", "FirstName", "LastName", "Age");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < results->count; i++)
    {
        printf("\n%-10d | %-10.10s | %-10.10s | %-10d\n", results->records[i]->id, results->records[i]->firstName,
               results->records[i]->lastName, results->records[i]->age);
    }

    printf("--------------------------------------------------\n");
}

void freeSearchResults(struct SearchResults *results)
{
    free(results->records);

    results->records = NULL;
    results->count = 0;
}

int getNextId(const struct RecordTable *table)
{
    if (table->idTree.root == NULL)
    {
        return 0;
    }

    struct SearchResults results = {0};

    enum SearchResultStatus recordsSorted = listRecordsSorted(table, SORT_ID, &results);

    if (recordsSorted == SEARCH_SUCCESS)
    {
        int nodeCount = results.count;
        int nextId = results.records[nodeCount - 1]->id + 1;

        freeSearchResults(&results);

        return nextId;
    }

    freeSearchResults(&results);

    return -1;
}

struct Record *findRecordById(const struct RecordTable *table, int id)
{
    return findRecord(&table->idTree, &(struct Record){.id = id});
}

enum InsertResult createRecord(struct RecordTable *table, const char *firstName, const char *lastName, int age)
{
    int nextId = getNextId(table);

    if (nextId == -1)
    {
        return INSERT_FAILED_GENERIC;
    }

    struct Record *record = malloc(sizeof(*record));

    if (record == NULL)
    {
        return INSERT_MEMORY_ERROR;
    }

    record->id = nextId;
    strcpy(record->firstName, firstName);
    strcpy(record->lastName, lastName);
    record->age = age;

    enum InsertResult idTreeInsert = insert(&table->idTree, record);

    if (idTreeInsert != INSERT_SUCCESS)
    {
        free(record);

        return idTreeInsert;
    }

    enum InsertResult firstNameTreeInsert = insert(&table->firstNameTree, record);

    if (firstNameTreeInsert != INSERT_SUCCESS)
    {
        enum InsertResult idRollback = treeRollback(&table->idTree, record);

        if (idRollback == INSERT_FAILED_CORRUPTED)
        {
            return INSERT_FAILED_CORRUPTED;
        }

        free(record);

        return firstNameTreeInsert;
    }

    enum InsertResult lastNameTreeInsert = insert(&table->lastNameTree, record);

    if (lastNameTreeInsert != INSERT_SUCCESS)
    {
        enum InsertResult firstNameRollback = treeRollback(&table->firstNameTree, record);

        if (firstNameRollback == INSERT_FAILED_CORRUPTED)
        {
            return INSERT_FAILED_CORRUPTED;
        }

        enum InsertResult idRollback = treeRollback(&table->idTree, record);

        if (idRollback == INSERT_FAILED_CORRUPTED)
        {
            return INSERT_FAILED_CORRUPTED;
        }

        free(record);

        return lastNameTreeInsert;
    }

    enum InsertResult ageTreeInsert = insert(&table->ageTree, record);

    if (ageTreeInsert != INSERT_SUCCESS)
    {
        enum InsertResult lastNameRollback = treeRollback(&table->lastNameTree, record);

        if (lastNameRollback == INSERT_FAILED_CORRUPTED)
        {
            return INSERT_FAILED_CORRUPTED;
        }

        enum InsertResult firstNameRollback = treeRollback(&table->firstNameTree, record);

        if (firstNameRollback == INSERT_FAILED_CORRUPTED)
        {
            return INSERT_FAILED_CORRUPTED;
        }

        enum InsertResult idRollback = treeRollback(&table->idTree, record);

        if (idRollback == INSERT_FAILED_CORRUPTED)
        {
            return INSERT_FAILED_CORRUPTED;
        }

        free(record);

        return ageTreeInsert;
    }

    return INSERT_SUCCESS;
}

enum DeleteResult deleteRecord(struct RecordTable *table, int id)
{
    struct Record *recordToDelete = findRecordById(table, id);

    if (recordToDelete == NULL)
    {
        return DELETE_FAILED;
    }

    struct Record *firstNameRecord = findRecord(&table->firstNameTree, recordToDelete);
    struct Record *lastNameRecord = findRecord(&table->lastNameTree, recordToDelete);
    struct Record *ageRecord = findRecord(&table->ageTree, recordToDelete);

    if (firstNameRecord != recordToDelete || lastNameRecord != recordToDelete || ageRecord != recordToDelete)
    {
        return DELETE_CORRUPTED;
    }

    enum DeleteResult idTreeDeleteResult = deleteNode(&table->idTree, recordToDelete);
    enum DeleteResult firstNameTreeDeleteResult = deleteNode(&table->firstNameTree, recordToDelete);
    enum DeleteResult lastNameTreeDeleteResult = deleteNode(&table->lastNameTree, recordToDelete);
    enum DeleteResult ageTreeDeleteResult = deleteNode(&table->ageTree, recordToDelete);

    if (idTreeDeleteResult == DELETE_SUCCESS && firstNameTreeDeleteResult == DELETE_SUCCESS &&
        lastNameTreeDeleteResult == DELETE_SUCCESS && ageTreeDeleteResult == DELETE_SUCCESS)
    {
        free(recordToDelete);

        return DELETE_SUCCESS;
    }

    return DELETE_FAILED;
}

enum EditResult editFirstName(struct RecordTable *table, int id, const char *newFirstName)
{
    struct Record *recordToEdit = findRecordById(table, id);

    if (recordToEdit == NULL)
    {
        return EDIT_FAILED;
    }

    if (strlen(newFirstName) > 100 || newFirstName == NULL)
    {
        return EDIT_FAILED;
    }

    if (strcmp(recordToEdit->firstName, newFirstName) == 0)
    {
        return EDIT_SUCCESS;
    }

    char oldFirstName[101];

    strcpy(oldFirstName, recordToEdit->firstName);

    enum DeleteResult deleteResult = deleteNode(&table->firstNameTree, recordToEdit);

    if (deleteResult == DELETE_FAILED)
    {
        return EDIT_CORRUPTED;
    }

    strcpy(recordToEdit->firstName, newFirstName);

    enum InsertResult insertResult = insert(&table->firstNameTree, recordToEdit);

    if (insertResult != INSERT_SUCCESS)
    {
        strcpy(recordToEdit->firstName, oldFirstName);

        enum InsertResult rollbackInsert = insert(&table->firstNameTree, recordToEdit);

        if (rollbackInsert != INSERT_SUCCESS)
        {
            return EDIT_CORRUPTED;
        }

        return EDIT_FAILED;
    }

    return EDIT_SUCCESS;
}

enum EditResult editLastName(struct RecordTable *table, int id, const char *newLastName)
{
    struct Record *recordToEdit = findRecordById(table, id);

    if (recordToEdit == NULL)
    {
        return EDIT_FAILED;
    }

    if (strlen(newLastName) > 100 || newLastName == NULL)
    {
        return EDIT_FAILED;
    }

    if (strcmp(recordToEdit->lastName, newLastName) == 0)
    {
        return EDIT_SUCCESS;
    }

    char oldLastName[101];

    strcpy(oldLastName, recordToEdit->lastName);

    enum DeleteResult deleteResult = deleteNode(&table->lastNameTree, recordToEdit);

    if (deleteResult == DELETE_FAILED)
    {
        return EDIT_CORRUPTED;
    }

    strcpy(recordToEdit->lastName, newLastName);

    enum InsertResult insertResult = insert(&table->lastNameTree, recordToEdit);

    if (insertResult != INSERT_SUCCESS)
    {
        strcpy(recordToEdit->lastName, oldLastName);

        enum InsertResult rollbackInsert = insert(&table->lastNameTree, recordToEdit);

        if (rollbackInsert != INSERT_SUCCESS)
        {
            return EDIT_CORRUPTED;
        }

        return EDIT_FAILED;
    }

    return EDIT_SUCCESS;
}

enum EditResult editAge(struct RecordTable *table, int id, const int newAge)
{
    struct Record *recordToEdit = findRecordById(table, id);

    if (recordToEdit == NULL)
    {
        return EDIT_FAILED;
    }

    if (newAge < 0)
    {
        return EDIT_FAILED;
    }

    if (newAge == recordToEdit->age)
    {
        return EDIT_SUCCESS;
    }

    int oldAge = recordToEdit->age;

    enum DeleteResult deleteResult = deleteNode(&table->ageTree, recordToEdit);

    if (deleteResult == DELETE_FAILED)
    {
        return EDIT_CORRUPTED;
    }

    recordToEdit->age = newAge;

    enum InsertResult insertResult = insert(&table->ageTree, recordToEdit);

    if (insertResult != INSERT_SUCCESS)
    {
        recordToEdit->age = oldAge;

        enum InsertResult rollbackInsert = insert(&table->ageTree, recordToEdit);

        if (rollbackInsert != INSERT_SUCCESS)
        {
            return EDIT_CORRUPTED;
        }

        return EDIT_FAILED;
    }

    return EDIT_SUCCESS;
}

enum SearchInitStatus initializeSearchResults(struct SearchResults *results, int count)
{
    results->records = NULL;
    results->count = 0;

    if (count == 0)
    {
        return INITIALIZATION_COMPLETE;
    }

    results->records = malloc(count * sizeof(struct Record *));

    if (results->records == NULL)
    {
        return INITIALIZATION_FAILED;
    }

    results->count = count;

    return INITIALIZATION_COMPLETE;
}

enum SearchResultStatus searchById(const struct RecordTable *table, const int id, struct SearchResults *results)
{
    struct Record *foundRecord = findRecordById(table, id);

    if (foundRecord == NULL)
    {
        results->records = NULL;
        results->count = 0;

        return SEARCH_SUCCESS;
    }

    enum SearchInitStatus searchResultsInitialized = initializeSearchResults(results, 1);

    if (searchResultsInitialized == INITIALIZATION_FAILED)
    {
        return SEARCH_FAILED;
    }

    results->records[0] = foundRecord;

    return SEARCH_SUCCESS;
}

enum SearchResultStatus searchByFirstName(const struct RecordTable *table, const char *firstName, struct SearchResults *results)
{
    int firstNameMatchesCount = 0;

    countFirstNameMatches(table->firstNameTree.root, firstName, &firstNameMatchesCount);

    enum SearchInitStatus searchResultsInitialized = initializeSearchResults(results, firstNameMatchesCount);

    if (searchResultsInitialized == INITIALIZATION_FAILED)
    {
        return SEARCH_FAILED;
    }

    int searchResultsIndex = 0;

    collectFirstNameMatches(table->firstNameTree.root, firstName, results, &searchResultsIndex);

    return SEARCH_SUCCESS;
}

enum SearchResultStatus searchByLastName(const struct RecordTable *table, const char *lastName, struct SearchResults *results)
{
    int lastNameMatchesCount = 0;

    countLastNameMatches(table->lastNameTree.root, lastName, &lastNameMatchesCount);

    enum SearchInitStatus searchResultsInitialized = initializeSearchResults(results, lastNameMatchesCount);

    if (searchResultsInitialized == INITIALIZATION_FAILED)
    {
        return SEARCH_FAILED;
    }

    int searchResultsIndex = 0;

    collectLastNameMatches(table->lastNameTree.root, lastName, results, &searchResultsIndex);

    return SEARCH_SUCCESS;
}

enum SearchResultStatus searchByAge(const struct RecordTable *table, const int age, struct SearchResults *results)
{
    int ageMatchesCount = 0;

    countAgeMatches(table->ageTree.root, age, &ageMatchesCount);

    enum SearchInitStatus searchResultsInitialized = initializeSearchResults(results, ageMatchesCount);

    if (searchResultsInitialized == INITIALIZATION_FAILED)
    {
        return SEARCH_FAILED;
    }

    int searchResultsIndex = 0;

    collectAgeMatches(table->ageTree.root, age, results, &searchResultsIndex);

    return SEARCH_SUCCESS;
}

enum SearchResultStatus listRecordsSorted(const struct RecordTable *table, enum SortField sortField, struct SearchResults *results)
{
    const struct RecordNode *root;

    switch (sortField)
    {
    case SORT_ID:
        root = table->idTree.root;
        break;

    case SORT_FIRSTNAME:
        root = table->firstNameTree.root;
        break;

    case SORT_LASTNAME:
        root = table->lastNameTree.root;
        break;

    case SORT_AGE:
        root = table->ageTree.root;
        break;

    default:
        return SEARCH_FAILED;
    }

    int nodeCount = countNodes(root);

    enum SearchInitStatus initialized = initializeSearchResults(results, nodeCount);

    if (initialized == INITIALIZATION_FAILED)
    {
        return SEARCH_FAILED;
    }

    int index = 0;

    collectNodesInOrder(root, results, &index);

    return SEARCH_SUCCESS;
}