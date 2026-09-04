# In-Memory Record Database (TempDB)

**TempDB** is an in-memory record database written in C.

It stores records in memory and uses AVL trees to index the records. The program supports record creation, search, edit, deletion, and sorted listing.

## Features

- Create records with an automatically assigned ID.
- Search records by:
  - ID
  - First name
  - Last name
  - Age
- Edit first name, last name, or age.
- Delete records by ID.
- List records in sorted order.
- Use separate AVL tree indexes for ID, first name, last name, and age.
- Use the record ID as a secondary key for duplicate names and ages.
- Manage heap memory manually.
- Clean up allocated records and tree nodes before program exit.

## Record Structure

```c
struct Record
{
    int id;
    char firstName[101];
    char lastName[101];
    int age;
};
```

The ID is unique and cannot be edited.

## Design

TempDB stores each record once on the heap.

Four AVL trees reference the same record:

```text
                 Record
              /    |    |    \
             /     |    |     \
          ID     First  Last   Age
         Tree    Tree   Tree   Tree
```

Each tree provides an index for one field.

An in-order traversal of an index returns records in sorted order.

## Build

### Requirements

- C compiler
- CMake
- CPack
- MinGW-w64 on Windows
- NSIS on Windows to create the installer

### Configure

Run from the `src` directory:

```sh
cmake -S . -B ../build -DCMAKE_BUILD_TYPE=Debug
```

### Build

```sh
cmake --build ../build
```

### Create Packages

```sh
cmake --build ../build --target package
```

On Windows, CPack creates:

- An NSIS installer
- A ZIP package

## Run

Run the generated executable from the build directory:

```sh
../build/TempDB.exe
```

The program uses a console menu for all record operations.

## Memory Management

TempDB allocates records and AVL tree nodes on the heap.

The program frees:

- Search result arrays after use.
- Record nodes during deletion.
- Records after deletion.
- All remaining records and tree nodes before program exit.

Secondary indexes do not own the records. They store pointers to the same record objects.

## Project Topics

- C
- Pointers
- Dynamic memory allocation
- AVL trees
- Recursive algorithms
- Data indexing
- Function pointers
- Structures
- Modular C design
- CMake
- CPack

## License

This project is licensed under the [MIT License](LICENSE).
