#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <time.h>
#include <ctype.h>
//Turbo C++ allows cin and cout from fstream
#include <fstream.h>
#include <iomanip.h>
#define ENDL 13
#define IO_ERROR 1
#define PARSE_X (isdigit(p[pid].x) ? p[pid].x - '0' : p[pid].x - 'A' + 10)
#define PARSE_Y (isdigit(p[pid].y) ? p[pid].y - '0' : p[pid].y - 'A' + 10)

const CS = 20; //Represents Cell Size
const int NC = 6; //Number of columns,  must be const.
const char* SCORE_FILE = "CR_SCORE.bin"; //File to put scores in
int totp = 2; //Default total number of players

struct player { //Player's definition
    char x;
    char y;
    int color;
    void setcolor(int c) { color = c; }
    player() { x = '0', y = '0'; }
};
player* p = new player[totp]; //All players
int pid = 0; //Id of current player

char* mkstr(char c, char* s) //Makes string from a character, i.e. converts 'a' to {'a',0}
{
    s[0] = c;
    s[1] = 0;
    return s;
}
class bufpos { //Position of current buffer for printing purposes.
    static int px;

public:
    static int x()
    {
        px += 10;
        return px - 10;
    }
    static void x(int a)
    {
        px = a;
    }
};

char moreno[][2] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" }; //Letters to use after digit 9 as row/column number

int valid(char c) //Checks if the move is valid or not. Returns non-zero on invalid move zero otherwise.
{
    int i;
    for (i = 0; i < sizeof(moreno) / (2 * sizeof(char)); i++)
        if (moreno[i][0] == c)
            return 1;
    for (i = 0; i < 10; i++)
        if (c - '0' == i)
            return 1;
    return 0;
}

void drawlines() //Draws the lines for the grid
{ //Global members pid and p will be accessed here (No alterations will be made to them)
    int i, j;
    char tmp[20]; //Temporary storage
    //lines
    setcolor(p[pid].color);
    for (i = 0; i <= NC * CS; i += CS) {
        if (i != NC * CS)
            outtextxy(5, i + CS, i / CS < 10 ? itoa(i / CS, tmp, 10) : moreno[i / CS - 10]);
        line(0, i + CS, NC * CS + CS, i + 20);
    }
    for (i = 0; i <= NC * CS; i += CS) {
        if (i != NC * CS)
            outtextxy(i + 20, 5, i / CS < 10 ? itoa(i / 20, tmp, 10) : moreno[i / CS - 10]);
        line(i + CS, 0, i + CS, NC * CS + CS);
    }
}

void drawBubbles(const int grid[NC][NC][2]) //Adds bubbles for player move
{
    clrscr();
    drawlines();
    int i, j;
    char tmp[5];

    for (i = 0; i < NC * CS; i += CS)
        for (j = 0; j < NC * CS; j += CS)
            if (grid[i / CS][j / CS][0] != -1) {
                setcolor(p[grid[i / CS][j / CS][0]].color);
                for (int k = 0; k < (grid[i / CS][j / CS][1]) * 5; k += 5)
                    outtextxy(i + k + 20, j + 20, "O" ); //The last argument is the one used as the bubble
            }
}

void render(const int grid[NC][NC][2]) //Renders lines and bubbles
{
    drawlines();

    drawBubbles(grid);
}
void react(int grid[NC][NC][2], int reaction = 1) //Mechanism for the reaction (Recursive)
{
    int i, j, k;

    if (!reaction)
        return;
    reaction = 0;
    for (i = 0; i < CS * NC; i += CS) {
        for (j = 0; j < CS * NC; j += CS) {
            //fetching exploding no of shells
            int ens = 0;
            if (i / CS + 1 < NC)
                ens++;
            if (i / CS - 1 >= 0)
                ens++;
            if (j / CS + 1 < NC)
                ens++;
            if (j / CS - 1 >= 0)
                ens++;
   
            if (grid[i / CS][j / CS][1] >= ens) {
 		reaction = 1;
                int pe = grid[i / CS][j / CS][0]; // player, exploder
                setcolor(p[pe].color);
                //clear the current postion
                outtextxy(i, j, "  ");
                grid[i / CS][j / CS][1] = 0;
                grid[i / CS][j / CS][0] = -1; //unoccupied space now.
                //spread shells to boundaries
                if (i / CS + 1 < NC)
                    grid[i / CS + 1][j / CS][0] = pe, grid[i / CS + 1][j / CS][1]++;
                if (i / CS - 1 >= 0)
                    grid[i / CS - 1][j / CS][0] = pe, grid[i / CS - 1][j / CS][1]++;
                if (j / CS + 1 < NC)
                    grid[i / CS][j / CS + 1][0] = pe, grid[i / CS][j / CS + 1][1]++;
                if (j / CS - 1 >= 0)
                    grid[i / CS][j / CS - 1][0] = pe, grid[i / CS][j / CS - 1][1]++;
                render(grid);
                delay(500);
                react(grid); //Next reaction
            }
        }
    }
}

void prompt(char* out, char* in) //Prompt dialog
{
    clrscr();
    int bkc = getbkcolor();
    setbkcolor(DARKGRAY);
    setfillstyle(0, 0);
    fillellipse(getmaxx() / 2, getmaxy() / 3 + 5, 300, 50);
    setcolor(WHITE);
    outtextxy(getmaxx() / 2 - 100, getmaxy() / 3 + 5, out);
    gotoxy(46, 11);
    gets(in);
    setbkcolor(bkc);
}

void alert(char* out, int notWarning = 0){ //Alert dialog (Green and red)
    int bkc = getbkcolor();
    int len = strlen(out);
    setbkcolor(DARKGRAY);
    if (notWarning)
	setfillstyle(1, GREEN);
    else
	setfillstyle(1, RED);
    bar(getmaxx() / 4, getmaxy() / 4, getmaxx() / 4 * 3, getmaxy() / 4 + 20);
    setcolor(BLACK);
    outtextxy(getmaxx() / 2 - (10 * len % 200), getmaxy() / 3 + 5, out);
    setbkcolor(bkc);
    setfillstyle(0, BLACK);
    getch();
}

void intro(void){ //Introduction dialog
    void showScores(void); //Declaration of showScores
    int bkc = getbkcolor();
    setbkcolor(DARKGRAY);
    bar3d(getmaxx() / 4, getmaxy() / 4, getmaxx() / 4 * 3, getmaxy() / 4 * 3, 10, 1);
    setcolor(BLACK);
    setfillstyle(1, WHITE);
    outtextxy(getmaxx() / 2 - 50, getmaxy() / 3 + 5, "CHAIN REACTION");
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 3 + 55, "Developed By:");
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 3 + 65, "Hritik Vijay");
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 3 + 75, "XII/B (18)");
    setbkcolor(bkc);
    setfillstyle(0, BLACK);
    char c = getch();
    switch (c) { case 's': case 'S': showScores(); break; default: return; }
}
   
struct user_group { 
    char name[100]; 
    time_t startT, endT, durationT; 
    user_group(void) {} //For temporary ugs (user groups) 
    user_group(char* n) {
        startT = time(NULL);
        strcpy(name, n);
    }
    void end(void)
    {
        endT = time(NULL);
        durationT = endT - startT;
    }
};

class score { //Handles and writes scores to scoreFile
    ofstream sc;

public:
    score(const char* scoreFile)
    {
        sc.open(scoreFile, ios::binary | ios::app);
        if (!sc) {
            alert("[E] Cannot open score file");
            exit(IO_ERROR);
        }
    }
    void write(user_group* ug)
    {
        if (!sc.write((char*)ug, sizeof(*ug))) {
            alert("[E] Cannot write to score file");
            exit(IO_ERROR);
        }
        sc.close(); //Score will be written only once
    }
};

char* strip(char* str) //Strips date/time provided by DOS to the required date/time
{ 
    str[19] = '\0';
    str = str + 4;
    return str;
}

void showScores(void) //Shows all scores
{
    setbkcolor(BLACK);
    clrscr();
    setcolor(WHITE);
    ifstream in(SCORE_FILE, ios::binary);
    user_group tmpu;
    cout << setw(18) << setfill(' ') << "GROUP NAME";
    cout << setw(18) << setfill(' ') << "START";
    cout << setw(18) << setfill(' ') << "END";
    cout << setw(18) << setfill(' ') << "DURATION" << endl;
    while (in.read((char*)&tmpu, sizeof(tmpu))) {
        cout << setw(18) << setfill(' ') << tmpu.name;
        cout << setw(18) << setfill(' ') << strip(ctime(&tmpu.startT));
        cout << setw(18) << setfill(' ') << strip(ctime(&tmpu.endT));
        cout << setw(18) << setfill(' ') << tmpu.durationT << 's' << endl;
    }
    getch();
}

int bufpos::px = 0;
int main()
{
    int gdriver = DETECT, gmode, errorcode;
    int xmax, ymax;
    int i; //Turbo C++ doesn't scope variables of for loop initialisation block to for block. A global counter is required.
    int isFirstRoundOver = 0;
    int lastPid = -1; //Pid of last user, will be used to check if the game is over (i.e. first user == last user)
    p[0].setcolor(GREEN); //Default colors [Fixed] for 2 players.
    p[1].setcolor(MAGENTA);

    initgraph(&gdriver, &gmode, "\\TC\\BGI\\");
    errorcode = graphresult();
    if (errorcode != grOk) {
        printf("Graphics error: %s\n", grapherrormsg(errorcode));
        printf("Press any key to halt:");
        getch();
        exit(1);
    }
    char tmp[5]; //Temporary storage
    //Grid array contains all info about the grid
    int grid[NC][NC][2]; // row->column->pid
    //		  			 ->no of bubbles
    for (i = 0; i < NC; i++) //Initialise the grid
        for (int j = 0; j < NC; j++) { 
            grid[i][j][0] = -1; //None has occupied
            grid[i][j][1] = 0; //No bubble is present
        }

    intro();
    char inputs[100];
    prompt("Enter Group Name: ", inputs);
    user_group ug(inputs);
    do {
        prompt("No of Players:    ", inputs);
        totp = atoi(inputs);
    } while (totp <= 1 || totp >= NC*NC);

    score sc(SCORE_FILE);
    for (;;) { //Game loop
        clrscr();

        if (pid >= totp) { //all users have moved, cycle turns
            pid = 0;
        }

        drawlines();
        react(grid); //Initiate the reaction
        drawBubbles(grid);
        //Killing block
        int isDead = 1;
        for (i = 0; i < NC; i++)
            for (j = 0; j < NC; j++)
                if (grid[i][j][0] == pid && grid[i][j][1] > 0)
                    isDead = 0;
        if (isDead && isFirstRoundOver) {
            pid++;
            continue; //Player is already dead
        }

        if (lastPid == pid) { //Game over!
            setfillstyle(XHATCH_FILL, p[pid].color);
            setcolor(WHITE);
            bar(240, 230, 400, 300);
            outtextxy(250, 250, "GAME OVER!");
            outtextxy(250, 270, "Winner: Player ");
            outtextxy(365, 270, itoa(pid, tmp, 10));
            getch();
            break;
        }

        //footer
        setcolor(WHITE);
        char c;
        outtextxy(10, 430, "Enter position: r,c:  ");
        bufpos::x(170);

        p[pid].y = getch();
        if (p[pid].y == 'q')
            break;
        outtextxy(bufpos::x(), 430, mkstr(p[pid].y, tmp));
        p[pid].x = getch();
        if (p[pid].x == 'q')
            break;
        outtextxy(bufpos::x(), 430, mkstr(p[pid].x, tmp));

        //validate user input for co-ordiates

        if (!(valid(p[pid].x) && valid(p[pid].y))) {
            setcolor(RED);
            outtextxy(bufpos::x(), 450, "Invalid Input");
            getch();
            setcolor(p[pid].color);
            continue;
        }
        if (PARSE_X >= NC || PARSE_Y >= NC) { //Checking limits
            setcolor(RED);
            outtextxy(bufpos::x(), 450, "Out of limits");
            getch();
            continue;
        }

        int penalty = 0; //identifier to be checked if the user stumbles upon an already occupied area
        for (i = 0; i < NC; i++)
            for (j = 0; j < NC; j++)
                if (grid[i][j][0] != -1 && PARSE_X == i && PARSE_Y == j && grid[i][j][0] != pid) {
                    setcolor(RED);
                    outtextxy(bufpos::x(), 450, "That's already occupied");
                    penalty = 1;
                    getch();
                }
        if (penalty)
            continue;

        //move
        grid[PARSE_X][PARSE_Y][0] = pid;
        grid[PARSE_X][PARSE_Y][1]++;

        if (getch() == 'q') //Quit command
            break;

        if (pid == totp - 1) {
            isFirstRoundOver = 1;
        }
        lastPid = pid;
        pid++;
    }

    /*Log current game*/
    ug.end();
    sc.write(&ug);

    /*Show scores*/
    getch();
    showScores();
    /* clean up */
    delete[] p;
    closegraph();
    return 0;
}
