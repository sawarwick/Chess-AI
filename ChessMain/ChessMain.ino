#include "Chess.c"
#include "ChessAI.c"

#include <Adafruit_ST7735.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

//FROM ADAFRUIT EXAMPLE SETUP CODE -----------------------------------------------------------------------------------------------------------------------
// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS 10
#define TFT_RST 9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to -1!
#define TFT_DC 8

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define NEUTRAL 0
#define PRESS 1
#define UP 2
#define DOWN 3
#define RIGHT 4
#define LEFT 5
 
// Checks & returns the joystick's current status
int checkJoystick()
{
  int joystickState = analogRead(3);
  
  if (joystickState < 50) return UP;
  if (joystickState < 150) return RIGHT;
  if (joystickState < 250) return PRESS;
  if (joystickState < 500) return DOWN;
  if (joystickState < 650) return LEFT;
  return NEUTRAL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  set(); 
  //Graphics setup
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  draw();
}

int curButton = NEUTRAL;
int lastButton = NEUTRAL;
int chosenX, chosenY;
int chosen;
moveList viewing;
int difficulty = 0;
int x = 0, y = 0;

void loop() {
  lastButton = curButton;
  curButton = checkJoystick();

  if(turn == 0 && curButton != lastButton){
    //Button has been pressed, cursor on board
    if (curButton == PRESS && y<8){ 
      //If no piece is chosen and a moveable piece is in this spot, chose this piece
      if (!chosen && b.plane[x][y] != 0 && turn==b.plane[x][y]/10){
        if (viewing.len != 0){
          chosen = 1;
          chosenX = x;
          chosenY = y; 
        }
      }
      //Moves chosen piece or unselects piece
      else if (chosen){
        if(chosenX == x && chosenY == y){
          chosen = 0;
        }
        for (int i = 0; i < viewing.len; i++){
          if (viewing.list[i][0] == x && viewing.list[i][1] == y){
            chosen = 0;
            playTurn(chosenX, chosenY, x, y);
            break;
          }
        }
      }
    }
    if(curButton == PRESS && y==8){
      difficulty = (difficulty+1)%2;
      chessReset();
    }
    moveCursor();
    if (!chosen && b.plane[x][y] != 0 && turn == b.plane[x][y]/10){
      getMovesPiece(&b, &viewing, x, y);
    }
    else if(!chosen){
      viewing.len = 0;
    }
    draw();
  }
  else if (turn == 1 && !checkmate){
    if(difficulty == 1){
      playAITurnHard(); 
    }
    else{
      playAITurnEasy();
    }
    draw();
  }
}


//Moves current cursor position based on user inputs
void moveCursor(){
  if (curButton == LEFT){
    x = (x - 1 + 8) % 8;
  }
  if (curButton == RIGHT){
    x = (x + 1) % 8;
  }
  if (curButton == DOWN){
    y = (y - 1 + 9) % 9;
  }
  if (curButton == UP){
    y = (y + 1) % 9;
  }  
}


/* Main drawing function
 * Draws chess board and pieces
 * Draws all informational selectors (valid moves, cursor, last moved position)
 * Writes check, checkmate, stalemate on the screen if they occur
 * Writes the number of moves performed on the screen
 */
void draw(){
  //Colours
  int GREEN = 0x0660;
  int W = 0xFFFF;
  int RED = 0xF000;
  int CURSOR = 0xB00A; 
  int CURSOR2 = 0xD88D;
  int BLUE = 0x441F;

  int side = 16;


  //Draws chessboard pattern
  for(int i = 0; i<8; i++){
    for(int j = 0; j<8; j++){
      if((i+j)%2==0){
        tft.fillRect(i*side, j*side, side, side, W);
      }
      else{
        tft.fillRect(i*side, j*side, side, side, GREEN);
      }
    }
  }


  //Displays the valid moves of the selected pieces
  for (int i = 0; i < viewing.len; i++){
    tft.fillRect(viewing.list[i][0]*side,viewing.list[i][1]*side,side,side,BLUE);
  }
  
  //Draws chess pieces
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      drawPiece(b.plane[i][j], i*side, j*side);
    }
  }


  
  //Rectangles to draw text on
  tft.fillRect(0, tft.width()+12, tft.width(),tft.height()-tft.width()-8,0xFFFF);
  tft.fillRect(0,tft.width(),tft.width(),12,0x0000);

  //Drawing text on the screen
  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.setCursor(2,tft.width()+2);
  tft.print(moveCount);
  tft.setCursor(tft.width()-27,tft.width()+2);
  if(difficulty == 0){
    tft.print("Easy");
  }
  else{
    tft.print("Hard");
  }

  tft.setTextSize(2);
  if (turn == BLACK){
    tft.setTextColor(RED);
  }
  else{
    tft.setTextColor(0x0000);
  }
  if(stalemate){
    tft.setCursor(10, tft.width()+13);
    tft.print("Stalemate");  
  }
  else if (checkmate){
    tft.setCursor(10, tft.width()+15);
    tft.print("Checkmate");
  }
  else if (check){
    tft.setCursor(35, tft.width()+13);
    tft.print("Check");
  }
  

  //Draws indicator around last AI move
  if(turn == 0){
    tft.drawRect(side*lastx, side*lasty, side, side, 0x0000);
    tft.drawRect(side*lastx+1, side*lasty+1, side-2, side-2, 0x0000);
  }

  //Draws cursor
  if(y<8){
    tft.drawRect(side*x, side*y, side, side, CURSOR);
    tft.drawRect(side*x+1, side*y+1, side-2, side-2, CURSOR);
  }
  else{
    tft.drawRect(tft.width()-29,tft.width(),27,12,CURSOR2);
    tft.drawRect(tft.width()-28,tft.width()+1,25,10,CURSOR2);
  }
}


//For a piece at the board position x,y draws the piece at the relevent position
void drawPiece(int piece, int x, int y){
  int col;
  if (piece/10 == BLACK){
    col = 0x0000;
  }
  else{
    col = 0xF000;
  }
  if (piece%10 == PAWN){
    tft.fillRect(x+7,  y+4,  2,  10, col);
    tft.fillRect(x+6,  y+5,  4,  4,  col);
    tft.fillRect(x+5,  y+6,  6,  2,  col);
    tft.fillRect(x+6,  y+12, 4,  1,  col);
    tft.fillRect(x+5,  y+13, 6,  1,  col);
  }
  if (piece%10==ROOK){
    tft.fillRect(x+5,  y+4,  6,  10, col);
    tft.fillRect(x+3,  y+4,  10, 2,  col);
    tft.fillRect(x+4,  y+6,  8,  1,  col);
    tft.fillRect(x+4,  y+12, 8,  1,  col);
    tft.fillRect(x+3,  y+13, 10, 1,  col);
    tft.fillRect(x+3,  y+2,  1,  2,  col);
    tft.fillRect(x+5,  y+2,  2,  2,  col);
    tft.fillRect(x+9,  y+2,  2,  2,  col);
    tft.fillRect(x+12, y+2,  1,  2,  col);
  }
  if (piece%10 == KNIGHT){
    tft.fillRect(x+6,  y+2,  1,  1,  col);
    tft.fillRect(x+5,  y+3,  4,  2,  col);
    tft.fillRect(x+9,  y+4,  1,  1,  col);
    tft.fillRect(x+4,  y+5,  3,  3,  col);
    tft.fillRect(x+3,  y+7,  3,  2,  col);
    tft.fillRect(x+4,  y+9,  1,  1,  col);
    tft.fillRect(x+7,  y+5,  4,  5,  col);
    tft.fillRect(x+6,  y+10, 4,  1,  col);
    tft.fillRect(x+5,  y+12, 7,  1,  col);
    tft.fillRect(x+4,  y+13, 9,  1,  col);
    tft.fillRect(x+11, y+6,  1,  2,  col);
    tft.fillRect(x+6,  y+11, 5,  1,  col);
  }
  if (piece%10 == BISHOP){
    tft.fillRect(x+7,  y+1,  1,  1,  col);
    tft.fillRect(x+6,  y+2,  3,  1,  col);
    tft.fillRect(x+7,  y+3,  3,  1,  col);
    tft.fillRect(x+8,  y+4,  3,  3,  col);
    tft.fillRect(x+5,  y+4,  1,  3,  col);
    tft.fillRect(x+6,  y+5,  1,  3,  col);
    tft.fillRect(x+6,  y+6,  4,  2,  col);
    tft.fillRect(x+7,  y+6,  2,  8,  col);
    tft.fillRect(x+6,  y+11, 4,  1,  col);
    tft.fillRect(x+5,  y+12, 6,  1,  col);
    tft.fillRect(x+4,  y+13, 8,  1,  col);
  }
  if (piece%10==QUEEN){
    tft.fillRect(x+7,  y+2,  2,  9,  col);
    tft.fillRect(x+6,  y+3,  4,  3,  col);
    tft.fillRect(x+5,  y+4,  6,  1,  col);
    tft.fillRect(x+6,  y+7,  4,  1,  col);
    tft.fillRect(x+6,  y+10, 4,  2,  col);
    tft.fillRect(x+5,  y+12, 6,  1,  col);
    tft.fillRect(x+4,  y+13, 8,  1,  col);
  }
  if (piece%10 == KING){
    tft.fillRect(x+7,  y+1,  2,  13, col);
    tft.fillRect(x+6,  y+2,  4,  2,  col);
    tft.fillRect(x+4,  y+5,  8,  2,  col);
    tft.fillRect(x+5,  y+7,  6,  1,  col);
    tft.fillRect(x+6,  y+8,  4,  1,  col);
    tft.fillRect(x+5,  y+12, 6,  1,  col);
    tft.fillRect(x+4,  y+13, 8,  1,  col);
  }
}
