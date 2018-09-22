#ifndef CHESS_H
#define CHESS_H



//---Structures---
/* Structure that holds a list of points.
 * Used to represent valid moves for a given piece.
 * len represents length of array, allowing for easy addition of elements
 */
typedef struct{
  int len;
  char list[30][2];
} moveList;



//Structure used for easier use of 2D arrays, represents chessboard
typedef struct{
  char plane[8][8];
} grid;

//---Function Declarations---
int canHit(grid *board, int col, int x, int y);
int canHitLowest(grid *board, int col, int x, int y);
int inCheck(grid *board, int col);
int inCheckmate(grid *board, int col);
int canCastle(grid *board, int col, int side);
void changeCastleBool(void);
void set();
void chessReset();
void playTurn(int x1, int y1, int x2, int y2);
void getMovesPiece(grid *board, moveList *toChange, int x, int y);
void getMovesPiece1(grid *board, moveList *toChange, int x, int y, int flag1);
void getMovesPiece2(grid *board, moveList *toReturn, int x, int y, int flag1, int flag2);
void getMovedBoard(grid *board, int x1, int y1, int x2, int y2);
void copyBoard(grid* from, grid* to);
long piecesLeft(grid* board);
int inStalemate(grid* board);


//---Global Variables---
grid b;//Current game board
int turn;//Current turn
int enPas[2];//Current enPas position
int check;//Whether in check or not
int checkmate;//Whether in checkmate or not
int castleBool[2][3];//Keeps track of if you can castle or not
int chessValues[7];//Point values for individual chess pieces
int drawCount;//Counts moves without a piece being taken or a pawn moving
int stalemate;//Whether in stalemate or not
int moveCount;//Total move count for the game
int lastx;//X coordinate for the last position moved to by the ai
int lasty;//Y coordinate for the last position moved to by the ai


//---Constants---
//Numbers representing certain pieces on the board
#define PAWN 1
#define ROOK 2
#define KNIGHT 3
#define BISHOP 4
#define QUEEN 5
#define KING 6

//Numbers representing certain colours on the board
#define BLACK 1
#define WHITE 0

#endif
