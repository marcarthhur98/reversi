#include "reversi.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static bool readLine(char *line, int size) {
    if(!fgets(line,size,stdin)) return false;
    if(!strchr(line,'\n') && !feof(stdin)) {
        int ch; while((ch=getchar())!='\n' && ch!=EOF) {}
        line[0]='\0';
    }
    return true;
}

int main(void) {
    Board board; initialiseBoard(board,SIZE);
    char line[128], human=0, turn='B', a,b,extra;
    puts("Reversi | 8x8 | alpha-beta opponent (4 plies)");
    while(!human) {
        printf("Play B (first) or W? "); fflush(stdout);
        if(!readLine(line,sizeof line)) return 0;
        if(sscanf(line," %c %c",&a,&extra)==1) {
            a=(char)toupper((unsigned char)a);
            if(a=='B' || a=='W') human=a;
        }
        if(!human) puts("Please enter B or W.");
    }
    while(true) {
        printBoard(board,SIZE);
        char moves[64][2];
        if(!getValidMoves(board,SIZE,turn,moves)) {
            if(!getValidMoves(board,SIZE,getOpponent(turn),moves)) break;
            printf("%c has no legal move and passes.\n",turn);
            turn=getOpponent(turn); continue;
        }
        int r=-1,c=-1;
        if(turn==human) {
            while(r<0) {
                printf("Your move (row then column, e.g. cd; q to quit): "); fflush(stdout);
                if(!readLine(line,sizeof line)) { puts("\nGoodbye."); return 0; }
                int tokens=sscanf(line," %c %c %c",&a,&b,&extra);
                if(tokens==1 && (a=='q' || a=='Q')) return 0;
                if(tokens==2) {
                    int rr=tolower((unsigned char)a)-'a', cc=tolower((unsigned char)b)-'a';
                    if(moveIsValid(board,SIZE,rr,cc,turn)) { r=rr; c=cc; }
                }
                if(r<0) puts("Invalid move. Choose a legal empty square from a to h.");
            }
        } else {
            SearchResult result=searchBest(board,turn,4,true);
            r=result.row; c=result.col;
            printf("Computer plays %c%c (%llu positions searched).\n",'a'+r,'a'+c,
                   (unsigned long long)result.nodes);
        }
        playMove(board,SIZE,r,c,turn); turn=getOpponent(turn);
    }
    int black=countPieces(board,SIZE,'B'), white=countPieces(board,SIZE,'W');
    printf("Final score: B %d - W %d\n",black,white);
    puts(black==white ? "Draw!" : black>white ? "Black wins!" : "White wins!");
    return 0;
}
