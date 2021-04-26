        /* 
         * File:   main.cpp
         * Author: Avery Wylin
         *
         */

#include <cstdlib>
#include <iostream>
#include <climits>
#include <iomanip>
#include "Board.h"
#include <math.h>
#include <time.h>

        using namespace std;
        
        Board minMaxAB(Board currentBoard, int depth, bool isMax, int alpha, int beta, int (*EvaluationFunction)(Board));
        bool calculateWin(Board &board);
        int evalFunction0(Board);
        int evalFunction1(Board);
        int evalFunction2(Board);
        void runGame(int,int,int,int,int(**evalFunctions)(Board));
        
        struct StatisticObject{
            int currentFunction=0;
            int totalNodesExpanded[3]={0,0,0};
            int totalNodesGenerated[3]={0,0,0};
            int nodesExpanded[3]={0,0,0};
            int nodesGenerated[3]={0,0,0};
            void print(int function){
                cout<<"\nEV Function: "<<function+1<<endl;
                cout<<"Nodes Expanded: "<<nodesExpanded[function]<<endl;
                cout<<"Nodes Generated: "<<nodesGenerated[function]<<endl;
            }
            void printTotals(int function){
                cout<<"\nEV Function: "<<function+1<<endl;
                cout<<"Total Nodes Expanded: "<<totalNodesExpanded[function]<<endl;
                cout<<"Total Nodes Generated: "<<totalNodesGenerated[function]<<endl;
            }
            void clear(){
                for(int i=0;i<3;i++){
                    nodesExpanded[i]=0;
                    nodesGenerated[i]=0;
                }
            }
        };
        
        StatisticObject stats;
        

int main(int argc, char** argv) {
    int (*evalFunctions[3])(Board);
    evalFunctions[0] = &evalFunction0;
    evalFunctions[1] = &evalFunction1;
    evalFunctions[2] = &evalFunction2;
    
    runGame(0,2,1,2,evalFunctions);
    runGame(0,2,2,4,evalFunctions);
    runGame(1,2,2,8,evalFunctions);
    runGame(0,4,1,2,evalFunctions);
    runGame(0,4,2,4,evalFunctions);
    runGame(1,4,2,8,evalFunctions);
    runGame(0,8,1,2,evalFunctions);
    runGame(0,8,2,4,evalFunctions);
    runGame(1,8,2,8,evalFunctions);
    
    cout<<"\nTOTALS: "<<endl;
    
    stats.printTotals(0);
    stats.printTotals(1);
    stats.printTotals(2);
    char input;
    cout<<"The program has finished. Enter any value to continue."<<endl;
    cin>>input;
    return 0;
}

void runGame(int EVMax, int depthMax, int EVMin, int depthMin, int(**evalFunctions)(Board)){
    Board mainBoard,tempBoard;
    bool isMax = true; //start with max
    
    while(!calculateWin(mainBoard) && !mainBoard.isFull()){
        stats.currentFunction = isMax?EVMax:EVMin;
        //clear the board path of main so it does not append to minmax's path
        mainBoard.path=0;
        mainBoard.pathLength=0;
        //call minmax
        tempBoard=minMaxAB(mainBoard,isMax?depthMax:depthMin,isMax,INT_MIN,INT_MAX,evalFunctions[isMax?EVMax:EVMin]);
        //play the value determined by minmax
        mainBoard.placePiece(tempBoard.getFirstPath(),isMax?2:1);
        //print the board
        mainBoard.printBoardState();
        cout<<(isMax?"Max EV: ":"Min EV: ")<<tempBoard.evaluatedState<<endl;
        //swap player
        isMax = !isMax;
    }
    //the game was won
        cout<<"\n"<<"MAX: "<<EVMax+1<<" DEPTH: "<<depthMax<<" vs MIN: "<<EVMin+1<<" DEPTH: "<<depthMin<<endl;
        stats.print(EVMax);
        stats.print(EVMin);
        stats.totalNodesGenerated[EVMax]+=stats.nodesGenerated[EVMax];
        stats.totalNodesGenerated[EVMin]+=stats.nodesGenerated[EVMin];
        stats.totalNodesExpanded[EVMax]+=stats.nodesExpanded[EVMax];
        stats.totalNodesExpanded[EVMin]+=stats.nodesExpanded[EVMin];
        stats.clear();
}

Board minMaxAB(Board currentBoard, int depth, bool isMax,int alpha, int beta, int (*EvaluationFunction)(Board)){
    //BASE CASE cutoff depth reaches
    if(depth==0){
        //compute the evaluated score of the board and couple it with the board
        currentBoard.evaluatedState=EvaluationFunction(currentBoard);
        //return the board with its computed score and path
        stats.nodesGenerated[stats.currentFunction]++;
        return currentBoard;
    }
    
    Board bestSuccessorBoard = currentBoard; //this stores the Maximum succesor
    Board tempBoard; //this stores an arbitrary successor, used for comparison
    bool init=true; //whether bestSuccessorBoard requires initialization
    
    
    //RECURSIVE CASE
    //dispatch a list of plays
    for(int i=0;i<Board::WIDTH;i++){
        //create a new play
        tempBoard=currentBoard;
        if(tempBoard.placePiece(i,isMax?2:1)){
            stats.nodesExpanded[stats.currentFunction]++;
            //valid play was made, call recursive algorithm
            tempBoard = minMaxAB(tempBoard,depth-1,!isMax,alpha,beta,EvaluationFunction);
            //initializes the best succesor once, prevents returning the parent by accident
            if(init){
                bestSuccessorBoard=tempBoard;
                init = false;
            }
//            cout<<i<<" : "<<tempBoard.evaluatedState<<endl;
            //choose the best successor or break using Alpha Beta Pruning
            if(isMax){
                //update alpha if the current board is greater
                alpha = tempBoard.evaluatedState>alpha?tempBoard.evaluatedState:alpha;
                //if alpha is not a better option, ignore this subtree, min will choose a worse option
                if(beta<=alpha)
                    break;
                //max chooses the greater of the boards
                if(tempBoard.evaluatedState>bestSuccessorBoard.evaluatedState){
                    bestSuccessorBoard=tempBoard;
                }
            }
            else{
                //update beta if the current board is less
                  beta = tempBoard.evaluatedState<beta?tempBoard.evaluatedState:beta;
                //if alpha is not a better option, ignore this subtree, min will choose a worse option
                if(beta<=alpha)
                    break;
                //min chooses the lesser of the boards
                if(tempBoard.evaluatedState<bestSuccessorBoard.evaluatedState){
                    bestSuccessorBoard=tempBoard;
                }
            }
        }
    }
    //return the successor chosen by min or max
    return bestSuccessorBoard;
}

bool calculateWin(Board &board){
    int consecutive=0;
    uint8_t lastPlayer=0;
    
    //check column cases
    for(int x=0;x<Board::WIDTH;x++){
        lastPlayer = board.getBoardState(x,0);
        //we can skip a column if it is empty
        if(lastPlayer==0)
            continue;
        
        for(int y=1;y<Board::HEIGHT;y++){
           if(lastPlayer!=0 && board.getBoardState(x,y)==lastPlayer){
               consecutive++;
           }
           else{
               consecutive = 0;
           }
           lastPlayer=board.getBoardState(x,y);
           if(consecutive == 3){
               if(lastPlayer==1){
                   cout<<"Min ";
               }
               else{
                   cout<<"Max ";
               }
               cout<<"won using a Column case."<<endl;
               return true;
           }
        }
    }
    
    consecutive = 0;
    //check row cases
    for(int y=0;y<Board::HEIGHT;y++){
        lastPlayer = board.getBoardState(0,y);
        for(int x=1;x<Board::WIDTH;x++){
            if(lastPlayer!=0 && board.getBoardState(x,y)==lastPlayer){
               consecutive++;
           }
           else{
               consecutive = 0;
           }
           lastPlayer=board.getBoardState(x,y);
           if(consecutive == 3){
               if(lastPlayer==1){
                   cout<<"Min ";
               }
               else{
                   cout<<"Max ";
               }
               cout<<"won using a Row case."<<endl;
               return true;
           }
        }
    }
    
     consecutive = 0;
    //check diagnal decreasing
    
    for(int z=3;z<=Board::WIDTH+Board::HEIGHT-5;z++){
        
        int x = z>=Board::HEIGHT?(z-Board::HEIGHT+1):0;
        lastPlayer = board.getBoardState(x,z-x);
        x++;
        for(;x<Board::WIDTH && x<=z;x++) {
           int y = z - x;
           if(lastPlayer!=0 && board.getBoardState(x,y)==lastPlayer){
           consecutive++;
           }
           else{
               consecutive = 0;
           }
           
           lastPlayer=board.getBoardState(x,y);
           if(consecutive == 3){
               if(lastPlayer==1){
                   cout<<"Min ";
               }
               else{
                   cout<<"Max ";
               }
               cout<<"won using a Decreasing Diagnal case."<<endl;
               return true;
           }
        }
    }
     
     consecutive=0;
 //check diagnal increasing
     for(int z=3;z<=Board::WIDTH+Board::HEIGHT-5;z++){
        
        int x = z>=Board::HEIGHT?(z-Board::HEIGHT+1):0;
        lastPlayer = board.getBoardState(x,z-x);
        x++;
        for(;x<Board::WIDTH && x<=z;x++) {
           int y = z - x;
           if(lastPlayer!=0 && board.getBoardState(Board::WIDTH-1-x,y)==lastPlayer){
           consecutive++;
           }
           else{
               consecutive = 0;
           }
           
           lastPlayer=board.getBoardState(Board::WIDTH-1-x,y);
           if(consecutive == 3){
               if(lastPlayer==1){
                   cout<<"Min ";
               }
               else{
                   cout<<"Max ";
               }
               cout<<"won using an Increasing Diagnal case."<<endl;
               return true;
           }
        }
    }
    return false;
}

int evalFunction0(Board board){
    
    int bestMin=0;
    int bestMax=0;
    int min=0;
    int max=0;
    
    uint8_t lastPlayer=0;
    
    //check column cases
    for(int x=0;x<Board::WIDTH;x++){
        //columns are skippable if empty
        if(board.getBoardState(x,0)==0)
            continue;
        lastPlayer = board.getBoardState(x,0);
        min=max=0;
        for(int y=1;y<Board::HEIGHT;y++){
            if(board.getBoardState(x,y)==0)
                continue;
            if(board.getBoardState(x,y)==lastPlayer){
                if(lastPlayer == 1){
                   min++;
                }
                else if(lastPlayer == 2){
                    max++;
                }
            }
            else{
                if(y>Board::HEIGHT-4){
                    //no possible win can be made, column too full
                    min=max=0;
                    break;
                }
                if(lastPlayer == 1){
                   min=0;
                }
                else if(lastPlayer == 2){
                   max=0;
                }
            }
            lastPlayer=board.getBoardState(x,y);
        }
            bestMin=bestMin<min?min:bestMin;
            bestMax=bestMax<max?max:bestMax;
    }
    
    
    //check row cases
    min=max=0;
    for(int y=0;y<Board::HEIGHT;y++){
        lastPlayer = board.getBoardState(0,y);
        min=max=0;
        for(int x=1;x<Board::WIDTH;x++){
            if(board.getBoardState(x,y)==0)
                continue;
            if(board.getBoardState(x,y)==lastPlayer){
                if(lastPlayer == 1){
                   min++;
                }
                else if(lastPlayer == 2){
                    max++;
                }
            }
            else{
                if(x>Board::WIDTH-4){
                    //no possible win can be made, row too full
                    min=max=0;
                    break;
                }
                if(lastPlayer == 1){
                   min=0;
                }
                else if(lastPlayer == 2){
                   max=0;
                }
            }
            lastPlayer=board.getBoardState(x,y);
        }
            bestMin=bestMin<min?min:bestMin;
            bestMax=bestMax<max?max:bestMax;
    }
    
    //check diagnal decreasing
    min=max=0;
    //3 comes from 4 in a row - 1, 5 comes from (4 in a row -1)+2(index offset for both row & column)
    for(int z=3;z<=Board::WIDTH+Board::HEIGHT-5;z++){
        int x = z>=Board::HEIGHT?(z-Board::HEIGHT+1):0;
        lastPlayer = board.getBoardState(x,z-x);
        x++;
        min=max=0;
        for(;x<Board::WIDTH && x<=z;x++) {
           int y = z - x;
           if(board.getBoardState(x,y)==0)
                continue;
            if(board.getBoardState(x,y)==lastPlayer){
                if(lastPlayer == 1){
                   min++;
                }
                else if(lastPlayer == 2){
                    max++;
                }
            }
           else{
                if(lastPlayer == 1){
                   min=0;
                }
                else if(lastPlayer == 2){
                   max=0;
                }
            }
            lastPlayer=board.getBoardState(x,y);
        }
            bestMin=bestMin<min?min:bestMin;
            bestMax=bestMax<max?max:bestMax;
    }
    
    //check diagnal increasing
    min=max=0;
     for(int z=3;z<=Board::WIDTH+Board::HEIGHT-5;z++){
        int x = z>=Board::HEIGHT?(z-Board::HEIGHT+1):0;
        lastPlayer = board.getBoardState(x,z-x);
        x++;
        min=max=0;
        for(;x<Board::WIDTH && x<=z;x++) {
           int y = z - x;
            if(board.getBoardState(Board::WIDTH-1-x,y)==0)
                continue;
            if(board.getBoardState(Board::WIDTH-1-x,y)==lastPlayer){
                if(lastPlayer == 1){
                   min++;
                }
                else if(lastPlayer == 2){
                    max++;
                }
            }
           else{
                if(lastPlayer == 1){
                   min=0;
                }
                else if(lastPlayer == 2){
                   max=0;
                }
            }
            lastPlayer=board.getBoardState(Board::WIDTH-1-x,y);
        }
            bestMin=bestMin<min?min:bestMin;
            bestMax=bestMax<max?max:bestMax;
    }
     return pow(bestMax,2) - pow(bestMin,2);
}

int evalFunction1(Board board){
    uint8_t player =  board.pathLength%2+1;
    uint8_t opposite = player==1?2:1;
    uint8_t temp=0;
    int frameSize=4;
    int value=0;
    static int playerValueMap[5]={0,1,10,100,1000};
    int playerCount=0;
    int oppositeCount=0;
    
    //column count
    for(int x=0;x<Board::WIDTH;x++){
        for(int y=0;y<=Board::HEIGHT-frameSize;y++){
            //count in frame
            for(int i=0;i<frameSize;i++){
                temp = board.getBoardState(x,y+i);
                if(temp==player)
                    playerCount++;
                else if(temp==opposite)
                    oppositeCount++;
            }
            //add to value
            
            if(oppositeCount==0){
            //player is progressing
            value+=playerValueMap[playerCount];
            }
            
            if(oppositeCount==3&&playerCount==0){
            //player will lose
            value-=100000;
            }
            
            playerCount=oppositeCount=0;
        }
    }
    
    //row count
    for(int y=0;y<Board::HEIGHT;y++){
        for(int x=0;x<=Board::WIDTH-frameSize;x++){
            //count in frame
            for(int i=0;i<frameSize;i++){
                temp = board.getBoardState(x+i,y);
                if(temp==player)
                    playerCount++;
                else if(temp==opposite)
                    oppositeCount++;
            }
            //add to value
            
            if(oppositeCount==0){
            //player is progressing
            value+=playerValueMap[playerCount];
            }
            
            if(oppositeCount==3&&playerCount==0){
            //player will lose
            value-=100000;
            }
            
            playerCount=oppositeCount=0;
        }
    }
    
    //decrese count
    for(int y=Board::HEIGHT-frameSize+1;y<Board::HEIGHT;y++){
        for(int x=0;x<=Board::WIDTH-frameSize;x++){
             //count in frame
            for(int i=0;i<frameSize;i++){
                temp = board.getBoardState(x+i,y-i);
                if(temp==player)
                    playerCount++;
                else if(temp==opposite)
                oppositeCount++;
            }
            //add to value
            
            if(oppositeCount==0){
            //player is progressing
            value+=playerValueMap[playerCount];
            }
            
            if(oppositeCount==3&&playerCount==0){
            //player will lose
            value-=100000;
            }
            
            playerCount=oppositeCount=0;
        }
    }
    
    //increase count
    for(int y=0;y<=Board::HEIGHT-frameSize;y++){
        for(int x=0;x<=Board::WIDTH-frameSize;x++){
             //count in frame
            for(int i=0;i<frameSize;i++){
                temp = board.getBoardState(x+i,y+i);
                if(temp==player)
                    playerCount++;
                else if(temp==opposite)
                oppositeCount++;
            }
            //add to value
            
            if(oppositeCount==0){
            //player is progressing
            value+=playerValueMap[playerCount];
            }
            
            if(oppositeCount==3&&playerCount==0){
            //player will lose
            value-=100000;
            }
            
            playerCount=oppositeCount=0;
        }
    }
    
    return value; 
    
}

int evalFunction2(Board board) {
    
        int lastPlay = board.path & 3;
        int rowVal = board.stackSize[lastPlay] - 1;
        int x = lastPlay;
        int y = rowVal;
        if(y<0)
            y=0;
        int score = 0;
        int player = (board.pathLength % 2) + 1;
        int otherPlayer = player == 1 ? 2 : 1;

        for (int i = 1; i < 4; i++) {

            if (x - i >= 0) {
                if (board.getBoardState(x - i, y) == player)
                    score++;
                else if (board.getBoardState(x - i, y) == otherPlayer)
                    break;
            }

        };

        for (int i = 1; i < 4; i++) {

            if (x + i < Board::WIDTH) {
                if (board.getBoardState(x + i, y) == player)
                    score++;
                else if (board.getBoardState(x + i, y) == otherPlayer)
                    break;
            }

        };

        for (int i = 1; i < 4; i++) {

            if (y - i >= 0) {
                if (board.getBoardState(x, y - i) == player)
                    score++;
                else if (board.getBoardState(x, y - i) == otherPlayer)
                    break;
            }

        };

        for (int i = 1; i < 4; i++) {

            if (y + i < Board::HEIGHT) {
                if (board.getBoardState(x, y + i) == player)
                    score++;
                else if (board.getBoardState(x, y + 1) == otherPlayer)
                    break;
            }

        };

        for (int i = 1; i < 4; i++) {

            if (x - i >= 0 && y + i < Board::HEIGHT) {
                if (board.getBoardState(x - i, y + i) == player)
                    score++;
                else if (board.getBoardState(x - i, y + i) == otherPlayer)
                    break;
            }

        };

        for (int i = 1; i < 4; i++) {

            if (x + i < Board::WIDTH && y - i >= 0) {
                if (board.getBoardState(x + i, y - i) == player)
                    score++;
                else if (board.getBoardState(x + i, y - i) == otherPlayer)
                    break;
            }

        };

        for (int i = 1; i < 4; i++) {

            if (x - i >= 0 && y - i >= 0) {
                if (board.getBoardState(x - i, y - i) == player)
                    score++;
                else if (board.getBoardState(x - i, y - i) == otherPlayer)
                    break;
            }

        };
        for (int i = 1; i < 4; i++) {

            if (x + i < Board::WIDTH && y + i < Board::HEIGHT) {
                if (board.getBoardState(x + i, y + i) == player)
                    score++;
                else if (board.getBoardState(x + i, y + i) == otherPlayer)
                    break;
            }

        };

        return score;
}
