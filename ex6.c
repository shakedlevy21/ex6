#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        BFSGeneric(owner->pokedexRoot, displayBFS);
        break;
    case 2:
        preOrderGeneric(owner->pokedexRoot ,preOrderTraversal);
        break;
    case 3:
        inOrderGeneric(owner->pokedexRoot, inOrderTraversal);
        break;
    case 4:
        postOrderGeneric(owner->pokedexRoot, postOrderTraversal);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    // list owners
    printf("\nExisting Pokedexes:\n");
    printOwnerNames();

    //choosing an owner
    int choice = readIntSafe("Choose a Pokedex by number: ");
    OwnerNode *current = findOwnerByIndex(choice);
    printf("\nEntering %s's Pokedex...\n", current->ownerName);

    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", current->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            addPokemon(current);
            break;
        case 2:
            displayMenu(current);
            break;
        case 3:
            //freePokemon(current);
            break;
        case 4:
            //pokemonFight(current);
            break;
        case 5:
            //evolvePokemon(current);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            //deletePokedex();
            break;
        case 4:
            //mergePokedexMenu();
            break;
        case 5:
            //sortOwners();
            break;
        case 6:
            //printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

void openPokedexMenu() {
    printf("Your name: ");
    char *name = getDynamicInput();
    printf("Choose Starter:\n"
           "1. Bulbasaur\n"
           "2. Charmander\n"
           "3. Squirtle\n");
    int choice = readIntSafe("Your choice: ");
    const PokemonData *newPokemon;
    switch (choice) {
        case 1:
            newPokemon = &pokedex[0];
        break;
        case 2:
            newPokemon = &pokedex[3];
        break;
        case 3:
            newPokemon = &pokedex[6];
        break;
    default:
        newPokemon = &pokedex[0];
    }
    PokemonNode *newpokemonnode = createPokemonNode(newPokemon);
    OwnerNode *newOwner = createOwner(name, newpokemonnode);
    printf("New Pokedex created for %s with starter %s\n"
        , newOwner->ownerName, newOwner->pokedexRoot->data->name);
}


PokemonNode *createPokemonNode(const PokemonData *data) {
    PokemonNode *node = (PokemonNode *)malloc(sizeof(PokemonNode));
    node->data = (PokemonData *)malloc(sizeof(PokemonData));
    node->data->id = data->id;
    node->data->name = data->name;
    node->data->TYPE = data->TYPE;
    node->data->hp = data->hp;
    node->data->attack = data->attack;
    node->data->CAN_EVOLVE = data->CAN_EVOLVE;
    node->left = NULL;
    node->right = NULL;
    return node;
}

OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    OwnerNode *owner = (OwnerNode *)malloc(sizeof(OwnerNode));
    if (owner == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    owner->ownerName = ownerName;
    owner->pokedexRoot = starter;

    //if the list is empty
    if (ownerHead == NULL) {
        ownerHead = owner;
        owner->next = NULL;
        owner->prev = NULL;
        return owner;
    }

    //if the list has one owner
    if (ownerHead->next == NULL) {
        ownerHead->next = owner;
        owner->next = ownerHead;
        owner->prev = ownerHead;
        return owner;
    }
    OwnerNode *owneriterrator = ownerHead;
    while (owneriterrator->next != ownerHead) {
        owneriterrator = owneriterrator->next;
    }

    //emplement prev and next owner
    owneriterrator->next = owner;
    owner->next = ownerHead;
    owner->prev = owneriterrator;
    ownerHead->prev = owner;
    return owner;
}

void printOwnerNames() {
    OwnerNode *iterator = ownerHead;
    int counter = 1;
    if (ownerHead == NULL) {
        printf("No Pokedex found.\n");
        return;
    }
    if (ownerHead->next == NULL) {
        printf("[%d] %s\n", counter, iterator->ownerName);
        return;
    }
    do {
        printf("[%d] %s\n", counter, iterator->ownerName);
        iterator = iterator->next;
        counter++;
    } while (iterator != ownerHead);
}

OwnerNode *findOwnerByIndex(int index) {
    OwnerNode *iterator = ownerHead;
    int counter = 1;
    while (counter != index) {
        iterator = iterator->next;
        counter++;
    }
    return iterator;
}

//search for pokemon in pokedex by id
PokemonNode *searchPokemonBFS(PokemonNode *root, int id) {
    if (root == NULL) {
        return NULL;
    }
    if (root->data->id > id) {
        return searchPokemonBFS(root->left, id);
    }
    if (root->data->id < id) {
        return searchPokemonBFS(root->right, id);
    }
    return root;
}

//insert node to BST by id
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    if (root == NULL) {
        PokemonNode *newRoot = createPokemonNode(newNode->data);
        return newRoot;
    }
    if (root->data->id > newNode->data->id) {
        root->left = insertPokemonNode(root->left, newNode);
    }
    if (root->data->id < newNode->data->id) {
        root->right = insertPokemonNode(root->right, newNode);
    }
    if (root->data->id == newNode->data->id) {
        return NULL;
    }
    return root;
}

void addPokemon(OwnerNode *owner) {
    int index = -1;
    printf("Enter ID to add: ");
    scanf("%d", &index);
    if (searchPokemonBFS(owner->pokedexRoot, index) == NULL) {
        //create a new pokemon node and insert to BST
        const PokemonData *newPokemon = &pokedex[index-1];
        PokemonNode *newpokemonnode = createPokemonNode(newPokemon);
        owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newpokemonnode);
        printf("Pokemon %s (ID %d) added.", newpokemonnode->data->name, newpokemonnode->data->id);
        free(newpokemonnode);
    } else {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", index);
    }
}

void initNodeArray(NodeArray *na, int cap) {
    na->capacity = cap;
    na->size = 0;
    na->nodes = (PokemonNode **)malloc(cap * sizeof(PokemonNode *));
    if (na->nodes == NULL) {
        exit(1);  // Memory allocation failed
    }
}

void addNode(NodeArray *na, PokemonNode *node) {
    if (na->size == na->capacity) {
        // Resize the array when it's full
        na->capacity *= 2;  // Double the capacity
        na->nodes = realloc(na->nodes, na->capacity * sizeof(PokemonNode *));
        if (na->nodes == NULL) {
            exit(1);  // Memory allocation failed
        }
    }
    na->nodes[na->size] = node;  // Add the new node
    na->size++;  // Increment the size
}

void displayBFS(PokemonNode *root) {
    printPokemonNode(root);
}

void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }

    NodeArray nodeArray;
    initNodeArray(&nodeArray, 1);
    addNode(&nodeArray, root);

    int currentIndex = 0;
    while (currentIndex < nodeArray.size) {
        PokemonNode *current = nodeArray.nodes[currentIndex];
        visit(current);
        currentIndex++;
        if (current->left != NULL) {
            addNode(&nodeArray, current->left);
        }
        if (current->right != NULL) {
            addNode(&nodeArray, current->right);
        }
    }

    free(nodeArray.nodes);
}

void preOrderTraversal(PokemonNode *root) {
    printPokemonNode(root);
}

void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    visit(root);
    if (root->left != NULL) {
        preOrderGeneric(root->left, visit);
    }
    if (root->right != NULL) {
        preOrderGeneric(root->right, visit);
    }
}

void inOrderTraversal(PokemonNode *root) {
    printPokemonNode(root);
}

void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    if (root->left != NULL) {
        inOrderGeneric(root->left, visit);
    }
    visit(root);
    if (root->right != NULL) {
        inOrderGeneric(root->right, visit);
    }
}

void postOrderTraversal(PokemonNode *root) {
    printPokemonNode(root);
}

void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    if (root->left != NULL) {
        postOrderGeneric(root->left, visit);
    }
    if (root->right != NULL) {
        postOrderGeneric(root->right,visit);
    }
    visit(root);
}

void addToArray(PokemonNode *root) {

}

void collectAll(PokemonNode *root, NodeArray *na) {

    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    if (root->left != NULL) {
        collectAll(root->left, na);
    }
    if (root->right != NULL) {
        collectAll(root->right,na);
    }
    addNode(na, root);

}

int compareByNameNode(const void *a, const void *b) {
    return strcmp(a, b);
}

void displayAlphabetical(PokemonNode *root) {
    NodeArray *nodeArr = NULL;
    initNodeArray(nodeArr, 1);
    //initializing the array with all the nodes in the tree
    collectAll(root, nodeArr);
    for (int i = 0; i < nodeArr->size; i++) {
        printPokemonNode(nodeArr->nodes[i]);
    }
    qsort(nodeArr, nodeArr->size, sizeof(PokemonNode), compareByNameNode);
    for (int i = 0; i < nodeArr->size; i++) {
        printPokemonNode(nodeArr->nodes[i]);
    }
}
int main()
{
    mainMenu();
    //freeAllOwners();
    return 0;
}
