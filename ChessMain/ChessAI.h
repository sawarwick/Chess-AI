#ifndef CHESSAI_H
#define CHESSAI_H

//Core AI functions
void playAITurnEasy();
void playAITurnHard();
long getScore(grid* board);
long minMax(grid *board, int depth, int end, int isMax, int isBlack, long *alpha, long *beta, int *currentx, int *currenty, int *moveTox, int *moveToy);

//Functions used to caluculate the positive portion of score
long canHitSquares(grid* board);
long plusMinus(grid* board);
long canProtectWeighted(grid* board);
int canProtect(grid* board, int x, int y);
long canAttack(grid* board);
long pawnRating(grid* board);

//Functions used to caluculate the negative portion of score
int spacePlus(grid* board, int x, int y, grid* flags);
long risk(grid* board);
long riskPiece(grid* board, int x, int y);
int spaceAll(grid* board, int x, int y, grid* flags);
long spaceAroundKing(grid* board);

#endif
