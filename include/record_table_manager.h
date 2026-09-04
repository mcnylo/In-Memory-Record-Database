#ifndef RECORD_MANAGER_H
#define RECORD_MANAGER_H

#include "record.h"
#include "record_avl_tree.h"

// ================================================================================================

enum SearchInitStatus
{
    INITIALIZATION_COMPLETE,
    INITIALIZATION_FAILED
};

enum SearchResultStatus
{
    SEARCH_SUCCESS,
    SEARCH_FAILED
};

enum SearchField
{
    SEARCH_ID,
    SEARCH_FIRSTNAME,
    SEARCH_LASTNAME,
    SEARCH_AGE
};

enum SortField
{
    SORT_ID,
    SORT_FIRSTNAME,
    SORT_LASTNAME,
    SORT_AGE
};

// ================================================================================================

struct RecordTable
{
    struct RecordAVLTree idTree;
    struct RecordAVLTree firstNameTree;
    struct RecordAVLTree lastNameTree;
    struct RecordAVLTree ageTree;
};

struct SearchResults
{
    struct Record **records;
    int count;
};

// ================================================================================================

void initializeRecordTable(struct RecordTable *table);
void destroyRecordTable(struct RecordTable *table);
void printSortedRecords(const struct SearchResults *results);
void freeSearchResults(struct SearchResults *results);

int getNextId(const struct RecordTable *table);

struct Record *findRecordById(const struct RecordTable *table, int id);

enum InsertResult createRecord(struct RecordTable *table, const char *firstName, const char *lastName, int age);

enum DeleteResult deleteRecord(struct RecordTable *table, int id);

enum EditResult editFirstName(struct RecordTable *table, int id, const char *newFirstName);
enum EditResult editLastName(struct RecordTable *table, int id, const char *newLastName);
enum EditResult editAge(struct RecordTable *table, int id, const int newAge);

enum SearchInitStatus initializeSearchResults(struct SearchResults *results, int count);

enum SearchResultStatus searchById(const struct RecordTable *table, const int id, struct SearchResults *results);
enum SearchResultStatus searchByFirstName(const struct RecordTable *table, const char *firstName, struct SearchResults *results);
enum SearchResultStatus searchByLastName(const struct RecordTable *table, const char *lastName, struct SearchResults *results);
enum SearchResultStatus searchByAge(const struct RecordTable *table, const int age, struct SearchResults *results);
enum SearchResultStatus listRecordsSorted(const struct RecordTable *table, enum SortField sortField, struct SearchResults *results);

#endif