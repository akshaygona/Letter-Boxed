#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MORE_LINES_NEEDED 1024
#define MORE_CHARS_NEEDED 1024

#define MAX_BOARD_SIZE 26

void freeResources(char **dictionary, size_t total_lines, FILE *dictFile) {
    for (size_t i = 0; i < total_lines; i++) {
        free(dictionary[i]);
    }
    free(dictionary);
    fclose(dictFile);
}

//./letter-boxed board1.txt dict.txt
int main(int argc, char *argv[]) {
    //check for correct number of arguments
    if (argc != 3) {
        printf("Incorrect number of arguments\n");
        return 1;
    }
    //board1.txt is the board
    char board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    FILE *boardFile = fopen(argv[1], "r");
    int line = 0;
    //reading the board file

    while (!feof(boardFile) && !ferror(boardFile) && line < MAX_BOARD_SIZE) {
        if(fgets(board[line], MAX_BOARD_SIZE, boardFile) != NULL) {
            line++;
        }
    }
    //checking if board is valid

    //1. check if the board has less than three sides
    if (line < 3) {
        printf("Invalid board\n");
        return 1;
    }
    //2. check if the board has the same letter more than once in any row or column
    for (int i = 0; i < line; i++) {
        for (int j = 0; j < line; j++) {
            for (int k = j + 1; k < line; k++) {
                if (board[i][j] == board[i][k] && board[i][j] != ' ' && board[i][j] != '\n') {
                    printf("Invalid board\n");
                    return 1;
                }
                if (board[j][i] == board[k][i] && board[j][i] != ' ' && board[j][i] != '\n') {
                    printf("Invalid board\n");
                    return 1;
                }
            }
        }
    }

    //initializing array of distinct letters
    char *distinct_letters[26];
    for (int i = 0; i < 26; i++) {
        distinct_letters[i] = "0";
    }
    //example, abde\n hjik\n olmn EOF is the board, array would have 1s at the indexes 0,1,3,4,7,8,9,10,11,12,14,15,16,17
    for (int i = 0; i < line; i++) {
        for (int j = 0; j < line; j++) {
            if (board[i][j] != ' ' && board[i][j] != '\n') {
                distinct_letters[board[i][j] - 'a'] = "1";
            }
        }
    }

    fclose(boardFile);


    //initialize dictionary and dynamically allocate/resize memory
    FILE *dictFile = fopen(argv[2], "r");
    if (dictFile == NULL) {
        printf("Error opening dictionary file\n");
        return 1;
    }
    char **dictionary;
    dictionary = malloc(MORE_LINES_NEEDED * sizeof(char *));

    size_t total_lines = 0;
    size_t total_chars = 0;
    char c;

    do {
        c = fgetc(dictFile);
        if(ferror(dictFile)) {
            printf("Error reading dictionary file\n");
            return 1;
        }
        if (feof(dictFile)){
            if(total_chars != 0) {
                dictionary[total_lines] = realloc(dictionary[total_lines], total_chars + 1);
                dictionary[total_lines][total_chars] = '\0';
                total_lines++;
            }
            break;
        }
        if (total_chars == 0) {
            dictionary[total_lines] = malloc(MORE_CHARS_NEEDED);
        }
        dictionary[total_lines][total_chars] = c;
        total_chars++;
        if (c == '\n') {
            dictionary[total_lines] = realloc(dictionary[total_lines], total_chars + 1);
            dictionary[total_lines][total_chars] = '\0';
            total_lines++;
            total_chars = 0;

            if (total_lines % MORE_LINES_NEEDED == 0) {
                size_t new_size = total_lines + MORE_LINES_NEEDED;
                dictionary = realloc(dictionary, new_size * sizeof(char *));
            }
        }
        else if (total_chars % MORE_CHARS_NEEDED == 0) {
            size_t new_size = total_chars + MORE_CHARS_NEEDED;
            dictionary[total_lines] = realloc(dictionary[total_lines], new_size);
        }
    } while (true);
    dictionary = realloc(dictionary, total_lines * sizeof(char *));
    for (size_t i = 0; i < total_lines; i++) {
        if (dictionary[i] && dictionary[i][strlen(dictionary[i]) - 1] == '\n') {
            dictionary[i][strlen(dictionary[i]) - 1] = '\0';
        }
    }
    // for (size_t i = 0; i < total_lines; i++) {
    //     printf("%s", dictionary[i]);
    // }
    //freeing memory

    char *word = NULL;
    char final_char = ' ';
    //while loop to get every line
    while (1) {
        size_t word_length = 0;
        ssize_t read = getline(&word, &word_length, stdin);
    
        //ideally shouldnt hit unless a file with all input at once is provided and it doesnt use every letter
        if (read == -1 && feof(stdin)) {
            freeResources(dictionary, total_lines, dictFile);
            printf("Not all letters used\n");
            exit(0);
        }
        //remove newline character
        word[strlen(word) - 1] = '\0';
        
        //1. check if the word has a letter that is not in the board
        for (size_t i = 0; i < strlen(word); i++) {
            bool found = false;
            for (int j = 0; j < line; j++) {
                for (int k = 0; k < line; k++) {
                    if (word[i] == board[j][k]) {
                        found = true;
                        break;
                    }
                }
                if (found) {
                    break;
                }
            }
            if (!found) {
                freeResources(dictionary, total_lines, dictFile);
                printf("Used a letter not present on the board\n");
                exit(0);
            }
        }

        //2. check if word starts with the last letter of the previous word
        if (final_char != ' ' && word[0] != final_char) {
            printf("First letter of word does not match last letter of previous word\n");
            freeResources(dictionary, total_lines, dictFile);
            exit(0);
        }

        //keep track of the final character in the word
        final_char = word[strlen(word) - 1];
        //printf("%c\n",final_char);

        //3. check if the word has same-side letter consecutively, for example, if the word is "flan", and the boardfile contains flet as one of the sides, then the word is invalid
        for(size_t i = 0; i < strlen(word) - 1; i++) {
            for (int j = 0; j < line; j++) {
                for (int k = 0; k < line; k++) {
                    if (word[i] == board[j][k]) {
                        if (word[i + 1] == board[j][k]) {
                            printf("Same-side letter used consecutively\n");
                            freeResources(dictionary, total_lines, dictFile);
                            exit(0);
                        }
                    }
                }
            }
        }

        //4. check if the word is in the dictionary
        bool found = false;
        for (size_t i = 0; i < total_lines; i++) {
            if (strcmp(word, dictionary[i]) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            freeResources(dictionary, total_lines, dictFile);
            printf("Word not found in dictionary\n");
            exit(0);
        }


        //4.5 update letter array for each letter in input word to 0 if the letter has been used
        for (size_t i = 0; i < strlen(word); i++) {
            for (int j = 0; j < line; j++) {
                for (int k = 0; k < line; k++) {
                    if (word[i] == board[j][k]) {
                        distinct_letters[board[j][k] - 'a'] = "0";
                    }
                }
            }
        }

        //5. check if all the letters on the board have been used
        bool all_used = true;
        for (int i = 0; i < 26; i++) {
            if (strcmp(distinct_letters[i], "1") == 0) {
                all_used = false;
                break;
            }
        }
        if (all_used) {
            freeResources(dictionary, total_lines, dictFile);
            printf("Correct\n");
            return 0;
        }
        //print read - getline thing
        
    }
    freeResources(dictionary, total_lines, dictFile);
    return 0;
}
