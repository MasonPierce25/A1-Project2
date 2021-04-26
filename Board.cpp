/* 
 * File:   Game.cpp
 * Author: Avery Wylin
 * 
 */

#include <iostream>

#include "Board.h"

using std::cout;
using std::endl;


Board::Board()
{   
    //clear possible unused data
    clearBoardState();
}

Board::Board(Board& other){
    for(int i=0;i<WIDTH*HEIGHT;i++){
        boardState[i]=other.boardState[i];
    }
    for(int i=0;i<WIDTH;i++){
        stackSize[i]=other.stackSize[i];
    }
    evaluatedState = other.evaluatedState;
    path = other.path;
    pathLength = other.pathLength;
}

Board::~Board() {
}

/*
 * Prints the game state in console form.
 */
void Board::printBoardState(){
    static char symbols[] = {' ','O','X'};
    for (int x = 0; x < WIDTH; x++) {
        cout<<'\t'<<'C'<<x+1;
    }
    cout<<'\n';
    for (int y = HEIGHT-1; y >= 0; y--) {
        cout<<'R'<<y+1<<'\t';
        for (int x = 0; x < WIDTH; x++) {
            cout<<symbols[getBoardState(x,y)]<<"\t";
        }
        cout<<"\n\n";
    }
}
/*
 * Clears the state of the game to zero(empty).
 */
void Board::clearBoardState() {
    //empty the gameState
    for (int i = 0; i <WIDTH*HEIGHT; i++) {
        boardState[i]=0;
    }
    //empty the stackSize
    for (int x = 0; x < WIDTH; x++) {
        stackSize[x] = 0;
    }
    //clear the path
    path = 0;
}

/*
 * Places a piece at a certain column. Returns false if the column is full or an error occured. 
 */
bool Board::placePiece(int x, uint8_t playerByte){
    if(x>WIDTH-1 || x<0){
        cout<<"Error: tried to place piece out of bounds at "<<x<<endl;
        return false;
    }
    if(stackSize[x]>=HEIGHT){
        //natural error, stack is full
        return false;
    }
    if(playerByte == 0){
        cout<<"Error: Player must be a non-zero value"<<endl;
        return false;
    }
    path <<= PATH_SIZE;
    path|=x;
    pathLength++;
    setBoardState(x,stackSize[x],playerByte);
    stackSize[x]++;
    return true;
}

bool Board::removePiece(int x, uint8_t playerByte){
    if(x>WIDTH-1 || x<0){
        cout<<"Error: tried to remove piece out of bounds at "<<x<<endl;
        return false;
    }
    if(stackSize[x]==0){
        cout<<"Error: Column "<< x <<" is empty"<<endl;
        return false;
    }
    if(playerByte == 0){
        cout<<"Error: Player must be a non-zero value"<<endl;
        return false;
    }
    path>>=PATH_SIZE;
    pathLength--;
    setBoardState(x,stackSize[x],0);
    stackSize[x]--;
    return true;
}

void Board::setBoardState(int x, int y, uint8_t value){
    if(x<0 || x>=WIDTH || y<0 || y>= HEIGHT)
        cout<<"Error: Cant set a value at: "<<x<<","<<y<<endl;
    else
    boardState[y*WIDTH+x]=value;
}

uint8_t Board::getBoardState(int x, int y) {
    if(x<0 || x>=WIDTH || y<0 || y>= HEIGHT){
        cout<<"Error: Cant retrieve a value at: "<<x<<","<<y<<endl;
        return 0;
    }
    return  boardState[y*WIDTH+x];
}

int Board::getFirstPath(){
    return path>>(PATH_SIZE*(pathLength-1));
}

bool Board::isFull(){
    for(int i=0;i<WIDTH;i++){
        if(stackSize[i]<HEIGHT){
            return false;
        }
    }
}

