// Implements a dictionary's functionality
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

void add_word(char *word, int index);
void null_hash_table();
void free_llist(node *node);
void init_hash_table();
void print_statistics();
void print_table_lengths();
void print_llist(node *node);

const unsigned int N = 200003;
unsigned int word_count = 0;

// Hash table
node *table[N];

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    int index = hash(word);
    node *supected_place = table[index];
    if (supected_place == NULL)
    {
        return false;
    }
    while (true)
    {
        if (strcasecmp(word, supected_place->word) == 0)
        {
            return true;
        }
        else if (strcasecmp(word, supected_place->word) > 0 && supected_place->next != NULL)
        {
            supected_place = supected_place->next;
        }
        else
        {
            return false;
        }
    }
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    int len = strlen(word);
    int starting_char_factor = (toupper(word[0]) - 'A') * N / 26;
    int length_factor = len * (N / 26 / 31);
    int sum_factor = 0;
    for (int i = 0; i < len; i++)
    {
        if (word[i] >= 'a')
        {
            sum_factor += (word[i] - 'a') * (len - i) * (len - i);
        }
        else if (word[i] >= 'A')
        {
            sum_factor += (word[i] - 'A') * (len - i) * (len - i);
        }
    }

    return (starting_char_factor + sum_factor + length_factor) % N;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    FILE *words = fopen(dictionary, "r");
    if (words == NULL)
    {
        printf("%s could not be opened\n", dictionary);
        return false;
    }
    void init_hash_table();
    char cur_letter[1];
    int cur_length = 0;
    char *cur_word = calloc(LENGTH + 1, 1);
    while (fread(cur_letter, 1, 1, words))
    {
        if (cur_letter[0] == '\n')
        {
            cur_word[cur_length] = '\0';
            int index = hash(cur_word);
            add_word(cur_word, index);
            cur_length = 0;
        }
        else
        {
            cur_word[cur_length] = cur_letter[0];
            cur_length++;
        }
    }
    free(cur_word);
    fclose(words);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return word_count;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    null_hash_table();
    for (int i = 0; i < N; i++)
    {
        if (table[i] != NULL)
        {
            return false;
        }
    }
    return true;
}

void free_llist(node *node)
{
    if (node->next != NULL)
    {
        free_llist(node->next);
        node->next = NULL;
    }
    free(node);
}

void null_hash_table()
{
    for (int i = 0; i < N; i++)
    {
        if (table[i] != NULL)
        {
            free_llist(table[i]);
            table[i] = NULL;
        }
    }
    return;
}
void init_hash_table()
{
    for (int i = 0; i < N; i++)
    {
        table[i] = NULL;
    }
    return;
}

void add_word(char *word, int index)
{
    node *new_node = malloc(sizeof(node));
    memcpy(new_node->word, word, 46);
    new_node->next = NULL;

    if (table[index] == NULL)
    {
        table[index] = new_node;
        word_count++;
        return;
    }
    else
    {
        node *cur = table[index];
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = new_node;
        word_count++;
        return;
    }
}
// The below were used to find better solutions and test where the collisions could come from.
void print_statistics()
{
    unsigned int longest_link = 0;
    unsigned int longest_index = 0;
    unsigned int nulls_in_table = N;
    int length_count[51];
    for (int i = 0; i < 51; i++)
    {
        length_count[i] = 0;
    }

    for (int i = 0; i < N; i++)
    {
        int length = 0;
        node *cur_node = table[i];
        if (cur_node != NULL)
        {
            length++;
            nulls_in_table--;
            while (cur_node->next != NULL)
            {
                length++;
                cur_node = cur_node->next;
            }
        }
        if (length > longest_link)
        {
            longest_link = length;
            longest_index = i;
        }
        if (length > 50)
        {
            length_count[50]++;
        }
        if (length <= 50)
        {
            length_count[length]++;
        }
    }
    int table_occupancy = 100 * (N - nulls_in_table) / N;
    printf("There are %i linked lists in the table.", N - nulls_in_table);
    printf("There are %i NULLs in table.\n", nulls_in_table);
    printf("%i %% of the table is utilized.\n", table_occupancy);
    printf("%i elems can only be accessed through another word.\n",
           word_count - (N - nulls_in_table));
    printf("The average element count in all linked lists is: %i.\n",
           word_count / (N - nulls_in_table));
    printf("The longest link consists of %i elements, and is at %i index in the table.\n",
           longest_link, longest_index);
    printf("The elements of this index are:\n");
    print_llist(table[longest_index]);

    for (int i = 0; i < 51; i++)
    {
        printf("From %i links: %i can be found in the table.\n", i, length_count[i]);
    }
}
void print_table_lengths()
{
    unsigned int link_counts[N];
    for (int i = 0; i < N; i++)
    {
        node *cur_node = table[i];
        if (cur_node != NULL)
        {
            int length = 1;
            while (cur_node->next != NULL)
            {
                length++;
                cur_node = cur_node->next;
            }
            printf("\n%i:%i", i, length);
        }
        else
        {
            printf("N");
        }
    }
}
void print_llist(node *init_node)
{
    node *cur_node = init_node;
    while (cur_node->next != NULL)
    {
        printf("%s\n", cur_node->word);
        cur_node = cur_node->next;
    }
}
