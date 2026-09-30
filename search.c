#include "reversi.h"
#include <string.h>
#define INF 1e30

static int orderScore(int r, int c) {
    if((r==0 || r==7) && (c==0 || c==7)) return 100;
    if((r<=1 || r>=6) && (c<=1 || c>=6)) return -20;
    return (r==0 || r==7 || c==0 || c==7) ? 10 : 0;
}

static void orderMoves(char moves[64][2], int count) {
    /* Stable ordering preserves deterministic row-major tie breaks. */
    for(int i=1;i<count;i++) {
        char r=moves[i][0], c=moves[i][1];
        int j=i;
        while(j>0 && orderScore(moves[j-1][0],moves[j-1][1])<orderScore(r,c)) {
            moves[j][0]=moves[j-1][0]; moves[j][1]=moves[j-1][1]; --j;
        }
        moves[j][0]=r; moves[j][1]=c;
    }
}

static double minimax(const char board[][26], char turn, char root, int depth,
                      double alpha, double beta, bool pruning, SearchResult *stats) {
    stats->nodes++;
    char moves[64][2], replies[64][2];
    int count=getValidMoves(board,SIZE,turn,moves);
    if(count==0) {
        if(getValidMoves(board,SIZE,getOpponent(turn),replies)==0) {
            int diff=countPieces(board,SIZE,root)-countPieces(board,SIZE,getOpponent(root));
            return diff>0 ? WIN_SCORE+diff : diff<0 ? -WIN_SCORE+diff : 0;
        }
        /* Passing does not consume a ply: depth counts placed discs. */
        return minimax(board,getOpponent(turn),root,depth,alpha,beta,pruning,stats);
    }
    if(depth==0) return evaluateBoard(board,SIZE,root);
    orderMoves(moves,count);
    bool maximizing=turn==root;
    double best=maximizing ? -INF : INF;
    for(int i=0;i<count;i++) {
        Board next; memcpy(next,board,sizeof next);
        playMove(next,SIZE,moves[i][0],moves[i][1],turn);
        double value=minimax(next,getOpponent(turn),root,depth-1,alpha,beta,pruning,stats);
        if(maximizing) { if(value>best) best=value; if(best>alpha) alpha=best; }
        else { if(value<best) best=value; if(best<beta) beta=best; }
        if(pruning && alpha>=beta) { stats->cutoffs++; break; }
    }
    return best;
}

SearchResult searchBest(const char board[][26], char turn, int depth, bool pruning) {
    SearchResult result={-1,-1,0,0,0};
    if((turn!='B' && turn!='W') || depth<1 || depth>6) return result;
    char moves[64][2];
    int count=getValidMoves(board,SIZE,turn,moves);
    if(!count) {
        result.score=minimax(board,turn,turn,depth,-INF,INF,pruning,&result);
        return result;
    }
    orderMoves(moves,count);
    result.score=-INF;
    double alpha=-INF;
    for(int i=0;i<count;i++) {
        Board next; memcpy(next,board,sizeof next);
        playMove(next,SIZE,moves[i][0],moves[i][1],turn);
        double value=minimax(next,getOpponent(turn),turn,depth-1,alpha,INF,pruning,&result);
        if(value>result.score) { result.score=value; result.row=moves[i][0]; result.col=moves[i][1]; }
        if(pruning && result.score>alpha) alpha=result.score;
    }
    return result;
}

bool greedyMove(const char board[][26], char turn, int *row, int *col) {
    *row=*col=-1;
    int best=0;
    for(int r=0;r<SIZE;r++) for(int c=0;c<SIZE;c++) {
        int score=moveScore(board,SIZE,r,c,turn);
        if(score>best) { best=score; *row=r; *col=c; }
    }
    return best>0;
}

int makeMove(const char board[][26], int n, char turn, int *row, int *col) {
    if(row) *row=-1;
    if(col) *col=-1;
    if(!row || !col || n!=SIZE) return 0;
    SearchResult result=searchBest(board,turn,4,true);
    *row=result.row; *col=result.col;
    return result.row>=0;
}
