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
          case MENU_REC : recommendedPlay(); break;
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

	for(i = 0; i < HEIGHT; i++)
		for(j = 0; j < WIDTH; j++)
			field[i][j] = 0;

    for(i = 0; i < VISIBLE_BLOCKS; i++)
	    nextBlock[i]=rand()%7;

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
		if((RCflag == true &&(command=='q'||command=='Q'))||(RCflag == 0 && ProcessCommand(command)==QUIT)){
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
	if(RCflag == false) newRank(score);
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
//---------
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

	for(i = 0; i < BLOCK_HEIGHT; i++){
		for(j = 0; j < BLOCK_WIDTH; j++){
			if(block[currentBlock][rotate][i][j] == 1 && prevY + i >= 0){
				move(i + prevY + 1, j + prevX + 1);
				printw(".");
			}
		}
	}
	while(1){
		if(CheckToMove(field, currentBlock, rotate, prevY+1, prevX)){
			prevY++;
		}
		else break;
	}
	for(i = 0; i < BLOCK_HEIGHT; i++){
		for(j = 0; j < BLOCK_WIDTH; j++){
			if(block[currentBlock][rotate][i][j] == 1 && prevY + i >= 0){
				move(i + prevY + 1, j + prevX + 1);
				printw(".");
			}
		}
    }
    DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	move(HEIGHT, WIDTH+10);
}

void BlockDown(int sig){
    // user code
    int i, j;

    if(RCflag == true){
        blockX = recommendX; blockY = recommendY; blockRotate = recommendR;
	}
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

        /*
		DelRecNode(recRoot);

		recRoot = (RecNode*)malloc(sizeof(RecNode));
		recRoot->lv = recRoot->score = 0;
		recRoot->x = blockX;
		recRoot->y = blockY;
		recRoot->r = blockRotate;

		for(i = 0; i < HEIGHT; i++)
			for(j = 0; j < WIDTH; j++)
				recRoot->f[i][j] = field[i][j];
		for(i = 0; i < CHILDREN_MAX; i++)
			recRoot->c[i] = NULL;
        */ //실습코드

        for(i = 0; i < VISIBLE_BLOCKS - 1; i++)
            nextBlock[i] = nextBlock[i + 1];
        nextBlock[VISIBLE_BLOCKS - 1] = rand() % 7;

        blockY = -1;
        blockX = (WIDTH)/2 - 2;
        blockRotate = 0;
        DrawField();

  /*     // modRoot = (MdfRec*)malloc(sizeof(MdfRec));
        modRoot->lv = 0;
        modRoot->score = score;

        for(i = 0; i < HEIGHT; i++)
            for(j = 0; j < WIDTH; j++)
                modRoot->f[i][j] = field[i][j];
        modifiedRecommend(modRoot);
       // free(modRoot);
*/
        DrawNextBlock(nextBlock);
        DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
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
                //DrawField();
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
    DrawBlock(y, x, blockID, blockRotate, ' ');
    if(RCflag ==false) DrawShadow(y, x, blockID, blockRotate);
    DrawRecommend(y, x, blockID, blockRotate);
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

Node *getNode(Node* ptr, Node* pre, int rank){
    if(ptr != NULL){
        getNode(ptr->leftChild, ptr, rank);
        rank--;
        if(rank == 0){
            if(ptr == Root) return Root;
            else {
                targetParent = pre;
                return ptr;
            }
        }
        getNode(ptr->rightChild, ptr, rank);
    }
}

void deleteNode(Node* parent, Node* target){
    Node *smallest, *temp;

    if(target == Root)
        parent = head;

    if(target->leftChild == NULL && target->rightChild == NULL){
        if(parent->leftChild == target) parent->leftChild = NULL;
        else if(parent->rightChild == target) parent->rightChild = NULL;
        free(target);
    }
    else if(target->leftChild == NULL){
        if(parent->leftChild == target) parent->leftChild = target->rightChild;
        else if(parent->rightChild == target) parent->rightChild = target->rightChild;
        free(target);
    }
    else if(target->rightChild == NULL){
        if(parent->leftChild == target) parent->leftChild = target->leftChild;
        else if(parent->rightChild == target) parent->rightChild = target->leftChild;
        free(target);
    }
    else{
        if(parent->leftChild == target) parent->leftChild = target->leftChild;
        else if(parent->rightChild == target) parent->rightChild = target->leftChild;
        smallest = parent->rightChild;
        if(smallest == NULL)  
            parent->rightChild = target->rightChild;
        else{  
            while(smallest != NULL){
                if(smallest->rightChild == NULL) break;
                smallest = smallest->rightChild;
            }
            smallest->rightChild = target->rightChild;
        }
        free(target);
    }
    Root = head->leftChild;
    NumData--;
}

void printInorder(Node* ptr, FILE *fp){
    if(ptr != NULL){
        printInorder(ptr->leftChild, fp);
        fprintf(fp, "%s %d\n", ptr->name, ptr->score);
        printInorder(ptr->rightChild, fp);
        free(ptr);
    }
    else return;
}

void searchInorder(Node* ptr, char *name, int *flag){
    if(ptr != NULL){
        searchInorder(ptr->leftChild, name, flag);
        if(strcmp(ptr->name, name) == 0){
            *flag = 1;
            printw("%-16s | %d\n", ptr->name, ptr->score);
        }
        searchInorder(ptr->rightChild, name, flag);
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
    int rank, deleteRank = 0, searchFlag = 0;
    Node* delNode = NULL, *parent = NULL;
    listRankFlag = 0;
    head = malloc(sizeof(Node));

    head->leftChild = Root;
    head->rightChild = NULL;
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
        printw("\n      name       |   score   \n");
        printw("------------------------------\n");
        if(x > y || x > NumData || y > NumData || x < 1 || y < 1){
            printw("\nsearch failure: no rank in list\n");
            wgetch(stdscr);
            break;
        }
        else{
            rankInorder(Root, x, y);
        }
        wgetch(stdscr);
        break;
      case '2' :
        echo();
        printw("Input the name: ");
        scanw("%s", name);
        noecho();
        printw("\n      name       |   score   \n");
        printw("------------------------------\n");

        searchInorder(Root, name, &searchFlag);
        if(!searchFlag)
          printw("\nsearch failure: the rank is not in the list\n");

        wgetch(stdscr);
        break;
      case '3' :
        echo();
        printw("Input the rank: ");
        scanw("%d", &deleteRank);
        noecho();
        printw("\n      name       |   score   \n");
        printw("------------------------------\n");

        if(deleteRank > NumData || deleteRank <= 0)
            printw("\nsearch failure: the rank is not in the list\n");
        else{
          targetParent = NULL;
          delNode = getNode(Root, Root, deleteRank);
          printw("%-16s | %d\n", delNode->name, delNode->score);
          deleteNode(targetParent, delNode);
          printw("\nresult: the rank deleted\n");
        }
        wgetch(stdscr);
        break;
      default : break;
    }

    free(head);

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
    modifiedRecommend(modRoot);
    DrawBlock(recommendY, recommendX, nextBlock[0], recommendR, 'R');
}

int recommend(RecNode *root){
	// user code
	int max = 0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int bR, bY, bX, temp_score, idx=0;
	int i, j, r, x, y, lv, tmp;

	RecNode *NewNode;
	lv = root->lv + 1;
	if(root->c[0] != NULL){
		for(i = 0; i < CHILDREN_MAX; i++){
			if(root->c[i] == NULL)
				break;
			temp_score = recommend(root->c[i]);
			if(max < temp_score){
				max = temp_score;
				bR = root->c[i]->r;
				bX = root->c[i]->x;
				bY = root->c[i]->y;
			}
		}
	}
	else{
		for(r = 0; r < 4; r++){
			if(r == 1 && nextBlock[lv - 1] == 4)
				break;
			else if(r == 2){
				tmp = nextBlock[lv - 1];
				if(tmp == 0 || tmp == 5 || tmp == 6)
					break;
			}
			for(x = -2; x < WIDTH; x++){
				y = -1;
				while(CheckToMove(root->f, nextBlock[lv - 1], r, y, x))
					y++;
				if(y == -1)
					continue;

				y--;
				NewNode = (RecNode*)malloc(sizeof(RecNode));
				NewNode->lv = lv;
				NewNode->score = root->score;
				for(i = 0; i < HEIGHT; i++)
					for(j = 0; j < WIDTH; j++)
						NewNode->f[i][j] = root->f[i][j];
				for(i = 0; i < CHILDREN_MAX; i++)
					NewNode->c[i] = NULL;
				NewNode->score += AddBlockToField(NewNode->f, nextBlock[lv-1], r, y, x);
				NewNode->score += DeleteLine(NewNode->f);
				NewNode->x = x;
				NewNode->y = y;
				NewNode->r = r;
				root->c[idx++] = NewNode;
				if(NewNode->lv < VISIBLE_BLOCKS){
					temp_score = recommend(NewNode);
				}
				else
					temp_score = NewNode->score;
				if(max < temp_score){
					max = temp_score;
					bR = r;
					bX = x;
					bY = y;
				}
			}
		}
	}
	if(root->lv == 0){
		recommendR = bR;
		recommendY = bY;
		recommendX = bX;
	}
	return max;
}

void rootInit(RecNode* root){
    int i;
    root->lv = 0;
    root->score = 0;
    for(i = 0; i < CHILDREN_MAX; i++)
        root->c[i] = NULL;
}

void DelRecNode(RecNode *del){
	int lv = del->lv;
	int i;
	if(lv == VISIBLE_BLOCKS)
		free(del);
	else{
		for(i = 0; i < CHILDREN_MAX; i++){
			if(del->c[i] == NULL)
				break;
			DelRecNode(del->c[i]);
		}
		free(del);
	}
}
/*
void InitRecommend(){
    int i, j;

    for(i = 0; i < VISIBLE_BLOCKS; i++)
	    nextBlock[i] = rand()%7;


	for(i = 0; i < HEIGHT; i++)
		for(j = 0; j < WIDTH; j++)
            modRoot->f[i][j] = field[i][j] = 0;

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();

    modifiedRecommend(modRoot);
    free(modRoot);
    
    DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
    DrawNextBlock(nextBlock);
    PrintScore(score);

    
    //field 초기화, nextBlock 생성 등등 기본 밑작업
    //1번 항목으로 들어갈때는 이 함수가 필요없도록 설계한다


}*/

void recommendedPlay(){
	// user code
    RCflag = true;
    play();
    RCflag = false;
}

int modifiedRecommend(MdfRec* root){
    int max = 0;
    int id, childNum = 0, rotateNum = 0, rotate[4];
    int *childScore, tmpScore;
    bool *search, Nflag = false;
    int i, j, idx, k;
    int posX, posY;

    if(root == NULL){
        root = (MdfRec*)malloc(sizeof(MdfRec));
        root->lv = 0;
        root->score = 0;
        for(i = 0; i < HEIGHT; i++)
          for(j = 0; j < WIDTH; j++)
            root->f[i][j] = field[i][j];
        Nflag = true;
    }

	id = root->lv;
	switch(nextBlock[id]){
		case 4 : childNum = 9; rotateNum = 1; rotate[0] = 9; break;
		case 0 : childNum = 17; rotateNum = 2; rotate[0] = 7; rotate[1] = 10; break;
		case 5 :
		case 6 : childNum = 17; rotateNum = 2; rotate[0] = 8; rotate[1] = 9;break;
		case 1 :
		case 2 :
		case 3 : childNum = 34; rotateNum = 4; rotate[0] = rotate[2] = 8; rotate[1] = rotate[3] = 9; break;
	}
    
    root->child = (MdfRec**)malloc(sizeof(MdfRec*) * childNum);
    childScore = (int*)malloc(sizeof(int) * childNum);
    search = (bool*)malloc(sizeof(bool) * childNum);

    for(k = 0; k < childNum; k++){
        root->child[k] = (MdfRec*)malloc(sizeof(MdfRec));
        for(i = 0; i < HEIGHT; i++)
            for(j = 0; j < WIDTH; j++)
                root->child[k]->f[i][j] = root->f[i][j];
    }
    //field 복사

    idx = 0;
	for(i = 0; i < rotateNum; i++){
		posX = -1;
		for(j = 0; j < rotate[i]; j++, posX++, idx++){
			posY = -1;
			if(!CheckToMove(root->f, nextBlock[id], i, -1, posX)) continue;
			root->child[idx]->lv = root->lv + 1;
			while(CheckToMove(root->child[idx]->f, nextBlock[id], i, posY + 1, posX)) posY++;
			root->child[idx]->score = root->score + AddBlockToField(root->child[idx]->f, nextBlock[id], i, posY, posX);
			root->child[idx]->score += DeleteLine(root->child[idx]->f);
            childScore[idx] = root->child[idx]->score;//ddddd
			if(root->child[idx]->lv < VISIBLE_BLOCKS - 1) tmpScore = modifiedRecommend(root->child[idx]);
			else tmpScore = root->child[idx]->score;
			if(tmpScore >= max){
				max = tmpScore;
				if(id == 0){
					recommendR = i; recommendY = posY; recommendX = posX;
				}
			}
		}
	}
    /*
    mid(childScore, search, childNum);

    idx = 0;
    for(i = 0; i < rotateNum; i++){
        posX = -2;
        for(j = 0; j < rotate[i]; j++, posX++, idx++){
            posY = -1;
			if(!CheckToMove(root->f, nextBlock[id], i, -1, posX)) continue;
            while(CheckToMove(root->child[idx]->f, nextBlock[id], i, posY + 1, posX)) posY++;
            if(search[idx] == true){
                if(root->child[idx]->lv < VISIBLE_BLOCKS - 1) tmpScore = modifiedRecommend(root->child[idx]);
                else tmpScore = root->child[idx]->score;
                if(tmpScore >= max){
                    max = tmpScore;
                    if(id == 0)
                      recommendR = i; recommendX = posX; recommendY = posY;
                }
            }
        }
    }
    */
    free(childScore);
    free(search);
    /*
    for(k = 0; k < childNum; k++)
      free(root->child[k]);
    free(root->child);//???
    if(Nflag == true) free(root);
    */
    return max;
    
}

void mid(int *cScore, bool *s, int num){
    int *tmp = malloc(sizeof(int) * num);
    int i, j, swap, mid;
    for(i = 0; i < num; i++)
      tmp[i] = cScore[i];

    for(i = 0; i < num - 1; i++){
        for(j = num - 1 ; j > i; j--){
            if(tmp[j - 1] <= tmp[j]) continue;
            swap = tmp[j - 1];
            tmp[j - 1] = tmp[j];
            tmp[j] = swap;
        }
    }
    
    mid = tmp[(num - 1)/2];
    
    for(i = 0; i < num; i++){
        if(cScore[i] < mid) s[i] = false;
        else s[i] = true;
    }

    free(tmp);
}
