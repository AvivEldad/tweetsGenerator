#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000

typedef struct WordStruct {
    char * word;
    struct WordProbability * prob_list;
    int size;
    int num_of_next_words;
    int is_last;
}
        WordStruct;

typedef struct WordProbability {
    struct WordStruct * word_struct_ptr;
    int frec;
}
        WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
    WordStruct * data;
    struct Node * next;
}
        Node;

typedef struct LinkList {
    Node * first;
    Node * last;
    int size;
}
        LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList * link_list, WordStruct * data) {
    Node * new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        return 1;
    }
    * new_node = (Node) {
            data,
            NULL
    };

    if (link_list -> first == NULL) {
        link_list -> first = new_node;
        link_list -> last = new_node;
    } else {
        link_list -> last -> next = new_node;
        link_list -> last = new_node;
    }

    link_list -> size++;
    return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number) {
    return rand() % max_number;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct * get_first_random_word(LinkList * dictionary) {
    int rand_num = 0;
    Node * p;
    while (1) {
        rand_num = get_random_number(dictionary -> size);
        p = dictionary -> first;
        for (int i = 0; i < rand_num; i++) {
            p = p -> next;
        }
        if (p -> data -> word[strlen(p -> data -> word) - 1] != '.' && p -> data -> word[strlen(p -> data -> word) - 1] != '\n') {
            return p -> data;
        }
    }
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct * get_next_random_word(WordStruct * word_struct_ptr) {

    int cumulative_sum = 0, i;
    for (i = 0; i < word_struct_ptr -> num_of_next_words; i++) {
        cumulative_sum += word_struct_ptr -> prob_list[i].frec;
    }
    int rand_num = get_random_number(cumulative_sum);

    cumulative_sum = 0;
    for (i = 0; i < word_struct_ptr -> num_of_next_words; i++) {
        cumulative_sum += word_struct_ptr -> prob_list[i].frec;
        if (cumulative_sum >= rand_num) {
            break;
        }
    }
    return word_struct_ptr -> prob_list[i].word_struct_ptr;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList * dictionary) {
    int counter = 1;
    WordStruct * word = get_first_random_word(dictionary), * next_word;
    printf("%s ", word -> word);
    while (counter < MAX_WORDS_IN_SENTENCE_GENERATION) {
        next_word = get_next_random_word(word);
        if (next_word -> word[strlen(next_word -> word) - 1] == '.') {
            printf("%s\n", next_word -> word);
            break;
        } else if (next_word -> word[strlen(next_word -> word) - 1] == '\n') {
            printf("%s", next_word -> word);
            break;
        }
        printf("%s ", next_word -> word);
        word = next_word;
        counter++;
    }
    if (counter == MAX_WORDS_IN_SENTENCE_GENERATION) {
        printf("\n");
    }
    return counter;
}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list(WordStruct * first_word,
                                 WordStruct * second_word) {
    int i;
    for (i = 0; i < first_word -> num_of_next_words; i++) {
        if (strcmp(first_word -> prob_list[i].word_struct_ptr -> word, second_word -> word) == 0) {
            break;
        }
    }
    if (i == first_word -> num_of_next_words) { ///the word not in the array
        if (first_word -> prob_list == NULL) { ///there is no next words
            first_word -> prob_list = (WordProbability * ) malloc(sizeof(WordProbability) * 1);
            if (first_word -> prob_list == NULL) {
                fprintf(stdout, "Allocation failure: cannot allocate memory");
                exit(EXIT_FAILURE);
            }
        } else {
            first_word -> prob_list = (WordProbability * ) realloc(first_word -> prob_list, sizeof(WordProbability) * (first_word -> num_of_next_words + 1));
            if (first_word -> prob_list == NULL) {
                fprintf(stdout, "Allocation failure: cannot allocate memory");
                exit(EXIT_FAILURE);
            }
        }
        first_word -> prob_list[first_word -> num_of_next_words].word_struct_ptr = second_word;
        first_word -> prob_list[first_word -> num_of_next_words].frec = 1;
        first_word -> num_of_next_words += 1;
        return 1;
    } else { ///the word in the array
        first_word -> prob_list[i].frec += 1;
        return 0;
    }
}

/**
 * Search for a word in the dictionary.
 * @param dictionary Dictionary to search in
 * @param word Word to search
 * @return the node of the word if in the dictionary, NULL elsewhere
 */
Node * find_word(LinkList * dictionary, char * word) {
    Node * curr = dictionary -> first;
    while (curr != NULL) {
        if (strcmp(word, curr -> data -> word) == 0) {
            return curr;
        }
        curr = curr -> next;
    }
    return NULL;
}

/**
 * Create a new word struct, assign word to it, and add it to the dictionary.
 * @param dictionary Dictionary to add data to
 * @param word Word to add to the dictionary
 */
void create_word(LinkList * dictionary, char * word) {
    WordStruct * new_word = (WordStruct * ) malloc(sizeof(WordStruct));
    if (new_word == NULL) {
        fprintf(stdout, "Allocation failure: cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    new_word -> word = (char * ) malloc(sizeof(char) * (strlen(word) + 1));
    if (new_word -> word == NULL) {
        fprintf(stdout, "Allocation failure: cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    strcpy(new_word -> word, word);
    new_word -> size = 1;
    new_word -> prob_list = NULL;
    new_word -> num_of_next_words = 0;
    new_word -> is_last = 0;
    if (word[strlen(word) - 1] == '.' || word[strlen(word) - 1] == '\n') {
        new_word -> is_last = 1;
    }

    if(add(dictionary, new_word) == 1){
        fprintf(stdout, "Allocation failure: cannot allocate memory");
        exit(EXIT_FAILURE);
    }
}

/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary(FILE * fp, int words_to_read, LinkList * dictionary) {
    int counter = 0;
    char * word;
    char buf[MAX_SENTENCE_LENGTH];
    Node * p, * q;
    while (fgets(buf, MAX_SENTENCE_LENGTH, fp) != NULL) {
        word = strtok(buf, " ");
        p = find_word(dictionary, word);
        if (p != NULL) { ///word in the dictionary
            p -> data -> size += 1;
        } else {
            create_word(dictionary, word);
            p = dictionary -> last;
            counter++;
        }
        word = strtok(NULL, " ");
        while (word != NULL) {
            if (counter == words_to_read && words_to_read != 0) {
                return;
            }
            q = find_word(dictionary, word);
            if (q == NULL) { ///word not in the dictionary
                create_word(dictionary, word);
                q = dictionary -> last;
                counter++;
            }
            if (p -> data -> is_last == 0) { ///not end with . char
                add_word_to_probability_list(p -> data, q -> data);
            }
            p = q;
            word = strtok(NULL, " ");
        }
    }
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList * dictionary) {
    Node * p;
    while (dictionary -> first != NULL) {
        p = dictionary -> first;
        dictionary -> first = dictionary -> first -> next;
        if (p -> data -> is_last == 0) {
            free(p -> data -> prob_list);
            p -> data -> prob_list = NULL;
        }
        free(p -> data -> word);
        p -> data -> word = NULL;
        free(p -> data);
        p -> data = NULL;
        free(p);
        p = NULL;
    }

}

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main(int argc, char * argv[]) {
    if (argc < 4 || argc > 5) {
        fprintf(stdout, "Usage: <Seed> <Number of sentences to generate> <Path to file> <Optional - Number of words to read> ");
        exit(EXIT_FAILURE);
    }
    srand((int) strtol(argv[1], NULL, 10));
    FILE * fp = fopen(argv[3], "r");
    if (fp == NULL) {
        fprintf(stdout, "ERROR: cannot open file");
        exit(EXIT_FAILURE);
    }
    LinkList * dictionary = (LinkList * ) malloc(sizeof(LinkList));
    if (dictionary == NULL) {
        fprintf(stdout, "Allocation failure: cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    dictionary -> first = NULL;
    dictionary -> last = NULL;
    dictionary -> size = 0;
    if (argc == 4)
        fill_dictionary(fp, 0, dictionary);
    else
        fill_dictionary(fp, (int) strtol(argv[4], NULL, 10), dictionary);
    for (int i = 1; i <= (int) strtol(argv[2], NULL, 10); i++) {
        printf("Tweet %d: ", i);
        generate_sentence(dictionary);
    }
    free_dictionary(dictionary);
    free(dictionary);
    dictionary = NULL;
    fclose(fp);
    return 0;
}