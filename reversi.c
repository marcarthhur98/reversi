#include "reversi.h"
#include <stdio.h>
#include <string.h>

// Utility helpers
char getOpponent(char turn);
void copyBoard(const char board[][26], char boardCopy[][26], int n);

// Move generation
int getValidMoves(const char board[][26], int n, char turn, char moves[64][2]);

// Evaluation feature functions
int countPieces(const char board[][26], int n, char colour);
int countMobility(const char board[][26], int n, char colour);
int countCorners(const char board[][26], int n, char colour);
int countDangersSquares(const char board[][26], int n, char colour);
int countEdgeOcc(const char board[][26], int n, char colour);
int countEdgeStable(const char board[][26], int n, char colour);

// Evaluation
double normalisedDifference(int myValue, int oppValue);
double evaluateBoard(const char board[][26], int n, char myColour);

// Board setup / display
void initialiseBoard(char board[][26], int n);
void printBoard(char board[][26], int n);

// Move validation
bool positionInBounds(int n, int row, int col);
bool checkLegalInDirection(const char board[][26], int n, int row, int col,
                           char colour, int deltaRow, int deltaCol);
bool moveIsValid(const char board[][26], int n, int row, int col, char colour);

// Move scoring (for flips)
int countscoreForDirection(const char board[][26], int n, int row, int col,
                           char colour, int deltaRow, int deltaCol);
int moveScore(const char board[][26], int n, int row, int col, char colour);

// Playing moves (these modify the board → NOT const)
void flipInDirection(char board[][26], int n, int row, int col,
                     char colour, int deltaRow, int deltaCol);
void playMove(char board[][26], int n, int row, int col, char colour);
char getOpponent(char turn){
    if(turn == 'W'){
        return 'B';
    }else{
    return 'W';
    }
}
void copyBoard(const char board[][26], char boardCopy[][26], int n){
    for(int i = 0; i < n;i++){
        for(int j=0; j < n;j++){
            boardCopy[i][j] = board[i][j];
        }
    }

}

int getValidMoves(const char board[][26],int n, char turn, char moves[64][2]){
    int count =0;
    for(int i = 0; i < n;i++){
        for(int j = 0; j < n;j++){
            if(moveIsValid(board,n,i,j,turn)){
                moves[count][0] = i;
                moves[count][1] = j;
                count++;
            }
        }
    }
return count;
}

int countPieces(const char board[][26], int n, char colour){
    int count = 0;
    for(int i = 0; i < n;i++){
        for(int j=0; j < n;j++){
            if(board[i][j] == colour){
                count++;
            }
        }
    }
return count;
}

int countMobility(const char board[][26], int n, char colour){
    char moves[64][2];
    return getValidMoves(board,n,colour,moves);
}

int countCorners(const char board[][26], int n, char colour){
    int count = 0;
    if(board[0][0] == colour){
        count++;
    }
    if(board[0][n-1] == colour){
        count++;
    }
    if(board[n-1][n-1] == colour){
        count++;
    }
    if(board[n-1][0] == colour){
        count++;
    }
return count;
    
}

int countDangersSquares(const char board[][26], int n, char colour){
int count = 0; 

//top left corner 
if(board[0][0] == 'U'){
    if(board[0][1] == colour){count++;}
    if(board[1][1] == colour){count++;}
    if(board[1][0] == colour){count++;}
}

// top right corner
if(board[0][n-1] == 'U'){
    if(board[0][n-2] == colour){count++;}
    if(board[1][n-1] == colour){count++;}
    if(board[1][n-2] == colour){count++;}
}

// bottom left corner
if(board[n-1][0] == 'U'){
    if(board[n-2][0] == colour){count++;}
    if(board[n-1][1] == colour){count++;}
    if(board[n-2][1] == colour){count++;}
}

// bottom right corner 
if(board[n-1][n-1] == 'U'){
    if(board[n-2][n-2] == colour){count++;}
    if(board[n-1][n-2] == colour){count++;}
    if(board[n-2][n-1] == colour){count++;}
}

return count;
}

int countEdgeOcc( const char board[][26], int n, char colour){
    int count = 0;
    // top edge with no corners
    for(int col = 1; col < n-1; col++){
        if(board[0][col] == colour){
            count++;
        }
    }
    // bottom edge with no corner
    for(int col = 1; col < n-1; col++){
        if(board[n-1][col] == colour){
            count++;
        }
    }
    // left edge with no corner
    for(int row = 1; row < n-1; row++){
        if(board[row][0] == colour){
            count++;
        }
    }
    // right edge with no corner
    for(int row = 1; row < n-1; row++){
        if(board[row][n-1] == colour){
            count++;
        }
    }
return count;
}

int countEdgeStable(const char board[][26], int n, char colour){
    bool counted[26][26] = {{false}};
    int count = 0;
    for(int r = 0; r < n; r += n-1){
        for(int c = 0; c < n; c += n-1){
            if(board[r][c] != colour) continue;
            int dr = r == 0 ? 1 : -1, dc = c == 0 ? 1 : -1;
            for(int j=c; j>=0 && j<n && board[r][j]==colour; j+=dc) counted[r][j]=true;
            for(int i=r; i>=0 && i<n && board[i][c]==colour; i+=dr) counted[i][c]=true;
        }
    }
    for(int r=0;r<n;r++) for(int c=0;c<n;c++) count += counted[r][c];
    return count;
}

double normalisedDifference(int myValue, int oppValue){
    if(myValue + oppValue == 0){
        return 0.0;
    }
    return 100.0 * (myValue - oppValue) / (myValue + oppValue);
}

double evaluateBoard(const char board[][26], int n, char myColour){
    char opponent;
    if(myColour == 'W'){
        opponent = 'B';
    }else{
        opponent = 'W';
    }

double score= 0.0;
int mine = countPieces(board,n,myColour), theirs = countPieces(board,n,opponent);
double pieceDiff = normalisedDifference(mine, theirs);
double mobiDiff = normalisedDifference(countMobility(board,n,myColour), countMobility(board,n,opponent));
double cornerControlDiff = normalisedDifference(countCorners(board,n,myColour) , countCorners(board,n,opponent));
double dangerSquaresDiff = normalisedDifference(countDangersSquares(board,n,opponent), countDangersSquares(board,n,myColour));
double eddOccDiff = normalisedDifference(countEdgeOcc(board,n,myColour), countEdgeOcc(board, n, opponent));
double edgeStableDiff = normalisedDifference(countEdgeStable(board,n,myColour), countEdgeStable(board,n,opponent));

int occupied = mine + theirs;
int empty= (n*n) - occupied;
double wPiece, wMobility, wCorner, wDanger, wEdgestable, wEdgeOcc;

// early game
if(empty >= 44){
    wPiece = 1.25;
    wMobility = 7.5;
    wCorner = 24.0;
    wDanger = 10.5;
    wEdgestable = 8.4;
    wEdgeOcc = 3.0;
}

//midgame 
if(empty >= 20 && empty <= 43){
    wPiece = 5.0;
    wMobility = 5.5;
    wCorner = 39.0;
    wDanger = 7.0;
    wEdgestable = 15.6;
    wEdgeOcc = 5.4;
}

// late game 
if(empty <= 19){
    wPiece = 55.0;
    wMobility = 2.0;
    wCorner = 30.0;
    wDanger = 1.4;
    wEdgestable = 12.0;
    wEdgeOcc = 4.2;
}

score = (wPiece * pieceDiff) + (wMobility * mobiDiff) + (wCorner * cornerControlDiff) + (wDanger * dangerSquaresDiff) + (wEdgestable * edgeStableDiff) + (wEdgeOcc * eddOccDiff);
return score;
}




void initialiseBoard(char board[][26], int n){
    memset(board, 0, sizeof(char) * 26 * 26);
    if(n != 8) return;
    // Set the initial four discs.
    for(int i =0; i<n; i++){
        for (int j= 0; j<n; j++){

            if((i==n/2 && j== n/2) || ((i== (n/2)-1) && (j==(n/2)-1))){
                board[i][j] = 'W';
            } else if((i==n/2 && j == (n/2)-1) || ((i== (n/2)-1) && (j==n/2))){
                board[i][j] = 'B';
            }else{
                board[i][j] = 'U';
            }
        }
    }
}

void printBoard(char board[][26], int n) {
    // prints the current state of the board
    char row = 'a';
    printf("  ");
    for(int j = 0; j< n ; j++){
            printf("%c",row+j); 
        }
    printf("\n");
    for(int i= 0;i< n; i++){
        printf("%c ",row+i);
        for(int j = 0; j<n; j++){
            printf("%c",board[i][j]);
        }
        printf("\n");
    }
}

// Makes sure the position stays within the bounds of the board
bool positionInBounds(int n, int row, int col) {
    if(n != 8) return false;
    bool bound = true;
    if(row >= n || col >= n || row < 0 || col < 0 ){
        bound = false;
    }
    return bound;

}
// checks if a move is legal in one of the 8 directions depending on what deltaRow and DeltaCol are
bool checkLegalInDirection(const char board[][26], int n, int row, int col,
char colour, int deltaRow, int deltaCol) {
    if(!positionInBounds(n,row,col) || (colour!='B' && colour!='W') ||
       deltaRow < -1 || deltaRow > 1 || deltaCol < -1 || deltaCol > 1 ||
       (deltaRow==0 && deltaCol==0)) return false;
    char opposite = getOpponent(colour);
    if(colour == 'W' ){
        opposite = 'B';
    }else if(colour == 'B'){
        opposite = 'W';
    }
    int r = row + deltaRow;
    int c = col + deltaCol;

    if(positionInBounds(n,r,c) == false 
    || board[r][c] != opposite ){
        return false;
    }
    r = r + deltaRow;
    c = c + deltaCol;
    
    while(positionInBounds(n,r,c) == true ){
        if(board[r][c] == 'U'){
            return false;
        }
        if(board[r][c] == colour){
            return true;
        }
        r = r + deltaRow;
        c = c + deltaCol;

    }
return false;

}
// checks if the move is valid by checking if it is within bounds and is in a legal direction
bool moveIsValid(const char board[][26], int n, int row, int col, char colour){

    if((colour != 'B' && colour != 'W') || positionInBounds(n,row,col) != true){
        return false;
    }
    if(board[row][col] != 'U'){
        return false;
    }
    for(int deltaRow= -1; deltaRow < 2;deltaRow++){
        for(int deltaCol = -1; deltaCol < 2;deltaCol++ ){
            //skips (0,0)
            if(!(deltaRow == 0  && deltaCol == 0)){
                if(checkLegalInDirection(board,n,row,col,colour,deltaRow,deltaCol) ==true){
                    return true;
                }
            }
        }
    }
    return false;
}
// counts for the number of opponent pieces to be flipped in one direction
int countscoreForDirection(const char board[][26], int n, int row, int col,
char colour, int deltaRow, int deltaCol){
    char opposite = getOpponent(colour);
    if(colour == 'W' ){
        opposite = 'B';
    }else if(colour == 'B'){
        opposite = 'W';
    }
    int count = 0;
    int r = row + deltaRow;
    int c = col + deltaCol;

    if(positionInBounds(n,r,c) == false 
    || board[r][c] != opposite ){
        return 0;
    }
    r = r + deltaRow;
    c = c + deltaCol;
     count++;
    while(positionInBounds(n,r,c) == true ){
        if(board[r][c] == 'U'){
            return 0;
        }
        if(board[r][c] == colour){
            return count;
        }
        r = r + deltaRow;
        c = c + deltaCol;
        count++;

    }
return 0;
}
//calcute the total number of opponent pieces eaten in each of the eight directions
int moveScore(const char board[][26], int n, int row, int col,
char colour){
    int score = 0;
    if((colour != 'B' && colour != 'W') || positionInBounds(n,row,col) != true){
        return 0;
    }
    if(board[row][col] != 'U'){
        return 0;
    }
    for(int deltaRow= -1; deltaRow < 2;deltaRow++){
        for(int deltaCol = -1; deltaCol < 2;deltaCol++ ){
            //skips (0,0)
            if(!(deltaRow == 0  && deltaCol == 0)){
                score = score + countscoreForDirection(board,n,row,col,colour,deltaRow,deltaCol);
            }
        }
    }
return score;
}
// flips the tile in a specific direction
void flipInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol){
    char opposite = getOpponent(colour);
    if(colour == 'W' ){
        opposite = 'B';
    }else if(colour == 'B'){
        opposite = 'W';
    }
    for(int r = row + deltaRow, c = col + deltaCol;
         positionInBounds(n,r,c) && board[r][c] == opposite;
          r = r + deltaRow, c = c + deltaCol){
        board[r][c] = colour;
    }
}
// plays the move after checking it is valid
void playMove(char board[][26], int n, int row, int col, char colour){
    if(!moveIsValid(board,n,row,col,colour)) return;
    board[row][col] = colour;
    for(int deltaRow= -1; deltaRow < 2;deltaRow++){
        for(int deltaCol = -1; deltaCol < 2;deltaCol++ ){
            //skips (0,0)
            if(!(deltaRow == 0  && deltaCol == 0)){
                if(checkLegalInDirection(board,n,row,col,colour,deltaRow,deltaCol) ==true){
                    flipInDirection(board,n,row,col,colour,deltaRow,deltaCol);
                }
            }
        }
    }
}

// checks if there is any move available that is still valid
bool ValidMoveOnBoard(const char board[][26], int n, char turn){
    for(int i = 0; i < n;i++ ){
        for(int j = 0; j < n ; j++){
            if(moveIsValid(board,n,i,j,turn)){
                return true;
            }
        }
    }
    return false;
}

