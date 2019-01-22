#include <stdio.h>
#include <stdbool.h>
#include "testwin.h"

int checkWin(int x,int y,char board[N][N],char flagWin){
	int i,j,cou1=0,cou2=0;
	for(i=y+1;i<N;i++)
		if(board[x][i]==flagWin)
			cou1++;
		else break;
	for(i=y-1;i>0;i--)
		if(board[x][i]==flagWin)
			cou1++;
		else break;
	if(cou1>=4) return 1;
	cou1=0;



	for(i=x+1;i<N;i++)
		if(board[i][y]==flagWin)
			cou1++;
		else break;
	for(i=x-1;i>0;i--)
		if(board[i][y]==flagWin)
			cou1++;
		else break;
	if(cou1>=4) return 1;
	cou1=0;



	for(i=x+1,j=y+1;i<N,y<N;i++,j++)
		if(board[i][j]==flagWin)
			cou1++;
		else break;
	for(i=x-1,j=y-1;i>0,j>0;i--,j--)
		if(board[i][j]==flagWin)
			cou1++;
		else break;
	if(cou1>=4) return 1;
	cou1=0;



	for(i=x-1,j=y+1;i>0,j<N;i--,j++)
		if(board[i][j]==flagWin)
			cou1++;
		else break;
	for(i=x+1,j=y-1;i<N,j>0;i++,j--)
		if(board[i][j]==flagWin)
			cou1++;
		else break;
	if(cou1>=4) return 1;
	return 0;
}

