#include "reversi.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static void fill(Board b, char value) { memset(b,value,sizeof(Board)); }
static void checkSearch(Board b, char turn, int depth) {
    Board original; memcpy(original,b,sizeof original);
    SearchResult plain=searchBest(b,turn,depth,false), fast=searchBest(b,turn,depth,true);
    assert(fabs(plain.score-fast.score)<1e-8);
    assert(plain.row==fast.row && plain.col==fast.col);
    assert(fast.nodes<=plain.nodes);
    assert(memcmp(b,original,sizeof original)==0);
}

int main(void) {
    Board b; char moves[64][2]; initialiseBoard(b,SIZE);
    assert(countPieces(b,SIZE,'B')==2 && countPieces(b,SIZE,'W')==2);
    assert(getValidMoves(b,SIZE,'B',moves)==4);
    assert(moveIsValid(b,SIZE,2,3,'B'));
    assert(!moveIsValid(b,SIZE,-1,3,'B'));
    assert(!moveIsValid(b,SIZE,3,3,'B'));
    assert(!moveIsValid(b,26,2,3,'B'));
    assert(!moveIsValid(b,SIZE,2,3,'X'));
    assert(!checkLegalInDirection(b,SIZE,2,3,'B',0,0));
    playMove(b,SIZE,2,3,'B');
    assert(countPieces(b,SIZE,'B')==4 && countPieces(b,SIZE,'W')==1);
    /* One move brackets an opponent in all eight directions. */
    fill(b,'U');
    for(int dr=-1;dr<=1;dr++) for(int dc=-1;dc<=1;dc++) if(dr || dc) {
        b[3+dr][3+dc]='W'; b[3+2*dr][3+2*dc]='B';
    }
    playMove(b,SIZE,3,3,'B');
    assert(countPieces(b,SIZE,'B')==17 && countPieces(b,SIZE,'W')==0);
    fill(b,'B');
    assert(countEdgeStable(b,SIZE,'B')==28);
    SearchResult won=searchBest(b,'B',4,true);
    assert(won.row==-1 && won.score>WIN_SCORE);
    /* Terminal game with an empty square and neither player able to move. */
    b[0][0]='U';
    assert(searchBest(b,'W',4,true).score < -WIN_SCORE);
    fill(b,'B'); b[0][0]='U'; b[0][1]='W';
    assert(getValidMoves(b,SIZE,'W',moves)==0);
    assert(getValidMoves(b,SIZE,'B',moves)==1);
    SearchResult pass=searchBest(b,'W',1,true);
    assert(pass.row==-1 && pass.col==-1 && pass.score < -WIN_SCORE);
    int r=99,c=99; assert(!makeMove(b,SIZE,'W',&r,&c) && r==-1 && c==-1);
    checkSearch(b,'W',3);
    for(int i=0;i<8;i++) for(int j=0;j<8;j++) b[i][j]=i<4?'B':'W';
    assert(searchBest(b,'B',2,true).score==0);
    /* Compare pruning with exhaustive minimax on varied reachable positions. */
    initialiseBoard(b,SIZE); char turn='B';
    for(int ply=0;ply<55;ply++) {
        int count=getValidMoves(b,SIZE,turn,moves);
        if(!count) { turn=getOpponent(turn); count=getValidMoves(b,SIZE,turn,moves); if(!count) break; }
        if(ply%5==0) checkSearch(b,turn,3);
        int index=(ply*7+3)%count;
        playMove(b,SIZE,moves[index][0],moves[index][1],turn);
        turn=getOpponent(turn);
    }
    puts("PASS: rules, eight-direction flips, bounds, stable edges, passes, terminal scores, search equivalence and immutability.");
    return 0;
}
