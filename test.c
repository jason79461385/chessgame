#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include <ev.h>
#define move_limit 50
#define counter 7
#define NotExcess 111
FILE *file;
FILE *store_file;
ev_io stdin_watcher;
ev_timer timeout_watcher;
int board[8][8] = {-1,-2,-3,-4,-5,-3,-2,-1,
                -6,-6,-6,-6,-6,-6,-6,-6,
                0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                6, 6, 6, 6, 6, 6 ,6, 6,
                1 ,2, 3, 4, 5, 3 ,2, 1};
int tempboard[8][8] = {-1,-2,-3,-4,-5,-3,-2,-1,
                -6,-6,-6,-6,-6,-6,-6,-6,
                0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                6, 6, 6, 6, 6, 6 ,6, 6,
                1 ,2, 3, 4, 5, 3 ,2, 1};
int player = 1;//white first
int row_counter;
int col_counter;
int inactive_moves;
int checkmate = 0;
int stalemate = 0;
int resigned = 0;
int number_of_pawns = 0;
int number_of_pawns_temp = 0;
int black_rook1_moved;
int black_rook2_moved;
int white_rook1_moved;
int white_rook2_moved;
int white_king_moved;
int black_king_moved;
char choose[1];
char yes[]="Y";
char no[]="N";
char sz[100];
char move[4];
int isValidMove(int [][8],int ,int ,int ,int);
int makeMove(int [][8],int ,int ,int ,int);
int whiteSoldierMove(int [][8],int ,int ,int ,int);
int blackSoldierMove(int [][8],int ,int ,int ,int);
int horseMove(int [][8],int ,int ,int ,int);
int elephantMove(int [][8],int ,int ,int ,int );
int camelMove(int [][8],int ,int ,int ,int );
int kingMove(int [][8],int ,int ,int ,int );
int isCastle(int [][8],int ,int ,int ,int );
int isKingSafe(int [][8]);
int playerHasValidMove(void);
void castle(int ,int ,int ,int );
void pawn_promotion(int ,int );
void printBoard(void);
void printInvalidMove(void);
void printCheckmate(void);
void printStalemate(void);
void printThanks(void);
void print50(void);
void printDraw(void);
void undo(void);
void printCheck(void);
void printVictory(void);
void readfile(void);
void savefile(void);
void replay(void);
void inputmove(EV_P_ ev_io *w, int revents);
void timeout_cb(EV_P_ ev_timer *w, int revents);
int main()
{
    struct ev_loop *loop = EV_DEFAULT;	
    printf("\e[1;1H\e[2J\n\n");//clrscr    
    store_file=fopen("Store_Data.txt","a"); // generate the file which can store the data 
    fclose(store_file);
    store_file=fopen("Store_Data.txt","r");//then read it
    int File_Exit;
    fscanf(store_file,"%d",&File_Exit);//read the first data of the store_file
    black_rook1_moved = 0;
    black_rook2_moved = 0;
    white_rook1_moved = 0;
    white_rook2_moved = 0;
    white_king_moved = 0;
    black_king_moved = 0;
    player = 1;
    number_of_pawns_temp = 32;
    int len;
    char C,c;
    char C_undo[1];
    printf("   ██████╗██╗  ██╗███████╗███████╗███████╗\n");
    printf("  ██╔════╝██║  ██║██╔════╝██╔════╝██╔════╝\n");
    printf("  ██║     ███████║█████╗  ███████╗███████╗\n");
    printf("  ██║     ██╔══██║██╔══╝  ╚════██║╚════██║\n");
    printf("  ╚██████╗██║  ██║███████╗███████║███████║\n");
    printf("   ╚═════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\n");
    printf("\n Do you want to read the instructions? (y/n) - ");
    scanf("%c",&C);
    if (C == 'y' || C == 'Y')
    {
        FILE *fptr;
        fptr = fopen("instructions.txt", "r");
        if (fptr == NULL)
        {
            printf("Cannot open file \n");
        }
        else
        {
            c = fgetc(fptr);
            while (c != EOF)
            {
                printf ("%c", c);
                c = fgetc(fptr);
            }
            fclose(fptr);
        }
    }
   
    printf("\n");
    //read store file or not
    if(File_Exit!=NotExcess&&File_Exit!='\n') 
	{
    	printf("would you want to read the store_file? type in 'Y' or 'N' \n");
    	fflush(stdin);//clear the input buffer
    	scanf("%s",choose);
    	if(strcmp(choose,yes)==0)
	{                      
		readfile();
		file = fopen("Moves.txt","a");
		fclose(file);

	}
	}
   else
	{
	file=fopen("Moves.txt","w");
	fprintf(file,"\n-----------------PREVIOUS GAME'S MOVES---------------------\n");
   	 fclose(file);

	}
	
      
    while (playerHasValidMove()||move[0]=='r'||move[0]=='R')
    {
        if (number_of_pawns_temp == number_of_pawns)
            inactive_moves++;
        else
            inactive_moves = 1;
        number_of_pawns_temp = number_of_pawns;
        if (inactive_moves>move_limit)
            break;
        printBoard();
        if (!isKingSafe(board))
            printCheck();
        //inputmove();
	move[0]='g';//initial
	len=1;//initial
	ev_timer_stop(loop,&timeout_watcher);
	printf("\n%s's turn\tEnter your move :\n",player?"White":"Black");
	ev_io_init (&stdin_watcher,inputmove,/*STDIN_FILENO*/ 0, EV_READ);
	ev_io_start(loop,&stdin_watcher);
	ev_timer_init (&timeout_watcher, timeout_cb, 90, 0.);
        ev_timer_start (loop, &timeout_watcher);
        //ev_timer_stop(loop,&timeout_watcher);
     // now wait for events to arrive
        ev_run (loop, 0);

        //scanf("%s",move);
	if(move[0]!='r'&&move[0]!='R')
	{
        printf("\e[1;1H\e[2J");
	}
        int start[2],end[2];
	if(move[0]!='G'&&move[0]!='g')
	{
        len = (int)strlen(move);
	}
        if (len == 1 && (move[0] == 'g' || move[0] == 'G'))//giveup
        {
            resigned = 1;
            break;
        }//end giveup 


	if(len==1&&((move[0]=='q')||move[0]=='Q'))//quit game and store the file

	{	
		printf("\n would you want to store file? \n if you want to store the file type in 'Y', if not type in 'N' \n");
		scanf("%s",choose);
		while(strcmp(choose,yes)!=0&&strcmp(choose,no)!=0)
		{
			
			 printf("\n would you want to store file? \n if you want to store the file type in 'Y', if not type in 'N' \n");
                	 scanf("%s",choose);
		}

		store_file=fopen("Store_Data.txt","w+");
		if(strcmp(choose,yes)==0)
		{
			savefile();

	
		}
		else if(strcmp(choose,no)==0)
		{
			fprintf(store_file,"%d",NotExcess);
		}
		fclose(store_file);
		break;	
	}//end quit 
	 if ((len == 4) && isalpha(move[0]) && isalpha(move[2]) && isdigit(move[1]) && isdigit(move[3])&&move[0]!='r'&&move[0]!='R' )
	 {
	       printf("Would you want to undo to the last step?'Y'or'N'\n ");//ask undo
               scanf("%s",C_undo);
               fflush(stdin);
	        printf("\e[1;1H\e[2J");
 
	 }		
	
        if ((len == 4) && isalpha(move[0]) && isalpha(move[2]) && isdigit(move[1]) && isdigit(move[3])&&strcmp(C_undo,yes)!=0 )//check the input whether corresponds the order (alpha number alpha number)
        {//check	        

            switch(move [0])
            {
                case 'a':
                    start[1]=0;
                    break;
                case 'b':
                    start[1]=1;
                    break;
                case 'c':
                    start[1]=2;
                    break;
                case 'd':
                    start[1]=3;
                    break;
                case 'e':
                    start[1]=4;
                    start[1]=5;
                    break;
                case 'g':
                    start[1]=6;
                    break;
                case 'h':
                    start[1]=7;
                    break;
		case 'r':
		    replay();
		    break;
                default:
                    printInvalidMove();
                    continue;
            }//
            start[0] = ('9' - move[1]) - 1;
            end[0]= ('9' - move[3]) - 1;
            if (start[0]>8 || end[0]>8)
            {
                printInvalidMove();
                continue;
            }//
            switch(move [2])
            {
                case 'a':
                    end[1]=0;
                    break;
                case 'b':
                    end[1]=1;
                    break;
                case 'c':
                    end[1]=2;
                    break;
                case 'd':
                    end[1]=3;
                    break;
                case 'e':
                    end[1]=4;
                    break;
                case 'f':
                    end[1]=5;
                    break;
                case 'g':
                    end[1]=6;
                    break;
                case 'h':
                    end[1]=7;
                    break;
                default:
                    printInvalidMove();
                    continue;
            }//
            if (isValidMove(board,start[0],start[1],end[0],end[1]))
            {		

                if ((board[start[0]][start[1]] == 6 && start[0] == 1) || (board[start[0]][start[1]] == -6 && start[0] == 6))
                    pawn_promotion(start[0], start[1]);
                if(makeMove(board,start[0],start[1],end[0],end[1]))
                {
                    file = fopen("Moves.txt","a");
                    if (file == NULL)
		    {
                        printf("ERORR!\n");
                    }//
                    else
                    {
                        fprintf(file,"%s's move - %.4s\n",!player?"White":"Black",move);
                        fclose(file);
                    }//
                    switch(board[end[0]][end[1]])
                    {
                        case 5:
                        case -5:
                            if (!player)
                                white_king_moved = 1;
                            else
                                black_king_moved = 1;
                            break;
                        case 1:
                        case -1:
                            if (start[0] == 0 && start[1] == 0)
                                black_rook1_moved = 1;
                            else if (start[0] == 0 && start[1] == 7)
                                black_rook2_moved = 1;
                            else if (start[0] == 7 && start[1] == 0)
                                white_rook1_moved = 1;
                            else if (start[0] == 7 && start[1] == 7)
                                white_rook2_moved = 1;
                            break;
                        default:
                            continue;
                    }//
                    continue;
                }//
                else
                    printInvalidMove();
                printf("\n %s king gets into check !\n",player?"White":"Black");        
	    }//



            else
                printInvalidMove();
        }//check
	
        else
            printInvalidMove();
	
    }
    
    printBoard();
    if(move[0]!='r'&&move[0]!='R')
    {
    file = fopen("Moves.txt","a");
    }
    if (checkmate)
    {
        printCheckmate();
        printVictory();
        fprintf(file,"%s won by checkmate\n",player?"White":"Black");
    }
    else if (stalemate)
    {
        printStalemate();
        printDraw();
        fprintf(file,"Match drawn by stalemate\n");
        
    }
    else if (inactive_moves>move_limit)
    {
        print50();
        printDraw();
        fprintf(file,"Match drawn by 50 move rule\n");
    }
    else if (resigned)
    {
        fprintf(file,"%s resigned\n",player?"White":"Black");
        fprintf(file,"%s won\n",!player?"White":"Black");
        player = player?0:1;
        printVictory();
        
    }
    printThanks();
    if(move[0]!='r'&&move[0]!='R')
    {
    fclose(file);
    }
}//main end 






void printCheck(void)
{
    printf("\x1b[31m");
    if (player)
    {
        printf("┬ ┬┬ ┬┬┌┬┐┌─┐  ┬┌─┬┌┐┌┌─┐  ┬┌┐┌  ┌─┐┬ ┬┌─┐┌─┐┬┌─ ┬\n");
        printf("│││├─┤│ │ ├┤   ├┴┐│││││ ┬  ││││  │  ├─┤├┤ │  ├┴┐ │\n");
        printf("└┴┘┴ ┴┴ ┴ └─┘  ┴ ┴┴┘└┘└─┘  ┴┘└┘  └─┘┴ ┴└─┘└─┘┴ ┴ o\n");
    }
    else
    {
        printf("┌┐ ┬  ┌─┐┌─┐┬┌─  ┬┌─┬┌┐┌┌─┐  ┬┌┐┌  ┌─┐┬ ┬┌─┐┌─┐┬┌─ ┬\n");
        printf("├┴┐│  ├─┤│  ├┴┐  ├┴┐│││││ ┬  ││││  │  ├─┤├┤ │  ├┴┐ │\n");
        printf("└─┘┴─┘┴ ┴└─┘┴ ┴  ┴ ┴┴┘└┘└─┘  ┴┘└┘  └─┘┴ ┴└─┘└─┘┴ ┴ o\n");
    }
    printf("\x1b[0m");
}
void printInvalidMove()
{
    inactive_moves--;
    printf("\x1b[31m");
    printf("╦┌┐┌┬  ┬┌─┐┬  ┬┌┬┐  ┌┬┐┌─┐┬  ┬┌─┐ ┬\n");
    printf("║│││└┐┌┘├─┤│  │ ││  ││││ │└┐┌┘├┤  │\n");
    printf("╩┘└┘ └┘ ┴ ┴┴─┘┴─┴┘  ┴ ┴└─┘ └┘ └─┘ o\n");
    printf("\x1b[0m");
}
void printVictory()
{
    printf("\x1b[32m");
    if (player)
    {
        printf("╦ ╦┬ ┬┬┌┬┐┌─┐  ┬ ┬┬┌┐┌┌─┐\n");
        printf("║║║├─┤│ │ ├┤   │││││││└─┐\n");
        printf("╚╩╝┴ ┴┴ ┴ └─┘  └┴┘┴┘└┘└─┘o\n");
    }
    else
    {
        printf("╔╗ ┬  ┌─┐┌─┐┬┌─  ┬ ┬┬┌┐┌┌─┐\n");
        printf("╠╩╗│  ├─┤│  ├┴┐  │││││││└─┐\n");
        printf("╚═╝┴─┘┴ ┴└─┘┴ ┴  └┴┘┴┘└┘└─┘o\n");
    }
    printf("\x1b[0m");
    
}
void printCheckmate()
{
    printf("\x1b[32m");
    printf("╔═╗┬ ┬┌─┐┌─┐┬┌─┌┬┐┌─┐┌┬┐┌─┐  ┬  \n");
    printf("║  ├─┤├┤ │  ├┴┐│││├─┤ │ ├┤   │  \n");
    printf("╚═╝┴ ┴└─┘└─┘┴ ┴┴ ┴┴ ┴ ┴ └─┘  o  o\n");
    printf("\x1b[0m");
    
}
void printStalemate(){
    printf("\x1b[36m");
    printf("┌─┐┌┬┐┌─┐┬  ┌─┐┌┬┐┌─┐┌┬┐┌─┐  ┬\n");
    printf("└─┐ │ ├─┤│  ├┤ │││├─┤ │ ├┤   │\n");
    printf("└─┘ ┴ ┴ ┴┴─┘└─┘┴ ┴┴ ┴ ┴ └─┘  o\n");
    printf("\x1b[0m");
    
}
void printThanks(){
    printf("\x1b[32m");
    printf("╔╦╗┬ ┬┌─┐┌┐┌┬┌─┌─┐  ┌─┐┌─┐┬─┐  ┌─┐┬  ┌─┐┬ ┬┬┌┐┌┌─┐  ┬\n");
    printf(" ║ ├─┤├─┤│││├┴┐└─┐  ├┤ │ │├┬┘  ├─┘│  ├─┤└┬┘│││││ ┬  │\n");
    printf(" ╩ ┴ ┴┴ ┴┘└┘┴ ┴└─┘  └  └─┘┴└─  ┴  ┴─┘┴ ┴ ┴ ┴┘└┘└─┘  o\n");
    printf("\x1b[0m");
    
}
void print50(){
    printf("\x1b[36m");
    printf("╔═╗┬┌─┐┌┬┐┬ ┬  ┬┌┐┌┌─┐┌─┐┌┬┐┬┬  ┬┌─┐  ┌┬┐┌─┐┬  ┬┌─┐┌─┐  ┬\n");
    printf("╠╣ │├┤  │ └┬┘  ││││├─┤│   │ │└┐┌┘├┤   ││││ │└┐┌┘├┤ └─┐  │\n");
    printf("╚  ┴└   ┴  ┴   ┴┘└┘┴ ┴└─┘ ┴ ┴ └┘ └─┘  ┴ ┴└─┘ └┘ └─┘└─┘  o\n");
    printf("\x1b[0m");
    
}
void printDraw(){
    printf("\x1b[36m");
    printf("╔╦╗┌─┐┌┬┐┌─┐┬ ┬  ┌┬┐┬─┐┌─┐┬ ┬┌┐┌  ┬\n");
    printf("║║║├─┤ │ │  ├─┤   ││├┬┘├─┤││││││  │\n");
    printf("╩ ╩┴ ┴ ┴ └─┘┴ ┴  ─┴┘┴└─┴ ┴└┴┘┘└┘  o\n");
    printf("\x1b[0m");
    
}
void undo()
{
	printf("\nNow we turn back to the last step\n");

}
void printBoard()
{
    char col_name[8] = {'a','b','c','d','e','f','g','h'};
    printf("\x1b[30m\x1b[46m   ");//
    for (int i=0; i<8;i++)
        printf(" %c ",col_name[i]);
    printf("   \x1b[0m\n");
    for (int row = 0,p=8;row<8;row++,p--)
    {
        printf("\x1b[30m\x1b[46m");
        printf(" %d ",p);
        printf("\x1b[0m\x1b[30m");
        for (int col = 0;col<8;col++)
        {
            if(row%2==0){
                col%2==0? printf("\x1b[47m"):printf("\x1b[100m");
            }else {
                col%2==0? printf("\x1b[100m"):printf("\x1b[47m");
            }
            switch (board[row][col])
            {
                case 0:
                    printf("   ");
                    break;
                case -1:
                    printf(" ♜ ");
                    break;
                case -2:
                    printf(" ♞ ");
                    break;
                case -3:
                    printf(" ♝ ");
                    break;
                case -4:
                    printf(" ♛ ");
                    break;
                case -5:
                    printf(" ♚ ");
                    break;
                case -6:
                    printf(" ♟ ");
                    break;
                case 1:
                    printf(" ♖ ");
                    break;
                case 2:
                    printf(" ♘ ");
                    break;
                case 3:
                    printf(" ♗ ");
                    break;
                case 4:
                    printf(" ♕ ");
                    break;
                case 5:
                    printf(" ♔ ");
                    break;
                case 6:
                    printf(" ♙ ");
                    break;
                default:
                    break;
            }
        }
        printf("\x1b[30m\x1b[46m");
        printf(" %d ",p);
        printf("\x1b[0m\n");
    }
    printf("\x1b[30m\x1b[46m   ");
    for (int i=0; i<8;i++)
        printf(" %c ",col_name[i]);
    printf("   \x1b[0m\n");
}
int isValidMove(int Board[][8],int r1,int c1,int r2,int c2)
{
    int pawn = Board[r1][c1];
    if ((player && pawn>0) || ((player == 0 )&& pawn<0))
    {
        switch (pawn)
        {
            case 6:
                if (whiteSoldierMove(Board,r1,c1,r2,c2)) return 1;
                return 0;
                break;
            case -6:
                if (blackSoldierMove(Board,r1,c1,r2,c2)) return 1;
                return 0;
                break;
            case 5:
            case -5:
                if (kingMove(Board,r1,c1,r2,c2)) return 1;
                return 0;
                break;
            case 3:
            case -3:
                if (camelMove(Board,r1,c1,r2,c2))return 1;
                return 0;
                break;
            case 2:
            case -2:
                if (horseMove(Board,r1,c1,r2,c2)) return 1;
                return 0;
                break;
            case 1:
            case -1:
                if (elephantMove(Board,r1,c1,r2,c2)) return 1;
                return 0;
                break;
            case 4:
            case -4:
                if (elephantMove(Board,r1,c1,r2,c2) || camelMove(Board,r1,c1,r2,c2))
                    return 1;
                return 0;
                break;
        }
        //return 1;
    }
    return 0;
}
int makeMove(int Board[][8],int r1,int c1,int r2,int c2)
{
    int temp;
    temp = Board[r2][c2];
    Board[r2][c2] = Board[r1][c1];
    Board[r1][c1] = 0;
    if (!isKingSafe(Board))
    {
        Board[r1][c1] = Board[r2][c2];
        Board[r2][c2] = temp;
        return 0;
    }
    else{
        player = player?0:1;
        return 1;
    }
}
int whiteSoldierMove(int Board[][8],int r1,int c1,int r2,int c2)
{
    int res;
    if (c1 == c2 )
    {
        if ((r1 == 6) && (Board[r2][c2] == 0) && (Board[r1 - 1][c2] == 0) &&  (r2==4 || r2==5))
            res = 1;
        else if ((r2+1) == r1 && (Board[r2][c2] == 0))
            res = 1;
        else
            res = 0;
    }
    else if ((Board[r2][c2]<0) && (r2 == (r1 - 1)) && (c2 == (c1 - 1) ||  c2 == (c1+1)))
        res = 1;
    else
        res = 0;
    return res;
}
int blackSoldierMove(int Board[][8],int r1,int c1,int r2,int c2)
{
    int res;
    if (c1 == c2 )
    {
        if ((r1 == 1) && (board[r2][c2] == 0) && (Board[r1 + 1][c2] == 0) &&  (r2==3 || r2==2))
            res = 1;
        else if ((r2-1) == r1 && (Board[r2][c2] == 0))
            res = 1;
        else
            res = 0;
    }
    else if ((Board[r2][c2]>0) && (r2 == (r1 + 1)) && (c2 == (c1 - 1) ||  c2 == (c1+1)))
        res = 1;
    else
        res = 0;
    return res;
}
int horseMove(int Board[][8],int r1,int c1,int r2,int c2)
{
    int res;
    if (player && Board[r2][c2]>0) return 0;
    else if (!player && Board[r2][c2]<0) return 0;
    if (((r2 == (r1+2)) || (r2 == (r1-2))) && ((c2 == (c1+1)) || (c2 == (c1-1))))
        res = 1;
    else if (((c2 == (c1+2)) || (c2 == (c1-2))) && ((r2 == (r1+1)) || (r2 == (r1-1))))
        res = 1;
    else
        res = 0;
    return res;
}
int elephantMove(int Board[][8],int r1,int c1,int r2,int c2)
{
    int res = 1;
    if (player && Board[r2][c2]>0) return 0;
    else if (!player && Board[r2][c2]<0) return 0;
    if (r1 == r2)
    {
        if (c1>c2)
        {
            for(int i = (c1-1);i>c2;i--)
            {
                if (Board[r2][i] == 0) continue;
                res = 0;
                break;
            }
        }
        else if (c2>c1)
        {
            for(int i = (c1+1);i<c2;i++)
            {
                if (Board[r2][i] == 0) continue;
                res = 0;
                break;
            }
        }
        else
            return 0;
    }
    else if(c1 == c2)
    {
        if (r1>r2)
        {
            for(int i = (r1-1);i>r2;i--)
            {
                if (Board[i][c2] == 0) continue;
                res = 0;
                break;
            }
        }
        else if (r2>r1)
        {
            for(int i = (r1+1);i<r2;i++)
            {
                if (Board[i][c2] == 0) continue;
                res = 0;
                break;
            }
        }
        else
            return 0;
    }
    else
        return 0;
    return res;
}
int camelMove(int Board[][8],int r1,int c1,int r2,int c2)
{
    int res = 1;
    if (player)
    {
        if (Board[r2][c2]>0)
            return 0;
    }
    else
        if (Board[r2][c2]<0)
            return 0;
    if (r2>r1 && c2>c1)
    {
        if ((r2 - r1) == (c2 - c1))
        {
            for(int i = r1+1,j = c1+1;i<r2;i++,j++)
            {
                if (Board[i][j] == 0)
                    continue;
                res = 0;
                break;
            }
        }
        else
            res = 0;
    }
    else if(r2>r1 && c2<c1)
    {
        if ((r2 - r1) == (c1 - c2))
        {
            for(int i = r1+1,j = c1-1;i<r2;i++,j--)
            {
                if (Board[i][j] == 0)
                    continue;
                res = 0;
                break;
            }
        }
        else
            res = 0;
    }
    else if(r1>r2 && c2>c1)
    {
        if ((r1 - r2) == (c2 - c1))
        {
            for(int i = r1-1,j = c1+1;j<c2;i--,j++)
            {
                if (Board[i][j] == 0)
                    continue;
                res = 0;
                break;
            }
        }
        else
            res = 0;
    }
    else if(r2<r1 && c2<c1)
    {
        if ((r1 - r2) == (c1 - c2))
        {
            for(int i = r1-1,j = c1-1;i>r2;i--,j--)
            {
                if (Board[i][j] == 0)
                    continue;
                res = 0;
                break;
            }
        }
        else
            res = 0;
    }
    else
        res = 0;
    return res;
}
int kingMove(int Board[][8],int r1,int c1,int r2,int c2)
{
    int res = 0;
    if (r2 == r1 && !(c2 == c1 + 1 || c2 == c1 - 1))
        if (isCastle(Board,r1,c1,r2,c2))
        {
            castle(r1,c1,r2,c2);
            return 1;
        }
    if (player)
    {
        if (Board[r2][c2]>0)
            return 0;
    }
    else
        if (Board[r2][c2]<0)
            return 0;
    if ((r2 == r1 + 1 || r2 == r1 -1) &&  (c2 == c1 + 1 || c2 == c1 -1))
        res =  1;
    else if ((r2 == r1) && (c2 == c1 + 1 || c2 == c1 - 1) )
        res = 1;
    else if ((c2 == c1) && (r2 == r1 + 1 || r2 == r1 - 1) )
        res = 1;
    else
        res = 0;
    return res;
}
int isCastle(int Board[][8],int r1,int c1,int r2,int c2)
{
    if (!isKingSafe(Board))
        return 0;
    int res = 0;
    if (r1 == 0 && c1 == 4)
    {
        if (black_king_moved)
            return 0;
        if (c2 == 6 && !black_rook2_moved && (Board[0][7] == -1))
        {
            for (int i = c1+1;i<=6;i++)
            {
                if (Board[r1][i] == 0)
                {
                    res = 1;
                    continue;
                }
                return 0;
            }
        }
        else if (c2 == 2 && !black_rook1_moved && (Board[0][0] == -1))
        {
            for (int i = c1-1;i>=1;i--)
            {
                if (Board[r1][i] == 0)
                {
                    res = 1;
                    continue;
                }
                return 0;
            }
        }
    }
    else if (r1 == 7 && c1 == 4)
    {
        if (white_king_moved)
            return 0;
        if (c2 == 6 && !white_rook2_moved && (board[7][7] == 1))
        {
            for (int i = c1+1;i<=6;i++)
            {
                if (Board[r1][i] == 0)
                {
                    res = 1;
                    continue;
                }
                return 0;
            }
        }
        else if (c2 == 2 && !white_rook1_moved  && (board[7][0] == 1))
        {
            for (int i = c1-1;i>=1;i--)
            {
                if (Board[r1][i] == 0)
                {
                    res = 1;
                    continue;
                }
                return 0;
            }
        }
    }
    return res;
}
void castle(int r1,int c1,int r2,int c2)
{
    if (r1 == 0)
    {
        if (c2 == 6)
        {
            board[0][7] = 0;
            board[0][5] = -1;
        }
        if (c2 == 2)
        {
            board[0][0] = 0;
            board[0][3] = -1;
        }
    }
    else
    {
        if (c2 == 6)
        {
            board[7][7] = 0;
            board[7][5] = 1;
        }
        if (c2 == 2)
        {
            board[7][0] = 0;
            board[7][3] = 1;
        }
    }
}
void pawn_promotion(int r1, int c1)
{
    char ch;
    printf("Which pawn do you want to promote?\n ");
    if (player)
    {
        printf("Enter Q for Queen\n\tH for Horse\n\tR for Rook\n\tB for Bishop\n");
        fflush(stdin);
        scanf(" %c",&ch);
        switch (ch)
        {
            case 'Q':
                board[r1][c1] = 4;
                break;
            case 'H':
                board[r1][c1] = 2;
                break;
            case 'R':
                board[r1][c1] = 1;
                break;
            case 'B':
                board[r1][c1] = 3;
                break;
            default:
                printf("Invalid input!\nPlease enter a valid input\n");
                pawn_promotion(r1,c1);
        }
    }
    else
    {
        printf("Enter q for Queen\n\th for Horse\n\tr for Rook\n\tb for Bishop\n");
        fflush(stdin);
        scanf(" %c",&ch);
        switch (ch)
        {
            case 'q':
                board[r1][c1] = -4;
                break;
            case 'h':
                board[r1][c1] = -2;
                break;
            case 'r':
                board[r1][c1] = -1;
                break;
            case 'b':
                board[r1][c1] = -3;
                break;
            default:
                printf("Invalid input!\nPlease enter a valid input\n");
                pawn_promotion(r1,c1);
        }
    }
}
int isKingSafe(int Board[][8])
{
    int r = -1,c = -1;
    for(int i = 0;i<8;i++)
    {
        for(int j = 0;j<8;j++)
        {
            if (((Board[i][j] == 5) && player) || ((Board[i][j] == -5) & !player) )
            {
                r = i;
                c = j;
                i = 8;
                break;
            }
        }
    }
    if (player)
    {
        player = 0;
        for(int i = 0;i<8;i++)
        {
            for(int j = 0;j<8;j++)
            {
                if (Board[i][j]>=0)
                    continue;
                else if (isValidMove(Board,i,j,r,c))
                {
                    player = 1;
                    return 0;
                }
            }
        }
        player = 1;
    }
    else
    {
        player = 1;
        for(int i = 0;i<8;i++)
        {
            for(int j = 0;j<8;j++)
            {
                if (Board[i][j]<=0)
                    continue;
                else if (isValidMove(Board,i,j,r,c))
                {
                    player = 0;
                    return 0;
                }
            }
        }
        player = 0;
    }
    return 1;
}
int playerHasValidMove()
{
    number_of_pawns = 0;
    int i,j;
    for (i = 0;i<8;i++)
        for(j = 0;j<8;j++)
        {
            tempboard[i][j] = board[i][j];
            if (board[i][j] != 0)
                number_of_pawns++;
        }
    int r = -1,c = -1;
    for (r = 0;r<8;r++)
    {
        for(c = 0;c<8;c++)
        {
            if ((board[r][c] == 0) || (player && board[r][c]<0) || (!player && board[r][c]>0))
                continue;
            for (i = 0;i<8;i++)
                for(j = 0;j<8;j++)
                    if(isValidMove(board,r, c, i, j))
                    {
                        tempboard[i][j] = tempboard[r][c];
                        tempboard[r][c] = 0;
                        if(isKingSafe(tempboard))
                        {
                            tempboard[i][j] = board[i][j];
                            tempboard[r][c] = board[r][c];
                            return 1;
                        }
                        tempboard[i][j] = board[i][j];
                        tempboard[r][c] = board[r][c];
                    }
        }
    }
    if (!isKingSafe(board))
        checkmate = 1;
    else
        stalemate = 1;
    player = player?0:1;
    return 0;
}
void readfile()
{
                 store_file=fopen("Store_Data.txt","r");
                        fscanf(store_file,"%d\n",&player);
                        fscanf(store_file,"%d\n",&inactive_moves);
                        fscanf(store_file,"%d\n",&checkmate);
                        fscanf(store_file,"%d\n",&stalemate);
                        fscanf(store_file,"%d\n",&number_of_pawns);
                        fscanf(store_file,"%d\n",&number_of_pawns_temp);
                        fscanf(store_file,"%d\n",&black_rook1_moved);
                        fscanf(store_file,"%d\n",&black_rook2_moved);
                        fscanf(store_file,"%d\n",&white_rook1_moved);
                        fscanf(store_file,"%d\n",&white_rook2_moved);
                        fscanf(store_file,"%d\n",&white_king_moved);
                        fscanf(store_file,"%d\n",&black_king_moved);
                        for(row_counter=0;row_counter<=counter;row_counter++)
                        {
                                for(col_counter=0;col_counter<=counter;col_counter++)
                                {

                                        fscanf(store_file,"%d\t",&board[row_counter][col_counter]);

                                }
                                fscanf(store_file,"\n");
                        }
                        for(row_counter=0;row_counter<=counter;row_counter++)
                        {
                                for(col_counter=0;col_counter<=counter;col_counter++)
                                {

                                        fscanf(store_file,"%d\t",&tempboard[row_counter][col_counter]);

                                }
                               fscanf(store_file,"\n");
                        }




        fclose(store_file);

}
void savefile()
{
 fprintf(store_file,"%d\n",player);
                        fprintf(store_file,"%d\n",inactive_moves);
                        fprintf(store_file,"%d\n",checkmate);
                        fprintf(store_file,"%d\n",stalemate);
                        fprintf(store_file,"%d\n",number_of_pawns);
                        fprintf(store_file,"%d\n",number_of_pawns_temp);
                        fprintf(store_file,"%d\n",black_rook1_moved);
                        fprintf(store_file,"%d\n",black_rook2_moved);
                        fprintf(store_file,"%d\n",white_rook1_moved);
                        fprintf(store_file,"%d\n",white_rook2_moved);
                        fprintf(store_file,"%d\n",white_king_moved);
                        fprintf(store_file,"%d\n",black_king_moved);
                        for(row_counter=0;row_counter<=counter;row_counter++)
                        {
                                for(col_counter=0;col_counter<=counter;col_counter++)
                                {

                                        fprintf(store_file,"%d\t",board[row_counter][col_counter]);

                                }
                                fprintf(store_file,"\n");
                        }
                        for(row_counter=0;row_counter<=counter;row_counter++)
                        {
                                for(col_counter=0;col_counter<=counter;col_counter++)
                                {

                                        fprintf(store_file,"%d\t",tempboard[row_counter][col_counter]);
                                }
                                fprintf(store_file,"\n");
                        }


}
void replay()
{
file=fopen("Moves.txt","r");
while(!feof(file))
{
strcpy(sz,"\n");
fgets(sz,100,file);
printf("\n%s",sz);
}
}
void inputmove(EV_P_ ev_io *w , int revents)
{
  //printf("\n%s's turn,Enter your move - ",player?"White":"Black");
  scanf("%s",move);
  ev_io_stop(EV_A_ w);
  ev_break (EV_A_ EVBREAK_ALL);
  printf("\e[1;1H\e[2J\n\n");//clrscr

}
void timeout_cb(EV_P_ ev_timer *w, int revents)
{
puts("timeout");
ev_break(EV_A_ EVBREAK_ONE);

}
