/* 
 * File:   Game.h
 * Author: Avery Wylin
 *
 */

#ifndef BOARD_H
#define BOARD_H

class Board {
    
public:
//Public Variables
    
    //Based on most common board size
    static const int WIDTH=7; 
    static const int HEIGHT=6;
    static const int PATH_SIZE=4;
    //stores number of values in a column, used to place pieces
    int stackSize[WIDTH];
    //stores the state of the board, used for Evaluation Function
    //null = 0, player min = 1, player max = 2
   uint8_t boardState[WIDTH*HEIGHT];
    //stores the evaluated value of the board
    int evaluatedState=0;
    //stores the path in PATH_SIZE bits, accessed using bitwise ops
    uint64_t path = 0;
    //how long the path is
    uint8_t pathLength=0;
    
//Public Functions
    Board();
    Board(Board&);
    virtual ~Board();
    
    void printBoardState();
    void clearBoardState();
    bool placePiece(int,uint8_t);
    bool removePiece(int,uint8_t);
    uint8_t getBoardState(int,int);
    void setBoardState(int,int,uint8_t);
    int getFirstPath();
};

#endif /* BOARD_H */

