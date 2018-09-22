from pygame import *
from math import *
from random import *
from chess import *

##PYGAME SETUP
running = True
maxx,maxy = 800,600
screen = display.set_mode((maxx,maxy))

side = maxy//8 ##Side length of one space on chess board

##IMAGE SETUP

blackPics = []
whitePics = []

highlight = Surface((side,side))  # the size of your rect
highlight.set_alpha(150)# alpha level
highlight.fill((0,0,0))

resetRect = Rect(610,10,180,80)


for i in range(6):
    blackPics.append(transform.scale(image.load("b"+blackPieces[i]+".png"), (side,side)))
    whitePics.append(transform.scale(image.load("w"+whitePieces[i]+".png"), (side,side)))
    



def drawAll():
    #Clear All
    screen.fill((0,0,0))
    #Drawing Chessboard
    draw.rect(screen,(0,150,0),(0,0,maxy,maxy))
    draw.rect(screen,(255,255,255),(0,0,maxy,maxy),1)
    for i in range(4):
        for j in range(4):
            draw.rect(screen,(255,255,255),((2*i)*side,(2*j)*side,side,side))
            draw.rect(screen,(255,255,255),((2*i+1)*side,(2*j+1)*side,side,side))
    #Drawing Buttons
    draw.rect(screen,(255,0,0), resetRect)
    #Drawing Valid Moves
    if chosen==1:
        drawValid = chosenValid
    else:
        drawValid = pieceValid
    for pos in drawValid:
        screen.blit(highlight,(pos[0]*side,pos[1]*side))
        #draw.rect(screen,(0,0,0),(pos[0]*side,pos[1]*side,side,side))
    

    #DrawPieces
    for i in range(8):
        for j in range(8):
            if board[i][j] in blackPieces:
                screen.blit(blackPics[blackPieces.index(board[i][j])], (i*side,j*side))
            if board[i][j] in whitePieces:
                screen.blit(whitePics[whitePieces.index(board[i][j])], (i*side,j*side))


    #Draw Check
    if getCheck() and not getCheckMate:
        draw.rect(screen,(255,255,0),(610,510,180,80))
    if getCheckMate() and getTurn() == 1:
        draw.rect(screen,(255,255,255), (0,0,600,600))
    if getCheckMate() and getTurn() == 0:
        draw.rect(screen,(0,0,0), (0,0,600,600))
        #print(1)



def reset():
    global turn
    global boardPos
    global pieceValid
    global chosen
    global turn
    boardPos = [0,0]
    pieceValid = []
    chosen = 0
    chessReset()
    

reset()
while running:
    for e in event.get():
        if e.type == QUIT:
            running = False
        if e.type == MOUSEBUTTONDOWN:
            if len(pieceValid)>0 and chosen == 0:
                chosenPos = boardPos[:]
                chosenValid = pieceValid[:]
                chosen = 1
            elif chosen == 1 and boardPos == chosenPos:
                chosen = 0
            elif chosen == 1 and boardPos in chosenValid:
                playTurn(boardPos, chosenPos)
                chosen = 0
                
    mb = mouse.get_pressed()
    mx,my = mouse.get_pos()
    if mx<600:##gets mouse position on the board
        boardPos = [mx//side,my//side]
    if resetRect.collidepoint(mx,my) and mb[0] == 1:
        reset()
    board = getBoard()
    piece = board[boardPos[0]][boardPos[1]]
    if getTurn() == 0 and piece in whitePieces or getTurn() == 1 and piece in blackPieces:
        pieceValid = getMovesPiece(board,boardPos)
    else:
        pieceValid = []
    drawAll()
    display.flip()
quit()
