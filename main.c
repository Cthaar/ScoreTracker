/*
 * Programmer: Charlotte Thaarup       Date Completed: 15/12-2020
 * Early project  in C from first semester.
 *
 * This program will read a file containing football games from 2019-2020, and
 * sort them in regards to who has most points. IF two or more teams have the
 * same amount of points, it will sort for the ones who have scored more goals
 * versus how many have been scored against them
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Globals
#define MAX_GAMES 182
#define MAX_TEAMS 14
#define NON_EXISTING -1

// Structs
struct games {
    char day[MAX_GAMES];
    char date[MAX_GAMES];
    char time[MAX_GAMES];
    char team1[MAX_GAMES];
    char team2[MAX_GAMES];
    int goals1;
    int goals2;
    int spectators;
};

struct team_tracker {
    char team_name[MAX_TEAMS];
    int total_points;
    int goals_scored;
    int goals_against;
    int goals_difference;
};

// Function Prototypes
void read_file(struct games information[]);
void construct_teams(struct team_tracker teams_array[], char **team_names);
void game_results(struct games games, struct team_tracker team_array[]);
void loop_games(struct games games[], struct team_tracker teams[]);
int next_index(int i);
int index_of_teams(char *team_names, struct team_tracker team_array[]);
void print_team_results(struct team_tracker team_array[]);
int match_won(int goals_scored, int goals_against);
int team_compare(const void *p1, const void *p2);
int is_empty(struct team_tracker tt);

int main() {
    struct games games[MAX_GAMES];
    struct team_tracker teams[MAX_TEAMS];

    /* Initializing the 14 teams in an array */
    char *team_names[] = {"FCM", "EFB", "SDR", "RFC", "ACH", "FCN", "LBK",
                          "AaB", "OB",  "FCK", "BIF", "SIF", "HOB", "AGF"};

    read_file(games);
    construct_teams(teams, team_names);
    loop_games(games, teams);
    print_team_results(teams);
    return 0;
}

void read_file(struct games information[]) {
    /* Opening the file containing all the games */
    FILE *f = fopen("kampe-2019-2020.txt", "r");
    /* Checking if file opens correctly, else it gives a warning */
    if (f == NULL) {
        printf("File cannot open, bye!");
        exit(EXIT_FAILURE);
    }

    int i = 0;

    /*
     * Looping through the file line by line using fscanf and scansets, to split
     * the read informaion
     */
    for (i = 0; i < MAX_GAMES; i++) {
        fscanf(f, " %[A-Za-z ]", information[i].day);
        fscanf(f, " %[/0-9] ", information[i].date);
        fscanf(f, "%[ .0-9]", information[i].time);
        fscanf(f, " %s", information[i].team1);
        fscanf(f, " -%s", information[i].team2);
        fscanf(f, " %d", &information[i].goals1);
        fscanf(f, " -%d", &information[i].goals2);
        fscanf(f, " %d\n", &information[i].spectators);
    }
    /* Closing the files to protect against output data loss */
    fclose(f);
}

/* 
 * Using ** to create a pointer from team_names in this function, to the 
 * pointer team_names in main 
 */
void construct_teams(struct team_tracker teams_array[], char **team_names) {
    int i;
    /*
     * Running through all the teams to set all data to zero, to avoid errors in
     * later calulations
     */
    for (i = 0; i < MAX_TEAMS; i++) {
        strcpy(teams_array[i].team_name, team_names[i]);
        teams_array[i].total_points = 0;
        teams_array[i].goals_scored = 0;
        teams_array[i].goals_against = 0;
    }
}

void game_results(struct games games, struct team_tracker team_array[]) {
    /* Function used to check goals and points to each team */
    int i;
    int index_team1, index_team2;

    /*
     * Used to find the two team in the array, and add the correct points and
     * goals to each team on the correct index space
     */
    index_team1 = index_of_teams(games.team1, team_array);
    index_team2 = index_of_teams(games.team2, team_array);

    /* Adding each teams goals to their index */
    team_array[index_team1].goals_scored += games.goals1;
    team_array[index_team2].goals_scored += games.goals2;

    /* Adding goals against each team to their index */
    team_array[index_team1].goals_against += games.goals2;
    team_array[index_team2].goals_against += games.goals1;

    /*
     * Checking which team won the game, in order to give them the correct
     * amount of points
     */
    if (games.goals1 > games.goals2) {
        team_array[index_team1].total_points += 3;
    } else if (games.goals1 < games.goals2) {
        team_array[index_team2].total_points += 3;
    } else if (games.goals1 == games.goals2) {
        team_array[index_team1].total_points += 1;
        team_array[index_team2].total_points += 1;
    }

    /*
     * Checking how many goals each team has scored, against how many there have
     * been scored against them, in order to be able to sort them by this later
     * in the program
     */
    for (i = 0; i < MAX_TEAMS; ++i) {
        team_array[i].goals_difference =
            team_array[i].goals_scored - team_array[i].goals_against;
    }
}

void loop_games(struct games games[], struct team_tracker teams[MAX_TEAMS]) {
    /*
     * Looping through all games, to be able to give the correct points, and
     * goals to each team, before printing the final result
     */
    int i;
    for (i = 0; i < 182; i++) {
        game_results(games[i], teams);
    }
    /* Sorting the points and goal difference */
    qsort(teams, MAX_TEAMS, sizeof(struct team_tracker), team_compare);
}

/* Using ternary operator to move around in the index */
int next_index(int i) { 
    return (i < MAX_TEAMS - 1) ? i + 1 : 0; 
}

int index_of_teams(char *team_names, struct team_tracker team_array[]) {
    int i = 0;
    /*
     * Checking if the index-space is empty. If it isn't empty, in will continue
     * to next index space and check again, until it finds an empty space
     */
    while (!is_empty(team_array[i]) &&
           strcmp(team_names, team_array[i].team_name) != 0) { /* No name hit */
        i = next_index(i);
    }

    /*
     * If either the index is empty (if) or have a match in name (else if) it
     * will return -1. Index should not be empty, as we have already initialized
     * the array in main
     */
    if (is_empty(team_array[i]))
        return -1;
    else if (strcmp(team_names, team_array[i].team_name) == 0)
        return i;
    else
        return -1;
}

int team_compare(const void *p1, const void *p2) {
    /* Function used to run qsort */
    int compare;

    struct team_tracker *team1 = (struct team_tracker *)p1;
    struct team_tracker *team2 = (struct team_tracker *)p2;

    /*
     * First sorting points, here it checks if team1 points is larger than
     * team2's
     * */
    if (team1->total_points < team2->total_points) {
        compare = 1;
    } else if (team1->total_points > team2->total_points) {
        compare = -1;
    }
    /*
     * If two team has the same amount of points, it then checks who has the
     * biggest goal difference
     */
    else if (team1->total_points == team2->total_points &&
             team1->goals_difference < team2->goals_difference) {
        compare = 1;
    } else if (team1->total_points == team2->total_points &&
               team1->goals_difference > team2->goals_difference) {
        compare = -1;
    } else
        compare = 0;

    return compare;
}

void print_team_results(struct team_tracker team_array[]) {
    printf("  Teams   Points   Goals   Goals against\n");
    printf("------------------------------------------\n");
    for (int i = 0; i < MAX_TEAMS; ++i) {
        printf("   %-3s", team_array[i].team_name);
        printf("%8d", team_array[i].total_points);
        printf("%8d", team_array[i].goals_scored);
        printf("%8d\n", team_array[i].goals_against);
    }
}

int is_empty(struct team_tracker tt) { 
    return (tt.total_points == -1); 
}
