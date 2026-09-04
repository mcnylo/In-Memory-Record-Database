#include "../include/console_ui.h"
#include "../include/record_avl_tree.h"
#include "../include/record_table_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================================================================================

static void clearScreen(void)
{
#ifdef _WIN32
    system("cls"); // Windows command
#else
    system("clear"); // Linux / macOS command
#endif
}

static void printListRecords(const struct RecordTable *table, enum SortField sortField)
{
    struct SearchResults results = {0};

    enum SearchResultStatus listRecordsResult = listRecordsSorted(table, sortField, &results);

    if (listRecordsResult != SEARCH_SUCCESS)
    {
        printf("*** COULD NOT LIST RECORDS ***\n");

        return;
    }

    clearScreen();

    printSortedRecords(&results);

    freeSearchResults(&results);

    return;
}

static void printSearchRecords(const struct RecordTable *table, const enum SearchField searchField, const struct Record record)
{
    struct SearchResults results = {0};
    enum SearchResultStatus searchStatus = 0;

    switch (searchField)
    {
    case SEARCH_ID:
        searchStatus = searchById(table, record.id, &results);
        break;

    case SEARCH_FIRSTNAME:
        searchStatus = searchByFirstName(table, record.firstName, &results);
        break;

    case SEARCH_LASTNAME:
        searchStatus = searchByLastName(table, record.lastName, &results);
        break;

    case SEARCH_AGE:
        searchStatus = searchByAge(table, record.age, &results);
        break;

    default:
        printf("*** UNKNOWN SEARCH PARAMETER ***\n");
        return;
    }

    if (searchStatus != SEARCH_SUCCESS)
    {
        printf("*** ERROR: SEARCH FAILED ***\n");
    }

    clearScreen();

    printSortedRecords(&results);

    freeSearchResults(&results);

    return;
}

static void searchIdMenu(const struct RecordTable *table)
{
    char buffer[100];
    int input = 0;

    clearScreen();

    while (1)
    {
        printf("Enter ID to search: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid ID! ***\n");
            continue;
        }

        size_t inputLength = strlen(buffer);

        if (inputLength > 0 && buffer[inputLength - 1] == '\n')
        {
            buffer[inputLength - 1] = '\0';
        }
        else if (inputLength == sizeof(buffer) - 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

            printf("\n*** Please enter a valid ID! ***\n");

            continue;
        }

        if (inputLength == 0)
        {
            printf("\n*** ID cannot be empty! ***\n");

            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &input, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid ID! ***\n");

            continue;
        }

        if (input < 0)
        {
            printf("\n*** Please enter a valid ID! ***\n");

            continue;
        }

        printSearchRecords(table, SEARCH_ID, (struct Record){.id = input});

        break;
    }
}

static void searchFirstNameMenu(const struct RecordTable *table)
{
    char buffer[200];

    clearScreen();

    while (1)
    {
        printf("Enter First Name to search: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid first name! ***\n");

            continue;
        }

        size_t inputLength = strlen(buffer);

        if (inputLength > 0 && buffer[inputLength - 1] == '\n')
        {
            buffer[inputLength - 1] = '\0';
            inputLength--;
        }
        else if (inputLength == sizeof(buffer) - 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

            printf("\n*** First name cannot be longer than 100 characters! ***\n");

            continue;
        }

        if (inputLength > 100)
        {
            printf("\n*** First name cannot be longer than 100 characters! ***\n");

            continue;
        }
        else if (inputLength == 0)
        {
            printf("\n*** First Name cannot be empty! ***\n");

            continue;
        }

        struct Record temp;

        strcpy(temp.firstName, buffer);

        printSearchRecords(table, SEARCH_FIRSTNAME, temp);

        break;
    }
}

static void searchLastNameMenu(const struct RecordTable *table)
{
    char buffer[200];

    clearScreen();

    while (1)
    {
        printf("Enter Last Name to search: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid last name! ***\n");

            continue;
        }

        size_t inputLength = strlen(buffer);

        if (inputLength > 0 && buffer[inputLength - 1] == '\n')
        {
            buffer[inputLength - 1] = '\0';
            inputLength--;
        }
        else if (inputLength == sizeof(buffer) - 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

            printf("\n*** Last name cannot be longer than 100 characters! ***\n");

            continue;
        }

        if (inputLength > 100)
        {
            printf("\n*** Last name cannot be longer than 100 characters! ***\n");

            continue;
        }
        else if (inputLength == 0)
        {
            printf("\n*** Last Name cannot be empty! ***\n");

            continue;
        }

        struct Record temp;

        strcpy(temp.lastName, buffer);

        printSearchRecords(table, SEARCH_LASTNAME, temp);

        break;
    }
}

static void searchAgeMenu(const struct RecordTable *table)
{
    char buffer[100];
    int input = 0;

    clearScreen();

    while (1)
    {
        printf("Enter Age to search: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid age! ***\n");
            continue;
        }

        size_t inputLength = strlen(buffer);

        if (inputLength > 0 && buffer[inputLength - 1] == '\n')
        {
            buffer[inputLength - 1] = '\0';
        }
        else if (inputLength == sizeof(buffer) - 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

            printf("\n*** Please enter a valid age! ***\n");

            continue;
        }

        if (inputLength == 0)
        {
            printf("\n*** Age cannot be empty! ***\n");

            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &input, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid age! ***\n");

            continue;
        }

        if (input < 0)
        {
            printf("\n*** Please enter a valid age! ***\n");

            continue;
        }

        printSearchRecords(table, SEARCH_AGE, (struct Record){.age = input});

        break;
    }
}

static void preCreateRecords(struct RecordTable *table)
{
    char buffer[200];

    while (1)
    {
        int input = 0;

        printf("Do you want to pre-fill the table with test records?\n");
        printf("1. Yes\n");
        printf("2. No\n");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid input! ***\n");
            continue;
        }

        size_t inputLength = strlen(buffer);

        if (inputLength > 0 && buffer[inputLength - 1] == '\n')
        {
            buffer[inputLength - 1] = '\0';
        }
        else if (inputLength == sizeof(buffer) - 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

            printf("\n*** Please enter a valid input! ***\n");

            continue;
        }

        if (inputLength == 0)
        {
            printf("\n*** Input cannot be empty! ***\n");

            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &input, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid input! ***\n");

            continue;
        }

        switch (input)
        {
        case 1:
            createRecord(table, "Liam", "Carter", 28);
            createRecord(table, "Olivia", "Bennett", 34);
            createRecord(table, "Liam", "Mitchell", 22);
            createRecord(table, "Emma", "Carter", 45);
            createRecord(table, "Ethan", "Parker", 28);
            createRecord(table, "Olivia", "Collins", 27);
            createRecord(table, "Mason", "Turner", 34);
            createRecord(table, "Emma", "Bennett", 19);
            createRecord(table, "Lucas", "Parker", 45);
            createRecord(table, "Liam", "Hayes", 28);
            createRecord(table, "James", "Carter", 63);
            createRecord(table, "Olivia", "Turner", 27);
            createRecord(table, "Benjamin", "Bennett", 34);
            createRecord(table, "Emma", "Collins", 45);
            createRecord(table, "Henry", "Parker", 27);
            break;

        case 2:
            break;

        default:
            printf("\n*** Please enter a valid input! ***\n");
            continue;
        }

        clearScreen();

        break;
    }
}

static enum DeleteResult confirmDelete(struct RecordTable *table, const struct SearchResults *results)
{
    char buffer[100];
    int input = 0;

    clearScreen();

    while (1)
    {
        printf(">>> Found Record <<<<\n");
        printf("ID: %d\n", results->records[0]->id);
        printf("First Name: %s\n", results->records[0]->firstName);
        printf("Last Name: %s\n", results->records[0]->lastName);
        printf("Age: %d\n\n", results->records[0]->age);

        printf("Are you sure you want to delete this record?\n");
        printf("1. Yes\n");
        printf("2. No\n");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid input! ***\n");
            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &input, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid input! ***\n");
            continue;
        }

        if (input < 1 || input > 2)
        {
            printf("\n*** Please enter a valid input! ***\n");
            continue;
        }
        else if (input == 2)
        {
            clearScreen();

            return DELETE_CANCELED;
        }

        enum DeleteResult deleteResult = deleteRecord(table, results->records[0]->id);

        if (deleteResult != DELETE_SUCCESS)
        {
            clearScreen();

            printf("\n*** ERROR: DELETE RESULT RETURNED WITH CODE: %d ***\n", deleteResult);

            return deleteResult;
        }

        clearScreen();

        break;
    }

    return DELETE_SUCCESS;
}

static enum EditResult editFirstNameMenu(struct RecordTable *table, const struct SearchResults *results)
{
    char buffer[200];

    clearScreen();

    printf("/// OLD FIRST NAME: %s ///\n", results->records[0]->firstName);

    while (1)
    {
        printf("Enter new First Name: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid first name! ***\n");

            continue;
        }

        size_t inputLength = strlen(buffer);

        if (inputLength > 0 && buffer[inputLength - 1] == '\n')
        {
            buffer[inputLength - 1] = '\0';
            inputLength--;
        }
        else if (inputLength == sizeof(buffer) - 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

            printf("\n*** First name cannot be longer than 100 characters! ***\n");

            continue;
        }

        if (inputLength > 100)
        {
            printf("\n*** First name cannot be longer than 100 characters! ***\n");

            continue;
        }
        else if (inputLength == 0)
        {
            printf("\n*** First Name cannot be empty! ***\n");

            continue;
        }

        enum EditResult editResult = editFirstName(table, results->records[0]->id, buffer);

        if (editResult != EDIT_SUCCESS)
        {
            return editResult;
        }

        break;
    }

    return EDIT_SUCCESS;
}

static enum EditResult editLastNameMenu(struct RecordTable *table, const struct SearchResults *results)
{
    char buffer[200];

    clearScreen();

    printf("/// OLD LAST NAME: %s ///\n", results->records[0]->lastName);

    while (1)
    {
        printf("Enter new Last Name: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid last name! ***\n");

            continue;
        }

        size_t inputLength = strlen(buffer);

        if (inputLength > 0 && buffer[inputLength - 1] == '\n')
        {
            buffer[inputLength - 1] = '\0';
            inputLength--;
        }
        else if (inputLength == sizeof(buffer) - 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

            printf("\n*** Last name cannot be longer than 100 characters! ***\n");

            continue;
        }

        if (inputLength > 100)
        {
            printf("\n*** Last name cannot be longer than 100 characters! ***\n");

            continue;
        }
        else if (inputLength == 0)
        {
            printf("\n*** Last Name cannot be empty! ***\n");

            continue;
        }

        enum EditResult editResult = editLastName(table, results->records[0]->id, buffer);

        if (editResult != EDIT_SUCCESS)
        {
            return editResult;
        }

        break;
    }

    return EDIT_SUCCESS;
}

static enum EditResult editAgeMenu(struct RecordTable *table, const struct SearchResults *results)
{
    char buffer[200];

    clearScreen();

    printf("/// OLD AGE: %d ///\n", results->records[0]->age);

    while (1)
    {
        int input = 0;

        printf("Enter new Age: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid age! ***\n");
            continue;
        }

        size_t inputLength = strlen(buffer);

        if (inputLength > 0 && buffer[inputLength - 1] == '\n')
        {
            buffer[inputLength - 1] = '\0';
        }
        else if (inputLength == sizeof(buffer) - 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

            printf("\n*** Please enter a valid age! ***\n");

            continue;
        }

        if (inputLength == 0)
        {
            printf("\n*** Age cannot be empty! ***\n");

            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &input, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid age! ***\n");

            continue;
        }

        if (input < 0)
        {
            printf("\n*** Please enter a valid age! ***\n");

            continue;
        }

        enum EditResult editResult = editAge(table, results->records[0]->id, input);

        if (editResult != EDIT_SUCCESS)
        {
            return editResult;
        }

        break;
    }

    return EDIT_SUCCESS;
}

static enum EditResult editFoundRecordMenu(struct RecordTable *table, const struct SearchResults *results)
{
    char buffer[100];
    int input = 0;

    clearScreen();

    while (1)
    {
        printf(">>> Found Record <<<<\n");
        printf("ID: %d\n", results->records[0]->id);
        printf("First Name: %s\n", results->records[0]->firstName);
        printf("Last Name: %s\n", results->records[0]->lastName);
        printf("Age: %d\n\n", results->records[0]->age);

        printf("Select the column you wish to edit:\n");
        printf("1. First Name\n");
        printf("2. Last Name\n");
        printf("3. Age\n");
        printf("------\n");
        printf("4. Back\n\n\n");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid input! ***\n");
            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &input, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid input! ***\n");
            continue;
        }

        enum EditResult editResult;

        switch (input)
        {
        case 1:
            editResult = editFirstNameMenu(table, results);
            break;

        case 2:
            editResult = editLastNameMenu(table, results);
            break;

        case 3:
            editResult = editAgeMenu(table, results);
            break;

        case 4:
            clearScreen();
            return EDIT_CANCELED;

        default:
            printf("\n*** Please enter a valid input! ***\n");
            continue;
        }

        if (editResult != EDIT_SUCCESS)
        {
            clearScreen();

            printf("\n*** ERROR: EDIT RESULT RETURNED WITH CODE: %d ***\n", editResult);

            return editResult;
        }

        clearScreen();

        break;
    }

    return EDIT_SUCCESS;
}

static void createRecordMenu(struct RecordTable *table)
{
    char buffer[200];

    clearScreen();

    printf("===============================\n");
    printf("         CREATE RECORD         \n");
    printf("===============================\n");

    while (1)
    {
        char firstNameInput[101];
        char lastNameInput[101];
        int ageInput = -1;

        int gotFirstName = 0;
        int gotLastName = 0;
        int gotAge = 0;

        while (!gotFirstName)
        {
            printf("Enter First Name: ");

            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            {
                printf("\n*** Please enter a valid first name! ***\n");

                continue;
            }

            size_t inputLength = strlen(buffer);

            if (inputLength > 0 && buffer[inputLength - 1] == '\n')
            {
                buffer[inputLength - 1] = '\0';
                inputLength--;
            }
            else if (inputLength == sizeof(buffer) - 1)
            {
                int ch;

                while ((ch = getchar()) != '\n' && ch != EOF)
                {
                }

                printf("\n*** First name cannot be longer than 100 characters! ***\n");

                continue;
            }

            if (inputLength > 100)
            {
                printf("\n*** First name cannot be longer than 100 characters! ***\n");

                continue;
            }
            else if (inputLength == 0)
            {
                printf("\n*** First Name cannot be empty! ***\n");

                continue;
            }

            strcpy(firstNameInput, buffer);

            gotFirstName = 1;
        }

        while (!gotLastName)
        {
            printf("Enter Last Name: ");

            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            {
                printf("\n*** Please enter a valid last name! ***\n");
                continue;
            }

            size_t inputLength = strlen(buffer);

            if (inputLength > 0 && buffer[inputLength - 1] == '\n')
            {
                buffer[inputLength - 1] = '\0';
                inputLength--;
            }
            else if (inputLength == sizeof(buffer) - 1)
            {
                int ch;

                while ((ch = getchar()) != '\n' && ch != EOF)
                {
                }

                printf("\n*** Last name cannot be longer than 100 characters! ***\n");

                continue;
            }

            if (inputLength > 100)
            {
                printf("\n*** Last name cannot be longer than 100 characters! ***\n");

                continue;
            }
            else if (inputLength == 0)
            {
                printf("\n*** Last Name cannot be empty! ***\n");

                continue;
            }

            strcpy(lastNameInput, buffer);

            gotLastName = 1;
        }

        while (!gotAge)
        {
            printf("Enter Age: ");

            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            {
                printf("\n*** Please enter a valid age! ***\n");
                continue;
            }

            size_t inputLength = strlen(buffer);

            if (inputLength > 0 && buffer[inputLength - 1] == '\n')
            {
                buffer[inputLength - 1] = '\0';
            }
            else if (inputLength == sizeof(buffer) - 1)
            {
                int ch;

                while ((ch = getchar()) != '\n' && ch != EOF)
                {
                }

                printf("\n*** Please enter a valid age! ***\n");

                continue;
            }

            if (inputLength == 0)
            {
                printf("\n*** Age cannot be empty! ***\n");

                continue;
            }

            char trashData;
            int parsed = sscanf(buffer, "%d %c", &ageInput, &trashData);

            if (parsed != 1)
            {
                printf("\n*** Please enter a valid age! ***\n");

                continue;
            }

            if (ageInput < 0)
            {
                printf("\n*** Please enter a valid age! ***\n");

                continue;
            }

            gotAge = 1;
        }

        enum InsertResult recordInsertResult = createRecord(table, firstNameInput, lastNameInput, ageInput);

        if (recordInsertResult != INSERT_SUCCESS)
        {
            printf("*** Record could not be created. Exited with Insert Code: %d ***\n", recordInsertResult);

            break;
        }

        clearScreen();

        printf(">>> Record Created <<<\n");
        printf(">>> %s %s, %d <<<\n", firstNameInput, lastNameInput, ageInput);

        break;
    }
}

static void editRecordMenu(struct RecordTable *table)
{
    char buffer[100];
    int input = 0;

    clearScreen();

    while (1)
    {
        printf("===============================\n");
        printf("          EDIT RECORDS         \n");
        printf("===============================\n");

        printf("/// NOTE: Enter -1 to return to the Main Menu ///\n");
        printf("Enter the ID of the record you want to edit: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid ID! ***\n");
            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &input, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid ID! ***\n");
            continue;
        }

        if (input < -1)
        {
            printf("\n*** Please enter a valid ID! ***\n");
            continue;
        }
        else if (input == -1)
        {
            clearScreen();
            break;
        }

        struct SearchResults results = {0};

        enum SearchResultStatus searchStatus = searchById(table, input, &results);

        if (searchStatus != SEARCH_SUCCESS)
        {
            clearScreen();

            printf("\n*** An error occurred when searching for the record! ***\n");

            break;
        }

        if (results.count == 0)
        {
            printf("\n>>> No records found for ID %d <<<\n", input);

            continue;
        }

        enum EditResult editResult = editFoundRecordMenu(table, &results);

        freeSearchResults(&results);

        if (editResult != EDIT_SUCCESS)
        {
            continue;
        }

        clearScreen();

        printf("\n>>> Record Successfully Edited! <<<\n");

        break;
    }
}

static void deleteRecordMenu(struct RecordTable *table)
{
    char buffer[100];
    int input = 0;

    clearScreen();

    while (1)
    {
        printf("===============================\n");
        printf("         DELETE RECORDS        \n");
        printf("===============================\n");

        printf("/// NOTE: Enter -1 to return to the Main Menu ///\n");
        printf("Enter the ID of the record you want to delete: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid ID! ***\n");
            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &input, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid ID! ***\n");
            continue;
        }

        if (input < -1)
        {
            printf("\n*** Please enter a valid ID! ***\n");
            continue;
        }
        else if (input == -1)
        {
            clearScreen();
            break;
        }

        struct SearchResults results = {0};

        enum SearchResultStatus searchStatus = searchById(table, input, &results);

        if (searchStatus != SEARCH_SUCCESS)
        {
            clearScreen();

            printf("\n*** An error occurred when searching for the record! ***\n");

            break;
        }

        if (results.count == 0)
        {
            printf("\n>>> No records found for ID %d <<<\n", input);

            continue;
        }

        enum DeleteResult deleteResult = confirmDelete(table, &results);

        freeSearchResults(&results);

        if (deleteResult != DELETE_SUCCESS)
        {
            continue;
        }

        clearScreen();

        printf("\n>>> Record Successfully Deleted! <<<\n");

        break;
    }
}

static void searchRecordMenu(const struct RecordTable *table)
{
    char buffer[100];
    int selection = 0;

    clearScreen();

    while (selection != 5)
    {
        printf("===============================\n");
        printf("         SEARCH RECORDS        \n");
        printf("===============================\n");
        printf("1. Search by ID\n");
        printf("2. Search by First Name\n");
        printf("3. Search by Last Name\n");
        printf("4. Search by Age\n");
        printf("------\n");
        printf("5. Back to Main Menu\n\n\n");

        printf("Select an option: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid option! ***\n");
            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &selection, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid option! ***\n");
            continue;
        }

        switch (selection)
        {
        case 1:
            searchIdMenu(table);
            break;

        case 2:
            searchFirstNameMenu(table);
            break;

        case 3:
            searchLastNameMenu(table);
            break;

        case 4:
            searchAgeMenu(table);
            break;

        case 5:
            clearScreen();
            break;

        default:
            printf("\n*** Please enter a valid option! ***\n");
            break;
        }
    }
}

static void listRecordMenu(const struct RecordTable *table)
{
    char buffer[100];
    int selection = 0;

    clearScreen();

    while (selection != 5)
    {
        printf("===============================\n");
        printf("          LIST RECORDS         \n");
        printf("===============================\n");
        printf("1. List by ID\n");
        printf("2. List by First Name\n");
        printf("3. List by Last Name\n");
        printf("4. List by Age\n");
        printf("------\n");
        printf("5. Back to Main Menu\n\n\n");

        printf("Select an option: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid option! ***\n");
            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &selection, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid option! ***\n");
            continue;
        }

        switch (selection)
        {
        case 1:
            printListRecords(table, SORT_ID);
            break;

        case 2:
            printListRecords(table, SORT_FIRSTNAME);
            break;

        case 3:
            printListRecords(table, SORT_LASTNAME);
            break;

        case 4:
            printListRecords(table, SORT_AGE);
            break;

        case 5:
            clearScreen();
            break;

        default:
            printf("\n*** Please enter a valid option! ***\n");
            break;
        }
    }
}

// ================================================================================================

void mainMenu(void)
{
    char buffer[100];
    int selection = 0;

    struct RecordTable table;

    printf("Initializing Record table...\n");

    initializeRecordTable(&table);

    printf(">>> Record table ready! <<<\n\n");

    preCreateRecords(&table);

    while (selection != 6)
    {
        printf("===============================\n");
        printf("        RECORD DATABASE        \n");
        printf("===============================\n");
        printf("1. Create Record\n");
        printf("2. Edit Record\n");
        printf("3. Delete Record\n");
        printf("4. Search for a Record\n");
        printf("5. List Records\n");
        printf("------\n");
        printf("6. Exit\n\n\n");

        printf("Select an option: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\n*** Please enter a valid option! ***\n");
            continue;
        }

        char trashData;
        int parsed = sscanf(buffer, "%d %c", &selection, &trashData);

        if (parsed != 1)
        {
            printf("\n*** Please enter a valid option! ***\n");
            continue;
        }

        switch (selection)
        {
        case 1:
            createRecordMenu(&table);
            break;

        case 2:
            editRecordMenu(&table);
            break;

        case 3:
            deleteRecordMenu(&table);
            break;

        case 4:
            searchRecordMenu(&table);
            break;

        case 5:
            listRecordMenu(&table);
            break;

        case 6:
            break;

        default:
            printf("\n*** Please enter a valid option! ***\n");
            break;
        }
    }

    destroyRecordTable(&table);
}
