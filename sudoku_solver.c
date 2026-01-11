#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool ok(int** sudoku, int n, int row, int col, int dig){
    //check in row
    for(int i=0; i<n; i++){
        if(sudoku[row][i]==dig){
            return false;
        }
    }

    //check in col
    for(int i=0; i<n; i++){
        if(sudoku[i][col]==dig){
            return false;
        }
    }

    //check in box
    int sr=(row/3)*3 , sc=(col/3)*3;
    for(int i=sr; i<=sr+2; i++){
        for(int j=sc; j<=sc+2; j++){
            if(sudoku[i][j]==dig){
                return false;
            }
        }
    }

    return true;
}

bool ss(int** sudoku, int n, int row, int col){
    if(row==n){
        return true;
    }

    int nextcol=col+1;
    int nextrow=row;
    if(nextcol==n){
        nextrow=row+1;
        nextcol=0;
    }
   
    if(sudoku[row][col]!=0){
        return ss(sudoku,n,nextrow,nextcol);
    }else{
        for(int i=1; i<=n; i++){
            if(ok(sudoku,n,row,col,i)){
                sudoku[row][col]=i;
                if(ss(sudoku,n,nextrow,nextcol)){
                    return true;
                }else{
                    sudoku[row][col]=0;
                }
            }
        }
    }
    return false;
}

int main(){
    int n =9;
    //creating 2d matrix
    int ** sudoku =(int**)malloc(n*sizeof(int*));
    for(int i=0; i<n; i++){
        sudoku[i]=(int*)malloc(n*sizeof(int));
    }
    //filling inputs
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            scanf("%d", &sudoku[i][j]);
        }
    }
    if(ss(sudoku,n,0,0)){
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                printf("%d ",sudoku[i][j]);
            }
            printf("\n");
        }
    }
    return 0;
}
