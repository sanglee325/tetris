#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
    createRankList();

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
        case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

    writeRankFile();
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
    nextBlock[2]=rand()%7;

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();

    DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

    DrawBox(9, WIDTH+10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(10+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
    int i,j;
    for(i=0;i<4;i++)
      for(j=0;j<4;j++){
          if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
              move(i+y+1,j+x+1);
              if(tile == '.')
                printw("%c", tile);
              else{
                  attron(A_REVERSE);
                  printw("%c",tile);
                  attroff(A_REVERSE);
              }
          }
      }

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
    int i, j;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            if(block[currentBlock][blockRotate][i][j] == 1){
                if(blockY + i >= HEIGHT) return 0;
                if(blockX + j < 0) return 0;
                if(blockX + j >= WIDTH) return 0;
                if(f[blockY + i][blockX + j] == 1) return 0;
            }
        }
    }
    return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
    // user code
    int i, j;
    int rotate = blockRotate;
    int prevX = blockX, prevY = blockY;

    switch(command){
      case KEY_UP : rotate = (rotate + 3) % 4;
                    break;
      case KEY_LEFT : prevX += 1;
                      break;
      case KEY_RIGHT : prevX -= 1;
                       break;
      case KEY_DOWN : prevY -= 1;
                      break;
      default : break;
    }

    DrawBlock(prevY, prevX, currentBlock, rotate, '.');

    while(CheckToMove(field, currentBlock, rotate, prevY, prevX))
        prevY++;

    DrawBlock(prevY - 1, prevX, currentBlock, rotate, '.');
    DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
}

void BlockDown(int sig){
    // user code
    if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)){
        blockY++;
        DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
    }//아래로 내려가는 것 가능

    else{
        if(blockY == -1){
            gameOver = 1;
            return;
        }
        score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
        score += DeleteLine(field);
        
        nextBlock[0] = nextBlock[1];
        nextBlock[1] = nextBlock[2];
        nextBlock[2] = rand() % 7;
        
        blockY = -1;
        blockX = (WIDTH)/2 - 2;
        blockRotate = 0;
        DrawField();
        DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
        DrawNextBlock(nextBlock);
    }
    PrintScore(score);
    timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
    // user code
    int i, j, touched = 0;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            if(block[currentBlock][blockRotate][i][j] == 1){
                f[i + blockY][j + blockX] = 1;
                DrawField();
                if(i + blockY + 1 == HEIGHT || f[i + blockY + 1][j + blockX] == 1) touched++;
            }
        }
    }
    return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
    int i, j, k;
    int count = 0;

    for(i = 0; i < HEIGHT; i++){
        for(j = 0; j < WIDTH; j++)
            if(f[i][j] == 0) break;//빈칸이 있다면 삭제 X

        if(j == WIDTH){
            count++;
            for(k = i; k > 0; k--){
                for(j = 0; j < WIDTH; j++)
                  f[k][j] = f[k - 1][j];//줄이 삭제된다면 아래로 한줄씩 내림
            }
        }
    }

    return count * count * 100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
    int ShadowY = y;

    while(CheckToMove(field, blockID, blockRotate, ShadowY + 1, x))
        ShadowY++;
     
    DrawBlock(ShadowY, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
    DrawShadow(y, x, blockID, blockRotate);
    DrawBlock(y, x, blockID, blockRotate, ' ');
}

void createRankList(){
	// user code
    FILE *fp = fopen("rank.txt", "r");
    int i, score;
    char name[NAMELEN];
    Node *temp;
    
    Root = NULL;
    fscanf(fp, "%d", &NumData);

    for(i = 0; i < NumData; i++){
        fscanf(fp, "%s %d", name, &score);
        if(Root == NULL)
          Root = createNode(name, score);
        else
          insertNode(Root, createNode(name, score));
    }

    fclose(fp);
}

Node* createNode(char* name, int score){
    Node* new = malloc(sizeof(Node));

    new->leftChild = new->rightChild = NULL;
    new->score = score;
    strcpy(new->name, name);

    return new;
}

void insertNode(Node *root, Node *child){
    if(root->score > child->score){
        if(root->rightChild == NULL)
          root->rightChild = child;
        else
          insertNode(root->rightChild, child);
    }
    else if(root->score < child->score){
        if(root->leftChild == NULL)
          root->leftChild = child;
        else
          insertNode(root->leftChild, child);
    }

}

Node* deleteNode(Node* root, Node *parent){

    NumData--;

}

void printInorder(Node* ptr, FILE *fp){
    if(ptr != NULL){
        printInorder(ptr->leftChild, fp);
        fprintf(fp, "%s %d\n", ptr->name, ptr->score);
        printInorder(ptr->rightChild, fp);
    }
    else return;
}

void rankInorder(Node* ptr, int x, int y){
    if(ptr != NULL){
        rankInorder(ptr->leftChild, x, y);
        listRankFlag++;
        if(listRankFlag >= x && listRankFlag <= y)
          printw("%-16s | %d\n", ptr->name, ptr->score);
        rankInorder(ptr->rightChild, x, y);
    }
    else return;
}

void rank(){
    int x = -1, y = -1;
    char name[NAMELEN];
    int rank;
    listRankFlag = 0;

    // user code
    clear();
    printw("1. list ranks from X to Y\n");
    printw("2. list ranks by a specific name\n");
    printw("3. delete a specific rank\n");

    switch(wgetch(stdscr)){
      case '1' :
        echo();
        printw("X: ");
        scanw("%d", &x);
        if(x == -1) x = 1;
        printw("Y: ");
        scanw("%d", &y);
        if(y == -1) y = NumData;
        noecho();
        printw("\n      name     |   score   \n");
        printw("------------------------------\n");
        if(x > y || x > NumData || y > NumData){
            printw("search failure: no rank in list\n");
            wgetch(stdscr);
            break;
        }
        else{
            rankInorder(Root, x, y);
        }
        wgetch(stdscr);
      case '2' :
        break;
      case '3' :
        break;
      default : break;
    }

}

void writeRankFile(){
    // user code
    FILE *fp = fopen("rank.txt", "w");

    fprintf(fp, "%d\n", NumData);
    printInorder(Root, fp);

    fclose(fp);
}

void newRank(int score){
	// user code
    Node* new;
    char name[NAMELEN];

    clear();
    printw("your name: ");
    echo();
    scanw("%s", name);
    noecho();

    new = createNode(name, score);
    insertNode(Root, new);

    NumData++;

}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
