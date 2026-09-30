#include "reversi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint32_t randomNext(uint32_t *state) {
    *state = *state * UINT32_C(1664525) + UINT32_C(1013904223); return *state;
}
int main(void) {
    int wins=0,losses=0,draws=0,decisions=0;
    double total=0,maximum=0; uint64_t nodes=0,cutoffs=0;
    puts("pair,ai_colour,black,white,result");
    for(int pair=0;pair<10;pair++) {
        Board opening; initialiseBoard(opening,SIZE); char openingTurn='B';
        uint32_t seed=(uint32_t)pair+1;
        /* Each paired game uses the same six random opening plies. */
        for(int p=0;p<6;p++) {
            char moves[64][2]; int count=getValidMoves(opening,SIZE,openingTurn,moves);
            if(count) { unsigned int pick=randomNext(&seed)%(unsigned int)count;
                playMove(opening,SIZE,moves[pick][0],moves[pick][1],openingTurn); }
            openingTurn=getOpponent(openingTurn);
        }
        for(int side=0;side<2;side++) {
            Board b; memcpy(b,opening,sizeof b); char ai=side?'W':'B', turn=openingTurn;
            while(true) {
                char moves[64][2];
                if(!getValidMoves(b,SIZE,turn,moves)) {
                    if(!getValidMoves(b,SIZE,getOpponent(turn),moves)) break;
                    turn=getOpponent(turn); continue;
                }
                int r,c;
                if(turn==ai) {
                    clock_t start=clock(); SearchResult result=searchBest(b,turn,4,true);
                    double elapsed=1000.0*(double)(clock()-start)/CLOCKS_PER_SEC;
                    total+=elapsed; if(elapsed>maximum) maximum=elapsed;
                    nodes+=result.nodes; cutoffs+=result.cutoffs; decisions++;
                    r=result.row; c=result.col;
                } else { greedyMove(b,turn,&r,&c); }
                if(!moveIsValid(b,SIZE,r,c,turn)) { fputs("Illegal engine move\n",stderr); return EXIT_FAILURE; }
                playMove(b,SIZE,r,c,turn); turn=getOpponent(turn);
            }
            int black=countPieces(b,SIZE,'B'),white=countPieces(b,SIZE,'W');
            int diff=ai=='B'?black-white:white-black;
            if(diff>0) wins++; else if(diff<0) losses++; else draws++;
            printf("%d,%c,%d,%d,%s\n",pair+1,ai,black,white,diff>0?"win":diff<0?"loss":"draw"); fflush(stdout);
        }
    }
    printf("Summary: %d wins, %d losses, %d draws; depth 4; 20 games\n",wins,losses,draws);
    printf("AI decisions: %d; average %.3f ms; maximum %.3f ms (C clock)\n",decisions,total/decisions,maximum);
    printf("Positions searched: %llu; average %.1f/move; cutoffs: %llu\n",
           (unsigned long long)nodes,(double)nodes/decisions,(unsigned long long)cutoffs);
    return 0;
}
