#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Chess.h"
#include "ChessAI.h"

#define INF 2000000000



//Plays a turn using the AI that looks ahead in the game
void playAITurnHard(){
  int fromx;
  int fromy;
  int tox;
  int toy;
  long al = -INF;
  long be = INF;
  long x  = minMax(&b,0,2,1,1,&al,&be,&fromx,&fromy,&tox,&toy);
  playTurn(fromx,fromy,tox,toy);
}



//Plays a turn using the AI that does not look ahead in the game
void playAITurnEasy(){
  moveList valid;
  grid board;
  long bestScore = -INF;
  long score;
  int bestx1, besty1, bestx2, besty2;
  for (int i = 0 ; i < 8; i++){
    for (int j = 0; j < 8; j++){
      if (b.plane[i][j] / 10 == BLACK){
        getMovesPiece(&b, &valid, i, j);
        for (int k = 0; k < valid.len; k++){
          copyBoard(&b, &board);
          getMovedBoard(&board, i, j, valid.list[k][0], valid.list[k][1]);
          score = getScore(&board);
          if(score > bestScore){
            bestScore = score;
            bestx1 = i;
            besty1 = j;
            bestx2 = valid.list[k][0];
            besty2 = valid.list[k][1];
          }
        }
      }
    }
  }
  playTurn(bestx1, besty1, bestx2, besty2);
}



/* Takes a board,
 * Returns the score of the board, based on
 *  if the human player (WHITE) is in check or checkmate,
 *  or a combination of good ratings (plusMinus, canProtectWeighted, canHitSquares, canAttack, pawnRating)
 *   and bad ratings (piecesLeft, spaceAroundKing, risk)
 */
long getScore(grid* board){
  //if the human player (WHITE) is in check or checkmate, return highest score
  if (inCheckmate(board, WHITE) && inCheck(board, WHITE)){
    return INF;
  }
  if (inCheckmate(board, BLACK) && inCheck(board, BLACK)){
    return -INF;
  }
  //evaluate the good and bad ratings
  long good = 40*plusMinus(board) + 1*canProtectWeighted(board) + 1*canHitSquares(board)
                      + 10*canAttack(board) + 4*(32-piecesLeft(board))*pawnRating(board);
  long bad = (3 * (32 - piecesLeft(board)) * spaceAroundKing(board)) + (120 * risk(board));
  return good - bad;
}



/* Takes a board,
 * Returns the number of squares that the AI (BLACK) can hit
 */
long canHitSquares(grid* board){
  //sum keeps track of the number of squares that the AI (BLACK) can hit
  long sum = 0;
  //runs through board, runs canHit on each piece, if an AI piece can hit that square, add to sum
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      if (canHit(board, BLACK, i, j)){
        sum++;
      }
    }
  }
  //returns the number of squares that the AI (BLACK) can hit
  return sum;
}



/* Takes a board,
 * Returns the number of human player pieces (WHITE) that can be hit by the AI (BLACK), weighted by the value of the pieces
 */
long canAttack(grid* board){
  long sum = 0;
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      if ((*board).plane[i][j]/10 == WHITE && canHit(board, BLACK, i, j) && (*board).plane[i][j]%10 != KING){
        sum += chessValues[(*board).plane[i][j]%10] * chessValues[(*board).plane[i][j]%10];
      }
      else if ((*board).plane[i][j]/10 == WHITE && canHit(board, BLACK, i, j)){
        sum += chessValues[(*board).plane[i][j]%10];
      }
    }
  }
  return sum;
}



/* Takes a board,
 * Returns a measure of the AI (BLACK) pawn nearest to the human player (WHITE) side of the board
 */
long pawnRating(grid* board){
  //best keeps track of which pawn is nearest to the human player (WHITE) side of the board
  int best = 0;
  //runs through board, finds the AI (BLACK) pawn closest to the human player (WHITE) side of the board
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      if ((*board).plane[i][j]%10 == PAWN && (*board).plane[i][j]%10 == BLACK){
        if (j > best){
          best = j;
        }
      }
    }
  }
  return best*best;
}



/* Takes a board,
 * Returns a measure of the pieces on the board based on the value of the piece and the colour
 */
long plusMinus(grid* board){
  long tot = 0;
  int piece;
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      piece = (*board).plane[i][j];
      tot += (-1 + 2*(piece/10)) * (chessValues[piece%10]);
    }
  }
  return tot*tot*tot;
}



/* Takes a board,
 * Returns a measure of how well the AI (BLACK) can protect its own pieces based on 
 * the value of the piece and canProtect
 */
long canProtectWeighted(grid* board){
  long tot = 0;
  //runs through board, if its a black piece and not the king,
  //  adds combination of the value of the piece and canProtect to tot
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      if ((*board).plane[i][j]/10 == BLACK && (*board).plane[i][j]%10 != KING){
        tot += chessValues[(*board).plane[i][j]%10] * canProtect(board,i,j);
      }
    }
  }
  return tot;
}



/* Takes a board,
 * Returns a measure of the risk of AI (BLACK) pieces, except for king, based on
 * the value of the piece and the the value of riskPiece, which rates how risky the move is
 */
long risk(grid* board){
  long totRisk = 0;
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      if ((*board).plane[i][j]/10 == BLACK && (*board).plane[i][j]%10 != KING){
        totRisk += chessValues[(*board).plane[i][j]%10] * chessValues[(*board).plane[i][j]%10] * riskPiece(board, i, j); 
      }
    }
  }
  return totRisk;
}



/* Takes a board and a position (x,y),
 * Returns a measure of the risk factor of the piece at (x,y) is based on 
 * the value of the AI piece at (x,y), the value of the least important Human player piece that can hit (x,y),
 * and whether or not the AI can protect its piece at (x,y)
 */
long riskPiece(grid* board, int x, int y){
  int lowestHitter = canHitLowest(board, WHITE, x, y);
  if (lowestHitter == 0){
    return 0;
  }
  int multi = 1;
  if (!canProtect(board,x,y)){
    multi = 0;
  }
  return chessValues[(*board).plane[x][y] % 10] - (multi*2*lowestHitter)/3;
}



/* Takes a board and a position (x,y),
 * Returns true when the AI (BLACK) can hit the position (x,y)
 */
int canProtect(grid* board, int x, int y){
  /* create temporary board and changes board(x,y) to 0
   * because canHit does not allow one piece to hit another of the same colour
   * (ie will return false every time because board(x,y) is a black piece)
   */
  int tmp = (*board).plane[x][y];
  (*board).plane[x][y] = 0;
  
  int toReturn = canHit(board, BLACK, x, y);
  
  //reset board to original state before returning
  (*board).plane[x][y] = tmp;
  return toReturn;
}



/* Takes a board, a position (x,y) and a list of which squares it has 'seen' (flags),
 * Returns the number of open squares above, below, and to the side of (x,y)
 * for which none of the opponents pieces can hit
 */
int spacePlus(grid* board, int x, int y, grid* flags){
  (*flags).plane[x][y] = 1;
  if (canHit(board, BLACK, x, y)){
    return 0;
  }
  else if ((*board).plane[x][y]/10 == WHITE){
    return 1;
  }
  else if ((*board).plane[x][y]/10 == BLACK){
    return 0;
  }
  int sum = 1;
  
  /* runs though squares surrounding (x,y) and calls sumPlus on that square 
   *  if the square on the board and it's flag is 0 (hasn't been 'looked' at yet)
   */
  for (int i = -1; i < 2; i++){
    for (int j = -1; j < 2; j++){
      if (-1 < x+i && 8 > x+i && -1 < y+j && 8 > y+j ){
        if ((*flags).plane[x+i][y+j] == 0 && abs(i) != abs(j)){
          sum += spacePlus(board, x+i, y+j, flags);
        }
      }
    }
  }
  return sum;
}



/* Takes a board, a position (x,y) and a list of which squares it has 'seen' (flags),
 * Returns the number of open squares immediately surrounding (x,y) for which none of the opponents pieces can hit
 */
int spaceAll(grid* board, int x, int y, grid* flags){
  (*flags).plane[x][y] = 1;
  if (canHit(board, BLACK, x, y)){
    return 0;
  }
  else if ((*board).plane[x][y]/10 == WHITE){
    return 1;
  }
  else if ((*board).plane[x][y]/10 == BLACK){
    return 0;
  }
  int sum = 1;

  /* runs though squares surrounding (x,y) and calls sumAll on that square 
   *  if the square on the board and it's flag is 0 (hasn't been 'looked' at yet)
   */
  for (int i = -1; i < 2; i++){
    for (int j = -1; j < 2; j++){
      if (-1 < x+i && 8 > x+i && -1 < y+j && 8 > y+j){
        if ((*flags).plane[x+i][y+j] == 0){
          sum += spaceAll(board, x+i, y+j, flags);
        }
      }
    }
  }
  return sum;
}



/* Takes a board,
 * Returns number of 'free' spaces around king (where the king can move)
 */
long spaceAroundKing(grid* board){
  //find white king - kings position is (kingx,kingy)
  int kingx,kingy;
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      if ((*board).plane[i][j] == KING){
        kingx = i;
        kingy = j;
      }
    }
  }

  //flags keeps track of what squares the function has 'looked' at - initially all 0
  grid flags;
  
  //clears flags
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      flags.plane[i][j] = 0;
    }
  }
  int sumPlus = spacePlus(board, kingx, kingy, &flags);
  
  //clears flags
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      flags.plane[i][j] = 0;
    }
  }
  int sumAll = spaceAll(board, kingx, kingy, &flags);

  //returns values sumPlus and sumAll weighted appropriatly
  return sumPlus + 4*sumAll;
}



/* minMax function: takes the inputs and mutates 4 integers to the optimal move, also returns the game theoried "score" of the board
 * board: the current state of the board
 * depth: current depth of the tree
 * end: final depth of the tree, how many moves to look ahead
 * isMax: is the node a minimizer or maximizer 
 * isBlack: looking at black moves or white moves
 * alpha: alpha value, best known value to the maximizer along path to root, inititalize with -INF
 * beta: beta value, best known value to the minimizer along path to root, inititalize with INF
 * currentX, currentY: starting position of the piece to be moved
 * moveToX, moveToY: final position of the piece to be moved
 */
long minMax(grid *board, int depth, int end, int isMax, int isBlack, long *alpha, long *beta, int *currentX, int *currentY, int *moveToX, int *moveToY){

  //copy over the original board, we're going to mutate the actual board
  grid test;
  copyBoard(board, &test);
  moveList allMoves;
  int length;
  long best = INF;
  long temp;

  //if we've reached the number of moves to predict forward, score the board and return it
  //or, if we've reached the end of our tree, return the value scored there
  if (depth == end) {
    return getScore(board);
  }

  //initializing the value of the node depending on whether its a minimizer node or a maximizer node
  if(isMax){
    best*=-1;
  }
  
  /*iterates through all positions on the board, once a piece (black or white depending on the turn) is found,
    get an movelist of all valid moves for said piece*/
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if((*board).plane[i][j]/10 == isBlack) {
        getMovesPiece(board, &allMoves, i, j);
        length = allMoves.len;

        //if the node is a maximizer node, iterate through all valid moves, or nodes of the tree, and return their values
        if(isMax){
          for (int k = 0; k < length; k++) {
              getMovedBoard(board, i, j, allMoves.list[k][0], allMoves.list[k][1]);
              temp = minMax(board, depth + 1, end, 0, 0, alpha, beta, currentX,currentY, moveToX,moveToY);

              //resets the board back to original for next moves
              copyBoard(&test,board);

              //at root, find the path of the optimal move for piece, mutates the integers to x and y coords before and after the move, respectively
              //root is always a maximizer
              if(depth == 0 && temp >best){
                *currentX = i;
                *currentY = j;
                *moveToX = allMoves.list[k][0];
                *moveToY = allMoves.list[k][1];
                
              }

              //if the current subnode returns a better value than our previous subnodes, replace the value of our node with it, larger cuz maximizer
              if (temp > best){
                best = temp;
              }
              
              //logic here is same, if current subnode returns a better value previous subnodes, replace alpha value with it
              if (best > *alpha) {
                *alpha = best;
              }

              //if beta is less than alpha, we get to prune the rest of the branches on this node as the minimizer above this node will not continue looking
              if (*beta <= *alpha) {
                break;
              }
          }
        }

        //if the node is a minimizer node, iterate through all valid moves, or nodes of the tree, and return their values
        else{
          for (int k = 0; k < length; k++) {
              getMovedBoard(board, i, j, allMoves.list[k][0], allMoves.list[k][1]);
              temp = minMax(board, depth + 1, end, 1, 1, alpha, beta, currentX,currentY, moveToX,moveToY);

              //resets the board back to original for next moves
              copyBoard(&test,board);

              //if the current subnode returns a better value than our previous subnodes, replace the value of our node with it, smaller cuz minimizer
              if (temp < best) {
                best = temp;
              }
              
              //same as before, if current subnode returns a better value previous subnodes, replace beta value with it
              if (best < *beta) {
                *beta = best;
              }

              //if beta is less than alpha, we get to prune the rest of the branches on this node as the maximizer above this node will not continue looking
              if (*beta <= *alpha) {
                break;
              }
          }
        }
      }
    }
  }

  //return the optimal value of this node
  return best;
}

