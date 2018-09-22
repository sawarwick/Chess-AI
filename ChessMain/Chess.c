#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Chess.h"



//Opening setup of chess file
void set(){
  chessReset();
  chessValues[0] = 0;
  chessValues[PAWN] = 1;
  chessValues[KNIGHT] = 3;
  chessValues[BISHOP] = 3;
  chessValues[ROOK] = 5;
  chessValues[QUEEN] = 9;
  chessValues[KING] = 100;
}



//Resets all important global variables for a game to be played
void chessReset(){
  int tmp [8][8] = {
  {12,11,0,0,0,0,1,2},
  {13,11,0,0,0,0,1,3},
  {14,11,0,0,0,0,1,4},
  {15,11,0,0,0,0,1,5},
  {16,11,0,0,0,0,1,6},
  {14,11,0,0,0,0,1,4},
  {13,11,0,0,0,0,1,3},
  {12,11,0,0,0,0,1,2}};
  for(int i = 0; i<8; i++){
    for(int j = 0; j<8; j++){
      b.plane[i][j] = tmp[i][j];
    }
  }
  turn = 0;
  enPas[0] = -1;
  enPas[1] = -1;
  check = 0;
  checkmate = 0;
  stalemate = 0;
  drawCount = 0;
  moveCount = 0;
  lastx = -1;
  lasty = -1;
  for(int i = 0; i<2; i++){
    for(int j = 0; j<3; j++){
      castleBool[i][j] = 1;
    }
  }
}



/* Plays through one turn in the chess game
 * Moves the piece at x1,y1 to x2,y1
 * Will still run if not given valid move
 */
void playTurn(int x1, int y1, int x2, int y2){
  lastx = x2;
  lasty = y2;
  if(checkmate||stalemate){
    return;
  }
  //Updates stalemate related globals
  drawCount++;
  moveCount++;
  int pieceCount = piecesLeft(&b);
  if(b.plane[x1][y1]%10 == PAWN){
    drawCount = 0;
  }
  //Updates game board
  getMovedBoard(&b,x1,y1,x2,y2);
  if(pieceCount != piecesLeft(&b)){
    drawCount = 0;
  }
  //Sets a new enPas value if there is one, otherwise sets it to an invalid value
  if(b.plane[x2][y2]%10 == PAWN && abs(y1-y2) == 2){
    enPas[0] = x2;
    enPas[1] = y2;
  }
  else{
    enPas[0] = -1;
    enPas[1] = -1;
  }
  changeCastleBool();
  //Makes turn cycle between 0 and 1
  turn = (turn+1)%2;
  //checks status of significant game events
  if(inStalemate(&b)){
    stalemate = 1;
  }
  if(inCheck(&b,turn)){
    check = 1;
    if(inCheckmate(&b,turn)){
      checkmate = 1;
    }
  }
  else{
    check = 0;
  }
}



/* Modifies the board at the given address to the state the board would be in if the given move was performed.
 * Moves the piece at position x1,y1 to the position x2, y2
 * Make the appropriate changes for special moves, such as castling or en passant
 * Will still run if given move is not valid.
 */
void getMovedBoard(grid *board, int x1, int y1, int x2, int y2){
  //moves piece
  int oldPiece = (*board).plane[x2][y2];
  (*board).plane[x2][y2] = (*board).plane[x1][y1];
  (*board).plane[x1][y1] = 0;

  //upgrades pawns that make it to the end of the board
  if((*board).plane[x2][y2]%10 == PAWN && (y2 == 7)){
    (*board).plane[x2][y2] = 10+QUEEN;//Black queen
  }
  if((*board).plane[x2][y2]%10 == PAWN && (y2 == 0)){
    (*board).plane[x2][y2] = QUEEN;//White queen
  }

  //En Passant
  if((*board).plane[x2][y2]%10 == PAWN){
    if(abs(x2-x1)==1 && oldPiece == 0){
      (*board).plane[x2][y1] = 0;
    }
  }

  //Castling
  if((*board).plane[x2][y2]%10 == KING){
    int r = ((*board).plane[x2][y2]/10)*10+ROOK;
    if(x2-x1 == 2){//Bottom corner rooks ([7,0] and [7,7])
      (*board).plane[7][y1] = 0;
      (*board).plane[5][y1] = r;
    }
    if(x2-x1 == -2){//Top corner rooks ([0,0] and [0,7])
      (*board).plane[0][y1] = 0;
      (*board).plane[3][y1] = r;
    }
  }
}



//No flags given, calls getMovesPiece2 with additional information
void getMovesPiece(grid* board, moveList* toChange, int x, int y){
  getMovesPiece2(board, toChange, x, y, 1, 1);
}



//castlingFlag given, calls getMovesPiece2 with additional information
void getMovesPiece1(grid* board, moveList* toChange, int x, int y, int castlingFlag){
  getMovesPiece2(board,toChange, x, y, castlingFlag, 1);
}



/* 2 Flags given
 * Given the board and an x,y position, gives all valid moves for the piece on that square
 * Flags are there to prevent infinite recursive loops
 * castlingFlag exists to prevent looping when checking if castling is valid
 * checkFlag exists to prevent looping when checking if the king is in check
 */
void getMovesPiece2(grid* board,moveList* toReturn, int x, int y, int castlingFlag, int checkFlag){
  int piece = (*board).plane[x][y];
  (*toReturn).len = 0;
  int curx, cury; // x,y values to loop with

  //Queen and Rook moves
  if(piece%10 == QUEEN || piece%10 == ROOK){
    int lines[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for(int i = 0; i<4; i++){
      curx = x;
      cury = y;
      //Runs through lines until it is off the board, or cannot continue otherwise
      while(1){
        if(curx+lines[i][0] < 0 || curx+lines[i][0]>7 || cury+lines[i][1]<0 || cury+lines[i][1]>7){
          break;
        }
        curx+=lines[i][0];
        cury+=lines[i][1];
        if((*board).plane[curx][cury] == 0){
          (*toReturn).list[(*toReturn).len][0] = curx;
          (*toReturn).list[(*toReturn).len][1] = cury;
          (*toReturn).len++;
        }
        else if(piece/10!=(*board).plane[curx][cury]/10){
          (*toReturn).list[(*toReturn).len][0] = curx;
          (*toReturn).list[(*toReturn).len][1] = cury;
          (*toReturn).len++;
          break;
        }
        else{
          break;
        }
      }
    }
  }

  //Queen and Bishop moves
  if(piece%10 == QUEEN || piece%10 == BISHOP){
    int diagonals[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for(int i = 0; i<4; i++){
      curx = x;
      cury = y;
      //Runs through diagonals until piece is off the board, or cannot continue otherwise
      while(1){
        if(curx+diagonals[i][0] < 0 || curx+diagonals[i][0]>7 || cury+diagonals[i][1]<0 || cury+diagonals[i][1]>7){
          break;
        }
        curx+=diagonals[i][0];
        cury+=diagonals[i][1];
        if((*board).plane[curx][cury] == 0){
          (*toReturn).list[(*toReturn).len][0] = curx;
          (*toReturn).list[(*toReturn).len][1] = cury;
          (*toReturn).len++;
        }
        else if(piece/10!=(*board).plane[curx][cury]/10){
          (*toReturn).list[(*toReturn).len][0] = curx;
          (*toReturn).list[(*toReturn).len][1] = cury;
          (*toReturn).len++;
          break;
        }
        else{
          break;
        }
      }
    }
  }

  //Pawn moves
  if(piece%10 == PAWN){
    int direct;
    //Sends pawn in correct direction
    if(piece/10 == 0){
      direct = -1;
    }
    else{
      direct = 1;
    }
    //One space forwards
    if((*board).plane[x][y+direct] == 0){
      (*toReturn).list[(*toReturn).len][0] = x;
      (*toReturn).list[(*toReturn).len][1] = y+direct;
      (*toReturn).len++;
      //2 spaces on first move
      if(y == 6 && piece/10 == WHITE && (*board).plane[x][y+2*direct] == 0){
        (*toReturn).list[(*toReturn).len][0] = x;
        (*toReturn).list[(*toReturn).len][1] = y+2*direct;
        (*toReturn).len++;  
      }
      if(y == 1 && piece/10 == BLACK && (*board).plane[x][y+2*direct] == 0){
        (*toReturn).list[(*toReturn).len][0] = x;
        (*toReturn).list[(*toReturn).len][1] = y+2*direct;
        (*toReturn).len++;  
      }
    }
    //Taking pieces
    for(int i = -1; i<2; i+=2){//Goes through -1, 1
      if(x+i>-1 && x+i <8 && piece/10!=(*board).plane[x+i][y+direct]/10 && 0!=(*board).plane[x+i][y+direct]){
        (*toReturn).list[(*toReturn).len][0] = x+i;
        (*toReturn).list[(*toReturn).len][1] = y+direct;
        (*toReturn).len++;  
      }
    }
    //En Passant
    if(enPas[0] > -1){
      if(abs(enPas[0]-x) == 1 && enPas[1] == y){ 
        (*toReturn).list[(*toReturn).len][0] = enPas[0];
        (*toReturn).list[(*toReturn).len][1] = y+direct;
        (*toReturn).len++;  
      }
    }
  }
  
  //Knight Move
  if(piece%10 == KNIGHT){
    int directions[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    int turns[2][2] = {{2,1},{1,2}};
    for(int i = 0; i<4; i++){
      for(int j = 0; j<2; j++){
        curx = x+directions[i][0]*turns[j][0];
        cury = y+directions[i][1]*turns[j][1];
        //checks that point is on board, space is either empty or has opposing piece on it
        if(curx<8 && curx>-1 && cury<8 && cury>-1 && ((*board).plane[curx][cury]==0 || (*board).plane[curx][cury]/10 != piece/10)){
          (*toReturn).list[(*toReturn).len][0] = curx;
          (*toReturn).list[(*toReturn).len][1] = cury;
          (*toReturn).len++;    
        }
      }
    }
  }
  
  //King Move
  if(piece%10==KING){
    for(int i = -1; i<2; i++){
      for(int j = -1; j<2; j++){
        if((j!=0 || i!=0) && x+i>-1 && x+i<8 && y+j >-1 && y+j < 8 && ((*board).plane[x+i][y+j]==0 || (*board).plane[x+i][y+j]/10 != piece/10)){
          (*toReturn).list[(*toReturn).len][0] = x+i;
          (*toReturn).list[(*toReturn).len][1] = y+j;
          (*toReturn).len++;    
        }
      }
    }
    //Castling
    if(castlingFlag){
      if(piece/10 == WHITE && canCastle(board, WHITE, QUEEN)){
        (*toReturn).list[(*toReturn).len][0] = 2;
        (*toReturn).list[(*toReturn).len][1] = 7;
        (*toReturn).len++;
      }
      if(piece/10 == WHITE && canCastle(board, WHITE, KING)){
        (*toReturn).list[(*toReturn).len][0] = 6;
        (*toReturn).list[(*toReturn).len][1] = 7;
        (*toReturn).len++;
      }
      if(piece/10 == BLACK && canCastle(board, BLACK, QUEEN)){
        (*toReturn).list[(*toReturn).len][0] = 2;
        (*toReturn).list[(*toReturn).len][1] = 0;
        (*toReturn).len++;
      }
      if(piece/10 == BLACK && canCastle(board, BLACK, KING)){
        (*toReturn).list[(*toReturn).len][0] = 6;
        (*toReturn).list[(*toReturn).len][1] = 0;
        (*toReturn).len++;
      }
    }
  }

  //If running through while checking check, dont worry about checking check again
  //(Stops infinite loops)
  if(!checkFlag){
    return;
  }

  //Only keeps positions in the list if it is valid (not in check)
  int tempLen = 0;
  grid tempboard;
  
  for(int i = 0; i<(*toReturn).len; i++){
    copyBoard(board,&tempboard);
    getMovedBoard(&tempboard,x,y,(*toReturn).list[i][0],(*toReturn).list[i][1]);
    if(!inCheck(&tempboard,piece/10)){
      (*toReturn).list[tempLen][0] = (*toReturn).list[i][0];
      (*toReturn).list[tempLen][1] = (*toReturn).list[i][1];
      tempLen++; 
    }
  }
  (*toReturn).len = tempLen;
}



//Returns a boolean desribing whether or not a piece of colour can attack/land on the position given by x,y on the given board
int canHit(grid *board, int col, int x, int y){
  int curx,cury;

  //Looks for Rooks or Queens along lines that can hit the given position
  int lines[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
  for(int i = 0; i<4; i++){
    for(int j = 1; j<9; j++){
      curx = x+j*lines[i][0];
      cury = y+j*lines[i][1];
      if(-1<curx && 8>curx && -1<cury && 8>cury){
        if(((*board).plane[curx][cury]%10 == ROOK || (*board).plane[curx][cury]%10 == QUEEN) && (*board).plane[curx][cury]/10 == col) {
          return 1;
        }
        else if((*board).plane[curx][cury] != 0){
          break;
        }
      }
    }
  }
  
  //Looks for Bishops or Queens along diagonals that can hit the given position
  int diagonals[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
  for(int i = 0; i<4; i++){
    for(int j = 1; j<8; j++){
      curx = x+j*diagonals[i][0];
      cury = y+j*diagonals[i][1];
      if(-1<curx && 8>curx && -1<cury && 8>cury){
        if(((*board).plane[curx][cury]%10 == BISHOP || (*board).plane[curx][cury]%10 == QUEEN) && (*board).plane[curx][cury]/10 == col) {
          return 1;
        }
        else if((*board).plane[curx][cury] != 0){
          break;
        }
      }
     
    }
  }

  //Looks for Knights in a valid position to hit the given position
  int directions[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
  int turns[2][2] = {{2,1},{1,2}};
  for(int i = 0; i<4; i++){
    for(int j = 0; j<2; j++){
      curx = x+directions[i][0]*turns[j][0];
      cury = y+directions[i][1]*turns[j][1];
      if(-1<curx && 8>curx && -1<cury && 8>cury){
        if((*board).plane[curx][cury]%10 == KNIGHT && (*board).plane[curx][cury]/10 == col) {
          return 1;
        }
      }
    }
  }

  //Looks one piece away in every direction for Kings that can hit the given position
  for(int i = -1; i<2; i++){
    for(int j = -1; j<2; j++){
      curx = x+i;
      cury = y+j;
      if((i!=0 || j!= 0) && -1<curx && 8>curx && -1<cury && 8>cury){
        if((*board).plane[curx][cury]%10 == KING && (*board).plane[curx][cury]/10 == col) {
          return 1;
        }
      }
    }
  }

  //Looks for Pawns that can hit the given position
  int dir;
  if(col == WHITE){
    dir = -1;
  }
  else{
    dir = 1;
  }
  if(-1<y-dir && 8>y-dir){
    if(((*board).plane[x+1][y-dir]%10 == PAWN && (*board).plane[x+1][y-dir]/10 == col)||((*board).plane[x-1][y-dir]%10 == PAWN && (*board).plane[x-1][y-dir]/10 == col)) {
      return 1;
    }
  }
  return 0;
}



/*Returns the smallest point value of any pieces that of colour col that can land on the square at x,y
 *If no pieces can hit there, it returns 0
 */
int canHitLowest(grid *board, int col, int x, int y){  //returns smallest chessValue of piece of col that canHit (x,y)
  int curx,cury;
  int lowest = 1000;

  //Looks for Rooks or Queens along lines that can hit the given position
  int lines[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
  for(int i = 0; i<4; i++){
    for(int j = 1; j<8; j++){
      curx = x+j*lines[i][0];
      cury = y+j*lines[i][1];
      if(-1<curx && 8>curx && -1<cury && 8>cury){
        if(((*board).plane[curx][cury]%10 == ROOK || (*board).plane[curx][cury]%10 == QUEEN) && (*board).plane[curx][cury]/10 == col) {
          if(chessValues[(*board).plane[curx][cury]%10]<lowest){
            lowest = chessValues[(*board).plane[curx][cury]%10];
            break;
          }
        }
        else if((*board).plane[curx][cury] != 0){
          break;
        }
      }
    }
  }
  
  //Looks for Bishops or Queens along diagonals that can hit the given position
  int diagonals[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
  for(int i = 0; i<4; i++){
    for(int j = 1; j<8; j++){
      curx = x+j*diagonals[i][0];
      cury = y+j*diagonals[i][1];
      if(-1<curx && 8>curx && -1<cury && 8>cury){
        if(((*board).plane[curx][cury]%10 == BISHOP || (*board).plane[curx][cury]%10 == QUEEN) && (*board).plane[curx][cury]/10 == col) {
          if(chessValues[(*board).plane[curx][cury]%10]<lowest){
            lowest = chessValues[(*board).plane[curx][cury]%10];
            break;
          }
        }
        else if((*board).plane[curx][cury] != 0){
          break;
        }
      }
     
    }
  }
  
  //Looks for Knights in a valid position to hit the given position
  int directions[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
  int turns[2][2] = {{2,1},{1,2}};
  for(int i = 0; i<4; i++){
    for(int j = 0; j<2; j++){
      curx = x+directions[i][0]*turns[j][0];
      cury = y+directions[i][1]*turns[j][1];
      if(-1<curx && 8>curx && -1<cury && 8>cury){
        if((*board).plane[curx][cury]%10 == KNIGHT && (*board).plane[curx][cury]/10 == col) {
          if(chessValues[(*board).plane[curx][cury]%10]<lowest){
            lowest = chessValues[(*board).plane[curx][cury]%10];
          }
        }
      }
    }
  }

  //Looks one piece away in every direction for Kings that can hit the given position
  for(int i = -1; i<2; i++){
    for(int j = -1; j<2; j++){
      curx = x+i;
      cury = y+j;
      if((i!=0 || j!= 0) && -1<curx && 8>curx && -1<cury && 8>cury){
        if((*board).plane[curx][cury]%10 == KING && (*board).plane[curx][cury]/10 == col) {
          if(chessValues[(*board).plane[curx][cury]%10]<lowest){
            lowest = chessValues[(*board).plane[curx][cury]%10];
          }
        }
      }
    }
  }

  //Looks for Pawns that can hit the given position
  int dir;
  if(col == WHITE){
    dir = -1;
  }
  else{
    dir = 1;
  }
  if(-1<y-dir && 8>y-dir){
    if(((*board).plane[x+1][y-dir]%10 == PAWN && (*board).plane[x+1][y-dir]/10 == col)||((*board).plane[x-1][y-dir]%10 == PAWN && (*board).plane[x-1][y-dir]/10 == col)) {
      return 1;
    }
  }
  if(lowest != 1000){
    return lowest;
  }
  return 0;
}



//Returns if col is in check in the given board state
int inCheck(grid *board, int col){
  int x,y;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if((*board).plane[i][j]%10 == KING && (*board).plane[i][j]/10 == col){
        x = i;
        y = j;
      }
    }
  }
  return canHit(board, (col+1)%2, x, y);  
}



/* Returns if col has no valid moves in the given board state
 * Must be used in conjunction with check in order to fully check if a team is in checkmate
 */
int inCheckmate(grid *board, int col){
  moveList temp;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){                
      if((*board).plane[i][j]/10 == col){              
        getMovesPiece1(board,&temp, i, j, 0);  
        //If 1 or more valid moves exist, there is no checkmate
        if(temp.len>0){ 
          return 0;
        }
      }
    }
  }
  return 1; 
}


//Checks if the given colour can castle on the given side (Either King or Queen)
int canCastle(grid *board, int col, int side){ 
  //side being KING or QUEEN
  int x, row, corner;
  if(col == BLACK){
    row = 0;
  } 
  else{
    row = 7;
  }
  if(side == KING){
    corner = 0;
  } else if(side == QUEEN){
    corner = 2;
  }
  //if Rook is King side Rook
  if(castleBool[col][corner] && castleBool[col][1]){
    if(side == KING){
      for(int i = 4; i < 8; i++){
        //check if spaces between and including king and rook can not be 'attacked'
        if(canHit(board, (col+1)%2, i, row) == 1){
          return 0;
        }
        //check if spaces between king and rook are empty
        if(i == 5 || i == 6){
          if((*board).plane[i][row] != 0){
            return 0;
          }
        }
      }
    } 
    //if Rook is Queen side Rook
    else{            
      for(int i = 0; i < 5; i++){
        //check if spaces between and including king and rook can not be 'attacked'
        if(canHit(board, (col+1)%2, i, row) == 1){
          return 0;
        }
        //check if spaces between king and rook are empty
        if(i == 1 || i == 2 || i == 3){
          if((*board).plane[i][row] != 0){
            return 0;
          }
        }
      }
    }
  } 
  else{
    return 0;
  }
  return 1;
}



//Updates castleBool, telling whether any Rooks or Kings have been moved 
void changeCastleBool(void){
  //White pieces
  if(b.plane[7][7] != ROOK) castleBool[0][0] = 0;  
  if(b.plane[4][7] != KING) castleBool[0][1] = 0; 
  if(b.plane[0][7] != ROOK) castleBool[0][2] = 0;
  //Black Pieces
  if(b.plane[7][0] != ROOK + 10) castleBool[1][0] = 0;
  if(b.plane[4][0] != KING + 10) castleBool[1][1] = 0;
  if(b.plane[0][0] != ROOK + 10) castleBool[1][2] = 0;
}



//Copies the content of from to to
void copyBoard(grid* from, grid* to){
  for(int i = 0; i<8; i++){
    for(int j = 0; j<8; j++){
      (*to).plane[i][j] = (*from).plane[i][j];
    }
  }
}



//Returns the number of pieces left on a given board
long piecesLeft(grid* board){
  long sum = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if((*board).plane[i][j]!= 0){
        sum++; 
      }
    }
  }
  return sum;
}



//Checks if the game has reached a tie
int inStalemate(grid* board){
  int toReturn = 0;
  if(inCheckmate(board,WHITE)&&!inCheck(board,WHITE)){
    toReturn = 1;
  }
  if(inCheckmate(board,BLACK)&&!inCheck(board,BLACK)){
    toReturn = 1;
  }
  if(drawCount>50){
    return 1;
  }
  return 0;
}

