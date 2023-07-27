// Written by Scott Tredinnick, z5258051
// Started on 31/10/2022
// Finished on 16/11/2022
// Create a todo list for the user
// Add tasks to the list with thier categories and priorities
// You can also complete the tasks in the list alongside other functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INVALID_PRIORITY    -1

#define MAX_TASK_LENGTH     200
#define MAX_CATEGORY_LENGTH 40
#define MAX_STRING_LENGTH   1024

#define COMMAND_ADD_TASK        'a'
#define COMMAND_PRINT_TASKS     'p'
#define COMMAND_UPDATE_PRIORITY 'i'
#define COMMAND_COUNT_TASKS     'n'
#define COMMAND_COMPLETE_TASK   'c'
#define COMMAND_PRINT_COMPLETE  'P'
#define COMMAND_ESTIMATE_TIME   'e'
#define COMMAND_DELETE_TASK     'd'
#define COMMAND_FINISH_DAY      'f'
#define COMMAND_REPEAT_TASK     'r'
#define COMMAND_MATCH_TASK      'm'
#define COMMAND_DELETE_MATCH    '^'
#define COMMAND_SORT_TASKS      's'

enum priority { LOW, MEDIUM, HIGH };

struct task {
    char task_name[MAX_TASK_LENGTH];
    char category[MAX_CATEGORY_LENGTH];
    enum priority priority;
    int repeatable;
    struct task *next;
};

struct completed_task {
    struct task *task;
    int start_time;
    int finish_time;
    struct completed_task *next;
};

struct todo_list {
    struct task *tasks;
    struct completed_task *completed_tasks;
};

////////////////////////////////////////////////////////////////////////////////
///////////////////// YOUR FUNCTION PROTOTYPES GO HERE /////////////////////////
////////////////////////////////////////////////////////////////////////////////

void initialise_todo(struct todo_list *todo);

void command_loop(struct todo_list *todo);

void add_task(struct todo_list *todo);

void print_tasks(struct todo_list *todo);

void update_priority(struct todo_list *todo);

void count_tasks(struct todo_list *todo);

void complete_task(struct todo_list *todo);
void comp_time(struct todo_list *todo, struct completed_task *comp, int start, int fin);
int comp_mv(struct todo_list *todo, struct completed_task *comp, char *cat, char *task);

void remove_task(struct todo_list *todo, struct task *curr, struct task *prev);

void print_complete(struct todo_list *todo);

void estimate_time(struct todo_list *todo);
int get_estimate(struct completed_task *comp, char *category);

void delete_task(struct todo_list *todo);

void finish_day(struct todo_list *todo);
void repeatable(struct todo_list *todo, struct completed_task *comp, struct task *curr);

void repeat_task(struct todo_list *todo);

void free_list(struct todo_list *todo);

void match_task(struct todo_list *todo);
void check_pattern(int *i, int *j, char *pattern, char *task, int *print_task);
void pattern_loop(int *i, int *j, char *pattern, char *task, int *print_task);

void star_pattern (int *i, int *j, char *pattern, char *task, int *print_task);
void end_star(int *match_count, int *j, char *match, char *task, int *is_match);
void star(int *match_count, int *j, char *match, char *task, int *is_match);

void bracket_pattern(int *i, int *j, char *task, char *pattern, int *print_task);

void delete_match(struct todo_list *todo);

void sort_tasks(struct todo_list *todo);
struct task *sort(struct task *insert_task, struct todo_list *new_list);
struct task *insert(struct task *insert, struct task *compare, struct task *previous);

////////////////////////////////////////////////////////////////////////////////
//////////////////////// PROVIDED HELPER PROTOTYPES ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void parse_add_task_line(
    char buffer[MAX_STRING_LENGTH], char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH], enum priority *prio
);
void parse_task_category_line(
    char buffer[MAX_STRING_LENGTH], char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH]
);
void parse_complete_task_line(
    char buffer[MAX_STRING_LENGTH], char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH], int *start_time, int *finish_time
);

enum priority string_to_priority(char priority[MAX_STRING_LENGTH]);
void remove_newline(char input[MAX_STRING_LENGTH]);
void trim_whitespace(char input[MAX_STRING_LENGTH]);
void print_one_task(int task_num, struct task *task);
void print_completed_task(struct completed_task *completed_task);

int task_compare(struct task *t1, struct task *t2);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {
    struct todo_list *todo = malloc(sizeof(struct todo_list));
    initialise_todo(todo);
    
    command_loop(todo);

    return 0;
}

/**
 * The central loop that executes commands until the program is completed.
 *
 * Parameters:
 *     todo - The todo list to execute commands on.
 *
 * Returns:
 *     Nothing
 */
void command_loop(struct todo_list *todo) {
    printf("Welcome to CS ToDo!\n");

    printf("Enter Command: ");

    char command;
    while (scanf(" %c", &command) == 1) {
        if (command == COMMAND_ADD_TASK) {
            add_task(todo);
        } else if (command == COMMAND_PRINT_TASKS) {
            print_tasks(todo);
        } else if (command == COMMAND_UPDATE_PRIORITY) {
            update_priority(todo);
        } else if (command == COMMAND_COUNT_TASKS) {
            count_tasks(todo);
        } else if (command == COMMAND_COMPLETE_TASK) {
            complete_task(todo);
        } else if (command == COMMAND_PRINT_COMPLETE) {
            print_complete(todo);
        } else if (command == COMMAND_ESTIMATE_TIME) {
            estimate_time(todo);
        } else if (command == COMMAND_DELETE_TASK) {
            delete_task(todo);
        } else if (command == COMMAND_FINISH_DAY) {
            finish_day(todo);
        } else if (command == COMMAND_REPEAT_TASK) {
            repeat_task(todo);
        } else if (command == COMMAND_MATCH_TASK) {
            match_task(todo);
        } else if (command == COMMAND_DELETE_MATCH) {
            delete_match(todo);
        } else if (command == COMMAND_SORT_TASKS) {
            sort_tasks(todo);
        }

        printf("Enter Command: ");
    }

    printf("All done!\n");

    free_list(todo);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////// YOUR HELPER FUNCTIONS ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Make the tasks and completed tasks pointers within todo point to NULL
void initialise_todo(struct todo_list *todo) {
    todo->completed_tasks = NULL;
    todo->tasks = NULL;
}

// Adds a new task to the end of the current tasks list in the provided todo list
void add_task(struct todo_list *todo) {
    struct task *new_task = malloc(sizeof(struct task));
    
    // Create a string to scan the entire command input into.
    char buffer[MAX_STRING_LENGTH];
    fgets(buffer, MAX_STRING_LENGTH, stdin);

    // Create variables for each part of the command being scanned in
    // (name of task, category of task and priority of task)
    char task_name[MAX_TASK_LENGTH];
    char task_category[MAX_CATEGORY_LENGTH];
    enum priority task_priority;
    parse_add_task_line(buffer, task_name, task_category, &task_priority);

    strcpy(new_task->task_name, task_name);
    strcpy(new_task->category, task_category);

    new_task->priority = task_priority;
    new_task->next = NULL;
    new_task->repeatable = 0;
    
    struct task *current_task = todo->tasks;

    if (current_task == NULL) {
        todo->tasks = new_task;
    } else {
        while (current_task->next != NULL) {
            current_task = current_task->next;
        }
        current_task->next = new_task;
    }
}

// Print the list of uncompleted tasks from the todo list
void print_tasks(struct todo_list *todo) {
    int i = 1;
    struct task *current_task = todo->tasks;

    printf("==== Your ToDo List ====\n");

    while (current_task != NULL) {
        print_one_task(i, current_task);

        i++;
        current_task = current_task->next;
    }

    if (i == 1) {
        printf("All tasks completed, you smashed it!\n");
    }

    printf("====   That's it!   ====\n");
}

// Increase the priority of a task, if it is at high change it to low instead
void update_priority(struct todo_list *todo) {
    // Fetch `[task] [category]` from stdin
    char buffer[MAX_STRING_LENGTH];
    fgets(buffer, MAX_STRING_LENGTH, stdin);

    // Create strings for `task`/`category` and populate them using the contents
    // of `buffer`
    char task[MAX_TASK_LENGTH];
    char category[MAX_CATEGORY_LENGTH];
    parse_task_category_line(buffer, task, category);

    struct task *current_task = todo->tasks;
    int end_loop = 0;
    
    while (current_task != NULL && !end_loop) {
        if (strcmp(current_task->task_name, task) == 0) {
            if (strcmp(current_task->category, category) == 0) {
                current_task->priority = (current_task->priority + 1) % 3;
                end_loop = 1;
            }
        } else {
            current_task = current_task->next;
        }
    }

    if (!end_loop) {
        printf("Could not find task '%s' in category '%s'.\n", task, category);
    }
}

// Count the number of tasks that haven't been completed in the todo list
void count_tasks(struct todo_list *todo) {
    struct task *current_task = todo->tasks;
    int count = 0;
    
    while (current_task != NULL) {
        current_task = current_task->next;
        count++;
    }
    printf("There are %d items on your list!\n", count);
}

// Identify and move a task to the completed task list
void complete_task(struct todo_list *todo) {
    struct completed_task *completed_task = malloc(sizeof(struct completed_task));
    completed_task->next = todo->completed_tasks;
    completed_task->task = NULL;
    
    // Fetch `[task] [category] [start_time] [finish_time]` from stdin
    char buffer[MAX_STRING_LENGTH];
    fgets(buffer, MAX_STRING_LENGTH, stdin);

    // Create strings for `task`/`category` and ints for times, then populate
    // them using the contents of `buffer`.
    char task[MAX_TASK_LENGTH];
    char category[MAX_CATEGORY_LENGTH];
    int start_time;
    int finish_time;

    parse_complete_task_line(buffer, task, category, &start_time, &finish_time);

    comp_time(todo, completed_task, start_time, finish_time);

    int found_task = comp_mv(todo, completed_task, category, task);

    if (!found_task) {
        printf("Could not find task '%s' in category '%s'.\n", task, category);

        free(completed_task);
    } else {
        todo->completed_tasks = completed_task;
    }
}

// Set the completion time of the task
void comp_time(struct todo_list *todo, struct completed_task *comp, int start, int fin) {
    if (start < 0) {
        struct completed_task *current_complete = todo->completed_tasks;
        int latest_time = 0;

        while (current_complete != NULL) {
            if (current_complete->finish_time > latest_time) {
                latest_time = current_complete->finish_time;
            }

            current_complete = current_complete->next;
        }

        comp->start_time = latest_time;
    } else {
        comp->start_time = start;
    }

    comp->finish_time = fin;
}

// Move the task to the completed task list
int comp_mv(struct todo_list *todo, struct completed_task *comp, char *cat, char *task) {
    struct task *current_task = todo->tasks;
    struct task *previous_task = todo->tasks;
    int end_loop = 0;

    while (current_task != NULL && !end_loop) {
        if (strcmp(current_task->task_name, task) == 0) {
            if (strcmp(current_task->category, cat) == 0) {
                comp->task = current_task;
                remove_task(todo, current_task, previous_task);

                end_loop = 1;
            }
        } else {
            previous_task = current_task;
            current_task = current_task->next;
        }
    }

    return end_loop;
}

// Remove a task from the task part of the todo list
void remove_task(struct todo_list *todo, struct task *curr, struct task *prev) {
    if (curr == todo->tasks) {
        todo->tasks = todo->tasks->next;
    } else {
        prev->next = curr->next;
    }
}

// Print the completed tasks for the day
void print_complete(struct todo_list *todo) {
    struct completed_task *current_task = todo->completed_tasks;

    printf("==== Completed Tasks ====\n");

    if (current_task == NULL) {
        printf("No tasks have been completed today!\n");
    } else {
        while (current_task != NULL) {
            print_completed_task(current_task);

            current_task = current_task->next;
        }
    }

    printf("=========================\n");
}

// Estimate the completion time for the remaining tasks and print the results
void estimate_time(struct todo_list *todo) {
    struct task *current_task = todo->tasks;
    struct completed_task *current_complete = todo->completed_tasks;

    char category[MAX_CATEGORY_LENGTH];
    int i = 1;

    printf("Expected completion time for remaining tasks:\n\n");

    while (current_task != NULL) {
        strcpy(category, current_task->category);

        int estimate = get_estimate(current_complete, category);

        print_one_task(i, current_task);
        printf("Expected completion time: %d minutes\n\n", estimate);

        i++;
        current_task = current_task->next;
        current_complete = todo->completed_tasks;
    }
}

// Find the estimated time to complete each task
int get_estimate(struct completed_task *comp, char *category) {
    int count = 0;
    int total = 0;
    int estimate = 0;

    while (comp != NULL) {
        if (strcmp(comp->task->category, category) == 0) {
            count++;
            total += comp->finish_time - comp->start_time;
        }
        comp = comp->next;
    }

    if (count > 0) {
        estimate = total / count;
    } else {
        estimate = 100;
    }

    return estimate;
}

// Delete a task from the todo list
void delete_task(struct todo_list *todo) {
    // Fetch `[task] [category]` from stdin
    char buffer[MAX_STRING_LENGTH];
    fgets(buffer, MAX_STRING_LENGTH, stdin);

    // Create strings for `task`/`category` and populate them using the contents
    // of `buffer`
    char task[MAX_TASK_LENGTH];
    char category[MAX_CATEGORY_LENGTH];
    parse_task_category_line(buffer, task, category);

    struct task *current_task = todo->tasks;
    struct task *previous_task = todo->tasks;
    int end_loop = 0;
    
    while (current_task != NULL && !end_loop) {
        if (strcmp(current_task->task_name, task) == 0) {
            if (strcmp(current_task->category, category) == 0) {
                remove_task(todo, current_task, previous_task);
                free(current_task);
                
                end_loop = 1;
            }
        } else {
            previous_task = current_task;
            current_task = current_task->next;
        }
    }

    if (!end_loop) {
        printf("Could not find task '%s' in category '%s'.\n", task, category);
    }
}

// Finish the day and clear the completed tasks
// Place the repeatable tasks back in the tasks list
void finish_day(struct todo_list *todo) {
    struct completed_task *current_complete = todo->completed_tasks;
    struct completed_task *delete = current_complete;
    struct task *current_task = todo->tasks;

    todo->completed_tasks = NULL;

    while (current_complete != NULL) {
        if (current_complete->task->repeatable) {
            repeatable(todo, current_complete, current_task);

            if (current_task == NULL) {
                current_task = todo->tasks;
            }
        } else {
            free(delete->task);
        }
        
        current_complete = current_complete->next;

        free(delete);

        delete = current_complete;
    }
}

// Move the repeatable tasks back to the task list from the completed task list
void repeatable(struct todo_list *todo, struct completed_task *comp, struct task *curr) {
    if (curr == NULL) {
        todo->tasks = comp->task;
        todo->tasks->next = NULL;
    } else {
        while (curr->next != NULL) {
            curr = curr->next;
        }

        curr->next = comp->task;
        curr->next->next = NULL;
    }
}

// Set a task to repeatable, or not-repeatable if it was already
void repeat_task(struct todo_list *todo) {
    // Fetch `[task] [category]` from stdin
    char buffer[MAX_STRING_LENGTH];
    fgets(buffer, MAX_STRING_LENGTH, stdin);

    // Create strings for `task`/`category` and populate them using the contents
    // of `buffer`
    char task[MAX_TASK_LENGTH];
    char category[MAX_CATEGORY_LENGTH];
    parse_task_category_line(buffer, task, category);

    struct task *current_task = todo->tasks;
    int end_loop = 0;
    
    while (current_task != NULL && !end_loop) {
        if (strcmp(current_task->task_name, task) == 0) {
            if (strcmp(current_task->category, category) == 0) {
                current_task->repeatable = (current_task->repeatable + 1) % 2;
                
                end_loop = 1;
            }
        } else {
            current_task = current_task->next;
        }
    }

    if (!end_loop) {
        printf("Could not find task '%s' in category '%s'.\n", task, category);
    }
}

// Free the full todo list
void free_list(struct todo_list *todo) {
    struct task *current_task = todo->tasks;
    struct completed_task *complete_task = todo->completed_tasks;

    struct task *delete = current_task;
    struct completed_task *delete_complete = complete_task;

    while (current_task != NULL) {
        current_task = current_task->next;

        free(delete);

        delete = current_task;
    }

    while (complete_task != NULL) {
        complete_task = complete_task->next;

        free(delete_complete->task);
        free(delete_complete);

        delete_complete = complete_task;
    }

    free(todo);
}

// Print a list of matching tasks based on the given pattern properties
void match_task(struct todo_list *todo) {
    struct task *current_task = todo->tasks;
    char task[MAX_TASK_LENGTH];

    char pattern[MAX_STRING_LENGTH];
    fgets(pattern, MAX_STRING_LENGTH, stdin);
    trim_whitespace(pattern);

    int task_number = 1;
    int i = 0;
    int j = 0;
    int print_task = 1;

    printf("Tasks matching pattern '%s':\n", pattern);

    while (current_task != NULL) {
        strcpy(task, current_task->task_name);

        pattern_loop(&i, &j, pattern, task, &print_task);

        if (print_task) {
            print_one_task(task_number, current_task);
            task_number++;
        }

        print_task = 1;
        current_task = current_task->next;
    }
}

// Get the final output on whether a task matches a pattern and should be printed
void pattern_loop(int *i, int *j, char *pattern, char *task, int *print_task) {
    while (*j < strlen(task) && *print_task) {
        check_pattern(i, j, pattern, task, print_task);
    }

    if (*i < strlen(pattern)) {
        *print_task = 0;
    }

    *i = 0;
    *j = 0;
}

// Completely check a task name to see if it matches a pattern
void check_pattern(int *i, int *j, char *pattern, char *task, int *print_task) {
    if (pattern[*i] == '*') {
        star_pattern(i, j, pattern, task, print_task);
    } else if (pattern[*i] == '?') {
        (*j)++;
        (*i)++;
    } else if (pattern[*i] == '[') {
        bracket_pattern(i, j, task, pattern, print_task);
    } else {
        if (pattern[*i] != task[*j]) {
            *print_task = 0;
        }

        (*j)++;
        (*i)++;
    }
}

// Perform the matching sequence when provided with *
// Finds the tasks with the same sequence of letters as what follows *
void star_pattern (int *i, int *j, char *pattern, char *task, int *print_task) {
    (*i)++;

    int is_match = 0;
    int match_count = 0;
    char match[MAX_STRING_LENGTH];

    while (isalnum(pattern[*i]) > 0) {
        match[match_count] = pattern[*i];

        (*i)++;
        match_count++;
    }

    match[match_count] = '\0';

    if (pattern[*i] == '\0') {
        end_star(&match_count, j, match, task, &is_match);
    }

    star(&match_count, j, match, task, &is_match);

    if (is_match != 1) {
        *print_task = 0;
    }
}

// If * is the last match pattern given then it goes backwards through the task name
void end_star(int *match_count, int *j, char *match, char *task, int *is_match) {
    *match_count = strlen(match) - 1;
    *j = strlen(task) - 1;
    
    while (*match_count >= 0 && !*is_match) {
        if (match[*match_count] != task[*j]) {
            *is_match = -1;
        } else {
            (*match_count)--;
            (*j)--;
        }
    }

    if (!*is_match) {
        *is_match = 1;
    }

    *j = strlen(task);
}

// If * isnt the last pattern indicator then goes forwards through the task name
void star(int *match_count, int *j, char *match, char *task, int *is_match) {
    while (*j + strlen(match) <= strlen(task) && !*is_match) {
        *match_count = 0;

        while (match[*match_count] == task[*j] && !*is_match) {
            if (*match_count == strlen(match) - 1) {
                *is_match = 1;
            } else {
                (*match_count)++;
                (*j)++;
            }
        }

        (*j)++;
    }
}

// If a [] pattern is given then see if the task has any of the included characters
void bracket_pattern(int *i, int *j, char *task, char *pattern, int *print_task) {
    (*i)++;

    int match_count = 0;
    int in_any = 0;
    char any_match[MAX_STRING_LENGTH];

    while (pattern[*i] != ']') {
        any_match[match_count] = pattern[*i];

        (*i)++;
        match_count++;
    }

    any_match[match_count] = '\0';

    match_count = 0;

    while (match_count < strlen(any_match) && !in_any) {
        if (any_match[match_count] == task[*j]) {
            in_any = 1;
        } else {
            match_count++;
        }
    }

    if (!in_any) {
        *print_task = 0;
    }

    (*j)++;
    (*i)++;
}

// Delete tasks that match based on the given pattern properties
void delete_match(struct todo_list *todo) {
    struct task *current_task = todo->tasks;
    struct task *previous_task = todo->tasks;
    char task[MAX_TASK_LENGTH];

    char pattern[MAX_STRING_LENGTH];
    fgets(pattern, MAX_STRING_LENGTH, stdin);
    trim_whitespace(pattern);

    int i = 0;
    int j = 0;
    int match = 1;

    while (current_task != NULL) {
        strcpy(task, current_task->task_name);

        pattern_loop(&i, &j, pattern, task, &match);

        if (match) {
            struct task *delete = current_task;
            remove_task(todo, current_task, previous_task);

            current_task = current_task->next;

            free(delete);
        } else {
            previous_task = current_task;
            current_task = current_task->next;
        }

        match = 1;
    }
}

// Sort the tasks based on category, then priority, then by name
void sort_tasks(struct todo_list *todo) {
    struct task *insert_task = todo->tasks->next;

    struct todo_list *new_list = malloc(sizeof(struct todo_list));
    initialise_todo(new_list);
    new_list->tasks = todo->tasks;
    new_list->tasks->next = NULL;

    while (insert_task != NULL) {
        insert_task = sort(insert_task, new_list);
    }

    todo->tasks = new_list->tasks;
    free(new_list);
}

// Search for the correct position in the list to insert the task
struct task *sort(struct task *insert_task, struct todo_list *new_list) {
    int end_loop = 0;
    int task_order;

    struct task *compare_task = new_list->tasks;
    struct task *previous_task = new_list->tasks;

    while (!end_loop) {
        task_order = task_compare(insert_task, compare_task);

        if (task_order > 0) {
            previous_task = compare_task;
            compare_task = compare_task->next;

            if (compare_task == NULL) {
                insert_task = insert(insert_task, compare_task, previous_task);

                end_loop = 1;
            }
        } else if (compare_task == new_list->tasks) {
            new_list->tasks = insert_task;

            insert_task = insert_task->next;
            new_list->tasks->next = compare_task;
            end_loop = 1;
        } else {
            insert_task = insert(insert_task, compare_task, previous_task);

            end_loop = 1;
        }
    }

    return insert_task;
}

// Insert the task at the current point of the list
struct task *insert(struct task *insert, struct task *compare, struct task *previous) {
    previous->next = insert;
    insert = insert->next;
    previous->next->next = compare;

    return insert;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////// PROVIDED HELPER FUNCTIONS //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * Helper Function
 * You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
 *
 * Given a raw string in the format: [string1] [string2] [enum priority]
 * This function will extract the relevant values into the given variables.
 * The function will also remove any newline characters.
 *
 * For example, if given: "finish_assignment_2 assignment2 high"
 * The function will copy the string:
 *     "finish_assignment_2" into the `task_name` array
 * Then copy the string:
 *     "assignment2" into the `task_category` array
 * And finally, copy over the enum:
 *     "high" into the memory that `prio` is pointing at.
 *
 * Parameters:
 *     buffer        - A null terminated string in the following format
 *                     [string1] [string2] [enum priority]
 *     task_name     - A character array for the [string1] to be copied into
 *     task_category - A character array for the [string2] to be copied into
 *     prio          - A pointer to where [enum priority] should be stored
 *
 * Returns:
 *     None
 */
void parse_add_task_line(
    char buffer[MAX_STRING_LENGTH],
    char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH],
    enum priority *prio
) {
    remove_newline(buffer);

    // Extract value 1 as string
    char *name_str = strtok(buffer, " ");
    if (name_str != NULL) {
        strcpy(task_name, name_str);
    }

    // Extract value 2 as string
    char *category_str = strtok(NULL, " ");
    if (category_str != NULL) {
        strcpy(task_category, category_str);
    }
    
    // Extract value 3 as string
    char *prio_str = strtok(NULL, " ");
    if (prio_str != NULL) {
        *prio = string_to_priority(prio_str);
    }

    if (
        name_str == NULL ||
        category_str == NULL ||
        prio_str == NULL ||
        *prio == INVALID_PRIORITY
    ) {
        // If any of these are null, there were not enough words.
        printf("Could not properly parse line: '%s'.\n", buffer);
    }
}

/*
 * Helper Function
 * You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
 *
 * See `parse_add_task_line` for explanation - This function is very similar,
 * with only the exclusion of an `enum priority`.
 */
void parse_task_category_line(
    char buffer[MAX_STRING_LENGTH],
    char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH]
) {
    remove_newline(buffer);

    // Extract value 1 as string
    char *name_str = strtok(buffer, " ");
    if (name_str != NULL) {
        strcpy(task_name, name_str);
    }

    // Extract value 2 as string
    char *category_str = strtok(NULL, " ");
    if (category_str != NULL) {
        strcpy(task_category, category_str);
    }

    if (name_str == NULL || category_str == NULL) {
        // If any of these are null, there were not enough words.
        printf("Could not properly parse line: '%s'.\n", buffer);
    }
}

/*
 * Helper Function
 * You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
 *
 * See `parse_add_task_line` for explanation - This function is very similar,
 * with only the exclusion of an `enum priority` and addition of start/end times
 */
void parse_complete_task_line(
    char buffer[MAX_STRING_LENGTH],
    char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH],
    int *start_time,
    int *finish_time
) {
    remove_newline(buffer);

    // Extract value 1 as string
    char *name_str = strtok(buffer, " ");
    if (name_str != NULL) {
        strcpy(task_name, name_str);
    }

    // Extract value 2 as string
    char *category_str = strtok(NULL, " ");
    if (category_str != NULL) {
        strcpy(task_category, category_str);
    }
    
    // Extract value 2 as string
    char *start_str = strtok(NULL, " ");
    if (start_str != NULL) {
        *start_time = atoi(start_str);
    }
    
    // Extract value 2 as string
    char *finish_str = strtok(NULL, " ");
    if (finish_str != NULL) {
        *finish_time = atoi(finish_str);
    }

    if (
        name_str == NULL ||
        category_str == NULL ||
        start_str == NULL ||
        finish_str == NULL
    ) {
        // If any of these are null, there were not enough words.
        printf("Could not properly parse line: '%s'.\n", buffer);
    }
}

/**
 * Helper Function
 * You should not need to change this function.
 *
 * Given a raw string, will return the corresponding `enum priority`,
 * or INVALID_PRIORITY if the string doesn't correspond with the enums.
 *
 * Parameters:
 *     priority - string representing the corresponding `enum priority` value
 * Returns:
 *     enum priority
 */
enum priority string_to_priority(char priority[MAX_STRING_LENGTH]) {
    if (strcmp(priority, "low") == 0) {
        return LOW;
    } else if (strcmp(priority, "medium") == 0) {
        return MEDIUM;
    } else if (strcmp(priority, "high") == 0) {
        return HIGH;
    }

    return INVALID_PRIORITY;
}

/**
 * Helper Function
 * You should not need to change this function.
 *
 * Given an priority and a character array, fills the array with the
 * corresponding string version of the priority.
 *
 * Parameters:
 *     prio - the `enum priority` to convert from
 *     out  - the array to populate with the string version of `prio`.
 * Returns:
 *     Nothing
 */
void priority_to_string(enum priority prio, char out[MAX_STRING_LENGTH]) {
    if (prio == LOW) {
        strcpy(out, "LOW");
    } else if (prio == MEDIUM) {
        strcpy(out, "MEDIUM");
    } else if (prio == HIGH) {
        strcpy(out, "HIGH");
    } else {
        strcpy(out, "Provided priority was invalid");
    }
}

/*
 * Helper Function
 * You should not need to change this function.
 *
 * Given a raw string will remove and first newline it sees.
 * The newline character wil be replaced with a null terminator ('\0')
 *
 * Parameters:
 *     input - The string to remove the newline from
 *
 * Returns:
 *     Nothing
 */
void remove_newline(char input[MAX_STRING_LENGTH]) {
    // Find the newline or end of string
    int index = 0;
    while (input[index] != '\n' && input[index] != '\0') {
        index++;
    }
    // Goto the last position in the array and replace with '\0'
    // Note: will have no effect if already at null terminator
    input[index] = '\0';
}

/*
 * Helper Function
 * You likely do not need to change this function.
 *
 * Given a raw string, will remove any whitespace that appears at the start or
 * end of said string.
 *
 * Parameters:
 *     input - The string to trim whitespace from
 *
 * Returns:
 *     Nothing
 */
void trim_whitespace(char input[MAX_STRING_LENGTH]) {
    remove_newline(input);
    
    int lower;
    for (lower = 0; input[lower] == ' '; ++lower);
    
    int upper;
    for (upper = strlen(input) - 1; input[upper] == ' '; --upper);
    
    for (int base = lower; base <= upper; ++base) {
        input[base - lower] = input[base];
    }

    input[upper - lower + 1] = '\0';
}

/**
 * Helper Function
 * You SHOULD NOT change this function.
 *
 * Given a task, prints it out in the format specified in the assignment.
 *
 * Parameters:
 *     task_num - The position of the task within a todo list
 *     task     - The task in question to print
 *
 * Returns:
 *     Nothing
 */
void print_one_task(int task_num, struct task *task) {
    char prio_str[MAX_STRING_LENGTH];
    priority_to_string(task->priority, prio_str);

    printf(
        "  %02d. %-30.30s [ %s ] %s\n",
        task_num, task->task_name, task->category, prio_str
    );

    int i = 30;
    while (i < strlen(task->task_name)) {
        printf("      %.30s\n", task->task_name + i);
        i += 30;
    }
}

/**
 * Helper Function
 * You SHOULD NOT change this function.
 *
 * Given a completed task, prints it out in the format specified in the
 * assignment.
 *
 * Parameters:
 *     completed_task - The task in question to print
 *
 * Returns:
 *     Nothing
 */
void print_completed_task(struct completed_task *completed_task) {
    int start_hour = completed_task->start_time / 60;
    int start_minute = completed_task->start_time % 60;
    int finish_hour = completed_task->finish_time / 60;
    int finish_minute = completed_task->finish_time % 60;
    
    printf(
        "  %02d:%02d-%02d:%02d | %-30.30s [ %s ]\n",
        start_hour, start_minute, finish_hour, finish_minute,
        completed_task->task->task_name, completed_task->task->category
    );

    int i = 30;
    while (i < strlen(completed_task->task->task_name)) {
        printf("      %.30s\n", (completed_task->task->task_name + i));
        i += 30;
    }
}

/**
 * Compares two tasks by precedence of category -> priority -> name and returns
 * an integer referring to their relative ordering
 * 
 * Parameters:
 *     t1 - The first task to compare
 *     t2 - The second task to compare
 *
 * Returns:
 *     a negative integer if t1 belongs before t2
 *     a positive integer if t1 belongs after t2
 *     0 if the tasks are identical (This should never happen in your program)
 */
int task_compare(struct task *t1, struct task *t2) {
    int category_diff = strcmp(t1->category, t2->category);
    if (category_diff != 0) {
        return category_diff;
    }
    
    int priority_diff = t2->priority - t1->priority;
    if (priority_diff != 0) {
        return priority_diff;
    }
    
    return strcmp(t1->task_name, t2->task_name);
}
