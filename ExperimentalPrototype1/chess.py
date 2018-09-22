##CHESSBOARD SETUP
blackPieces = ['R','P','B','N','K','Q']
whitePieces = ['r','p','b','n','k','q']


#Newly Setup Chessboard
blankBoard = [
['R','P','','','','','p','r'],
['N','P','','','','','p','n'],
['B','P','','','','','p','b'],
['Q','P','','','','','p','q'],
['K','P','','','','','p','k'],
['B','P','','','','','p','b'],
['N','P','','','','','p','n'],
['R','P','','','','','p','r']]



cols = ['w','b']
b = []
turn = 0
enPas = None
check = False
checkmate = False
#[1] = black [0] = white | king side rook = [0], king= [1], queen side rook=[2] 
castleBool = [[True,True,True],[True,True,True]]

#moves the piece and does the nessesary stuff for the weird moves
def playTurn(bp,cp):
    global turn
    global check
    global checkmate
    global b
    global enPas
    if checkmate: #Stops fn if there is checkmate
        return
    #moves piece

    b = getMovedBoard(b,cp,bp)

    #Sets the current en passent value if there is one
    if b[bp[0]][bp[1]] in ['p','P'] and abs(cp[1]-bp[1])==2:
        enPas = bp[:]
    else:
        enPas = None

    #Changes castled bools
    changeCastleBool()
       
    
    turn = (turn+1)%2#changes turn
    if inCheck(b,cols[turn]):
        check = True
        if isCheckMate(b,cols[turn]):
            checkmate = True
    else:
        check = False

def changeCastleBool():
    global b
    global castleBool
    if b[0][0] != 'R':
        castleBool[1][2] = False
    if b[7][0] != 'R':
        castleBool[1][0] = False
    if b[0][7] != 'r':
        castleBool[0][2] = False
    if b[7][7] != 'r':
        castleBool[0][0] = False
    if b[4][0] != 'K':
        castleBool[1][1] = False
    if b[4][7] !='k':
        castleBool[0][1] = False
    

#resets the chess game
def chessReset():
    global b
    global turn
    global enPas
    global check
    global checkmate
    global castleBool
    b = [i[:] for i in blankBoard]
    turn = 0
    enPas = None
    check = False
    checkmate = False
    castleBool = [[True,True,True],[True,True,True]]

#returns playboard
def getBoard():
    return b

#Returns turn

def getTurn():
    return turn

def getCheck():
    return check

def getCheckMate():
    return checkmate
    
#return True if a piece of (col) colour can land on given point
def canHit(board, col, point):
    for i in range(8):
        for j in range(8):
            if col == "b":
                if board[i][j] == board[i][j].upper():
                    possibleMoves = getMovesPiece(board, [i,j], False, False)
                    for x in range(len(possibleMoves)):
                        if possibleMoves[x] == point:
                            return True
            elif col == 'w':
                if board[i][j] == board[i][j].lower():
                    possibleMoves = getMovesPiece(board, [i,j], False, False)
                    for x in range(len(possibleMoves)):
                        if possibleMoves[x] == point:
                            return True
    return False

def getValid(board, colour):
    if colour == 'b':
        cur = blackPieces
    if colour == 'w':
        cur = whitePieces

def getMoves(board, colour):#Will need to be updated for move to C
    if colour == 'b':
        cur = blackPieces
    if colour == 'w':
        cur = whitePieces
    toReturn = [] ##List in format [start point, list of endpoints, ...]
    for i in range(8):
        for j in range(8):
            if board[i][j] in cur:
                toReturn.append([i,j])
                toReturn.append(getMovesPiece(board,[i,j]))
    return toReturn

#find king, see if in check, return true or false
def inCheck(board,col):
    if col == "b":
        col = 'w'
        #find black king - "K"
        for i in range(8):
            for j in range(8):
                if board[i][j] == "K":
                    point = [i,j]
    elif col == "w":
        col = 'b'
        #find white king - "k"
        for i in range(8):
            for j in range(8):
                if board[i][j] == "k":
                    point = [i,j]
    #now have kings position, check if it can be hit
    return canHit(board, col, point)


#Checks if checkmate
def isCheckMate(board,col):
    for i in range(8):
        for j in range(8):
            if col == "b":
                if board[i][j] == board[i][j].upper(): #Checks piece colour
                    possibleMoves = getMovesPiece(board, [i,j])
                    for x in range(len(possibleMoves)):
                        tempboard = [lis[:] for lis in board]
                        tempboard[possibleMoves[x][0]][possibleMoves[x][1]] = board[i][j]
                        tempboard[i][j] = ""
                        if inCheck(tempboard, col) == False:
                            return False
            elif col == 'w':
                if board[i][j] == board[i][j].lower():
                    possibleMoves = getMovesPiece(board, [i,j])
                    for x in range(len(possibleMoves)):
                        tempboard = [lis[:] for lis in board]
                        tempboard[possibleMoves[x][0]][possibleMoves[x][1]] = board[i][j]
                        tempboard[i][j] = ""
                        if inCheck(tempboard, col) == False:
                            return False
    return True


#castleBool = [[0,0,0],[0,0,0]] where [w,b] and [ks,k,qs]
def canCastle(board, col, side):
    global castleBool
    if inCheck(board,col) == True:
            return False
    if col == 'w':
        castleX = 0
        row = 7
        col = 'b'
    elif col == 'b':
        castleX = 1
        row = 0
        col = 'w'
    if side == 'k':
        corner = 0
    elif side == 'q':
        corner = 2
        
    if castleBool[castleX][corner] == True and castleBool[castleX][1] == True:
        if side == 'k':
            for x in range (4):
                if canHit(board, col, [x+4,row]) == True:
                    return False
                if x == 1 or x == 2:
                    if board[x+4][row] != "":
                        return False
        elif side == 'q':
            for x in range (5):
                if canHit(board, col, [x,row]) == True:
                    return False
                if x == 1 or x == 2 or x == 3:
                    if board[x][row] != "":
                        return False
        return True

def getMovedBoard(board,start,end):
    newBoard = [x[:] for x in board]
    newBoard[end[0]][end[1]] = newBoard[start[0]][start[1]]
    newBoard[start[0]][start[1]] = ''

    #Turns pawns that reach the end to queens
    if newBoard[end[0]][end[1]] == 'P' and end[1] == 7:
       newBoard[end[0]][end[1]] = 'Q'
    if newBoard[end[0]][end[1]] == 'p' and end[1] == 0:
       newBoard[end[0]][end[1]] = 'q'
    
    if newBoard[end[0]][end[1]].lower() == 'p': #en passant
        if abs(end[0]-start[0]) == 1 and board[end[0]][end[1]] == '':
            newBoard[end[0]][start[1]] = ''

    if newBoard[end[0]][end[1]].lower() == 'k': #castling
        if newBoard[end[0]][end[1]] in whitePieces:
            r = 'r'
        else:
            r = 'R'
        if end[0]-start[0] == 2:## bottom corner rooks ([7,0] and [7,7]):
            newBoard[7][start[1]] = ''
            newBoard[5][start[1]] = r
        if end[0]-start[0] == -2:## bottom corner rooks ([0,0] and [0,7]):
            newBoard[0][start[1]] = ''
            newBoard[3][start[1]] = r

    return newBoard


#Gets all possible moves of a piece at a given point
def getMovesPiece(board, point, flag=True, flag2=True):
    global enPas
    x,y = point
    piece = board[x][y] #Current chess piece
    toReturn = []
    if piece.lower() in ['q','r']: #Straight (Queen/Rook)
        directs = [[1,0],[-1,0],[0,1],[0,-1]] # all possible directions for this piece
        for direct in directs:
            curx = x #x and y to loop with
            cury = y
            while True:
                if not (-1<curx+direct[0]<8 and -1<cury+direct[1]<8): #x&y must be on board
                    break
                curx+=direct[0]
                cury+=direct[1]
                if board[curx][cury] =='':
                    toReturn.append([curx,cury])
                elif (board[curx][cury] in whitePieces) != (piece in whitePieces):#Checks if pieces are different colours
                    toReturn.append([curx,cury])
                    break
                else:
                    break
                
    if piece.lower() in ['q','b']: #Diagonal (Queen/Bishop)
        directs = [[1,1],[1,-1],[-1,1],[-1,-1]] # all possible directions for this piece
        for direct in directs:
            curx = x #x and y to loop with
            cury = y
            while True:
                if not (-1<curx+direct[0]<8 and -1<cury+direct[1]<8): #x&y must be on board
                    break
                curx+=direct[0]
                cury+=direct[1]
                if board[curx][cury] =='':
                    toReturn.append([curx,cury])
                elif (board[curx][cury] in whitePieces) != (piece in whitePieces):#Checks if pieces are different colours
                    toReturn.append([curx,cury])
                    break
                else:
                    break
                
    if piece.lower() == 'p':
        if piece in whitePieces:
            direct = -1;
        else:
            direct = 1;
        if board[x][y+direct] == '':
            toReturn.append([x,y+direct])
            if y == 6 and piece in whitePieces and board[x][y+2*direct] == '':#Pawn moving 2 spots
                toReturn.append([x,y+2*direct])
            if y == 1 and piece in blackPieces and board[x][y+2*direct] == '':
                toReturn.append([x,y+2*direct])
        for i in [-1,1]:#Pawn taking pieces
            if 0<=x+i<=7 and 0<=y+direct<=7 and(board[x+i][y+direct] in whitePieces) != (piece in whitePieces) and board[x+i][y+direct]!='':
                toReturn.append([x+i,y+direct])
        if enPas != None:   #enPas is coordinates that the piece moved to --> [x,y]
            if abs(enPas[0]-x) == 1 and enPas[1]==y and board[enPas[0]][y+direct] == '':
                toReturn.append([enPas[0],y+direct])
        
    if piece.lower() == 'n':
        directs = [[1,1],[1,-1],[-1,1],[-1,-1]]
        turns = [[2,1],[1,2]]
        for direct in directs:
            for turn in turns:
                curx = x+direct[0]*turn[0]
                cury = y+direct[1]*turn[1]
                if 0<=curx<=7 and 0<=cury<=7 and (board[curx][cury] == '' or (board[curx][cury] in whitePieces) != (piece in whitePieces)):
                    toReturn.append([curx,cury])
                    
    if piece.lower() == 'k':
        for i in range(-1,2):
            for j in range(-1,2):
                if i==j==0:
                    pass
                if 0<=x+i<=7 and 0<=y+j<=7 and (board[x+i][y+j] == '' or (board[x+i][y+j] in whitePieces) != (piece in whitePieces)):
                    toReturn.append([x+i,y+j])
        if flag:
            if piece in whitePieces and canCastle(board,'w','q'):
                toReturn.append([2,7])
            if piece in whitePieces and canCastle(board,'w','k'):
                toReturn.append([6,7])
            if piece in blackPieces and canCastle(board,'b','q'):
                toReturn.append([2,0])
            if piece in blackPieces and canCastle(board,'b','k'):
                toReturn.append([6,0])

    if not flag2:
        return toReturn
    toReturnFinal = []
    for pos in toReturn:
        if piece in blackPieces:
            col = 'b'
        else:
            col = 'w'
        if not inCheck(getMovedBoard(board,point,pos),col):
            toReturnFinal.append(pos)

    return toReturnFinal






