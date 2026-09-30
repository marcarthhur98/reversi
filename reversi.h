#ifndef REVERSI_H
#define REVERSI_H
#include <stdbool.h>
#include <stdint.h>
#define SIZE 8
#define STRIDE 26
#define WIN_SCORE 1000000.0
typedef char Board[STRIDE][STRIDE];
typedef struct { int row, col; double score; uint64_t nodes, cutoffs; } SearchResult;
char getOpponent(char turn);
void initialiseBoard(char board[][26], int n);
void printBoard(char board[][26], int n);
bool positionInBounds(int n, int row, int col);
bool checkLegalInDirection(const char board[][26], int n, int row, int col, char colour, int dr, int dc);
bool moveIsValid(const char board[][26], int n, int row, int col, char colour);
int getValidMoves(const char board[][26], int n, char turn, char moves[64][2]);
int countPieces(const char board[][26], int n, char colour);
int countEdgeStable(const char board[][26], int n, char colour);
int moveScore(const char board[][26], int n, int row, int col, char colour);
void playMove(char board[][26], int n, int row, int col, char colour);
double evaluateBoard(const char board[][26], int n, char colour);
SearchResult searchBest(const char board[][26], char turn, int depth, bool pruning);
bool greedyMove(const char board[][26], char turn, int *row, int *col);
/* Standalone adapter: 1 = move; 0 = pass/invalid input, outputs set to -1. */
int makeMove(const char board[][26], int n, char turn, int *row, int *col);
#endif
