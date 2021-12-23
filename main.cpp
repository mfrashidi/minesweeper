#include <iostream>
#include "clui.h"
#include <stdlib.h>
#include <time.h>
using namespace std;

//Default Variables
int SIZE;
int MINES;
bool SHOW_NUMBERS = true;
const int DEFAULT_SPACE = 4;
int MINES_LOCATION[399][2];
char table[20][20];
int WIDTH_SPACE;
int HEIGHT_SPACE;
int NUMBER_OF_FLAGS;
int SELECTED_ITEM[2];
bool IS_LOST;
bool IS_WON;
bool MESSAGE_SHOWN = false;
bool REPEAT_GAME = true;
bool IS_TIMING = false;
int SCORE;
int MENU_POSITION = 0;
int TIME_LENGTH = 0;
int START_TIME;
int row,cols,input;
bool LOST_BY_TIME = false;



//Characters
const int DEFAULT_CHAR = '.';
const char MINE_CHAR = 'X';
const char FLAG_CHAR = 'F';

//Colors
const int SELECTED_COLOR = 15;
const int ALPHABET_COLOR = 12;
const int EMPTYs_COLOR = 3;
const int NUMBERS_COLOR = 6;
const int MINEs_COLOR = 2;
const int FLAG_COLOR = 6;
const int GOOD_NEWS_COLOR = 3;
const int BAD_NEWS_COLOR = 2;
const int ERRORS_COLOR = 6;
const int MENU_COLOR = 12;

void initial(); //Initialize the basics to start the game
void init_menu();//Initialize the main menu
void init_header(); //Handle the header texts
int get_digits(int x); //Return the number of digits an int
void check_screen_size(); //Check if the terminal size is suitable for the game or not
char int_to_char(int x); //Cast a number to char type of it
void get_settings(); //Get desired settings of the custom mode
bool is_mine(int i,int j); //Check if a position is a mine or not
bool is_flag(int i,int j); //Check if a position is a flag or not
void print_table(bool showMines=false); //Print the whole table
int mines_nearby(int i, int j);//Returns the number of mines that are around a position
int flags_nearby(int i, int j);//Returns the number of flags that are around a position
void show_message(int message);//Show a specific message to the user during the game
void clear_message();//Clear the already written message
void select(int i, int j);//Select a position
void flag_item();//Make a position flag
void change_position(int input);//Change the position based on the user's input
void start_game();//Start a new game
void end_game();//End the current game
void update_footer();//Update the details in the footer

int main(){
    srand(time(0));
    init_clui();
    init_menu();
    flush();
    while(true){
        input = getch();
        row = get_window_rows();
        cols = get_window_cols();
        if(input==65){
            if(MENU_POSITION==0){
                play_beep();
                MENU_POSITION=3;
                init_menu();
            }
            else{
                play_beep();
                MENU_POSITION--;
                init_menu();
            }
        }else if(input==66){
            if(MENU_POSITION==3){
                play_beep();
                MENU_POSITION=0;
                init_menu();
            }else{
                play_beep();
                MENU_POSITION++;
                init_menu();
            }
        }else if(input==10){
            if(MENU_POSITION==0){
                SIZE = 5;
                MINES = 5;
                SHOW_NUMBERS = true;
                IS_TIMING = false;
            }else if(MENU_POSITION==1){
                SIZE = 10;
                MINES = 20;
                SHOW_NUMBERS = true;
                IS_TIMING = false;
            }else if(MENU_POSITION==2){
                SIZE = 15;
                MINES = 50;
                SHOW_NUMBERS = true;
                IS_TIMING = false;
            }else if(MENU_POSITION==3){
                get_settings();
            }
            start_game();
            while(REPEAT_GAME){
                start_game();
            }
            init_menu();
        }
    }
    
}
int get_digits(int x){
    int n=0;
    while(x!=0){
        x/=10;
        n++;
    }
    return n;
}
void check_screen_size(){
    bool FIRST_RUN = true;
    int row,col;
    int REQUIRED_ROWS = SIZE*2+2+4+1;
    int REQUIRED_COLS = max(35+2*(SIZE*SIZE-MINES)+1+14+get_digits(TIME_LENGTH),4*SIZE+1+4);
    while(get_window_rows()<REQUIRED_ROWS || get_window_cols()<86){
        if(FIRST_RUN || row!=get_window_rows() || col != get_window_cols()){
            clear_screen();
            cursor_to_pos((get_window_rows()-1)/2, (get_window_cols()-31)/2);
            cout << "The window is not big enough :(";
            FIRST_RUN=false;
            row = get_window_rows();
            col = get_window_cols();
        }
        flush();
    }
}
void init_header(){
    int row = get_window_rows();
    int cols = get_window_cols();
    cursor_to_pos(2,(cols-14)/2);
    change_color(12);
    cout << "[Minesweeper]";
    cursor_to_pos(4,(cols-13)/2);
    change_color(4);
    cout << "By @mfrashidi";
    cursor_to_pos(5,(cols-13)/2);
    change_color(6);
    cout << "ITP Fall 1400";
    change_color(0);
}
char int_to_char(int x){
    char a = 48+x;
    return ((a==0)?'-':a);
}
void get_settings(){
    int POSITION = 0;
    int len = 0;
    SIZE = 5;
    MINES = 1;
    while(true){
        change_color(12);
        cursor_to_pos(get_window_rows()-2, (get_window_cols()-24)/2);
        cout << "Hit ENTER to play the game";
        change_color(0);
        init_header();
        clear_screen();
        int row = (get_window_rows()-7)/2;
        int cols = get_window_cols();

        len = ((SIZE>=10)?4:2) + 12 + ((SIZE==5||SIZE==25)?2:4);
        cursor_to_pos(row,(cols-((POSITION==0)?len+4:len))/2);
        if(POSITION==0){
            change_color(2);
            cout << "* ";
            change_color(0);
        }
        change_color(10);
        cout << "[TABLE]  ";
        change_color(0); 
        cout << ((SIZE==5)?"":"< ") << SIZE << " * " << SIZE << ((SIZE==20)?"":" >");

        len = ((MINES>=10 && MINES<100)?1:0) + ((MINES>=100)?1:0) + 9 + ((MINES==1||MINES==SIZE*SIZE-1)?2:4);
        cursor_to_pos(row+2,(cols-((POSITION==1)?len+4:len))/2);
        if(POSITION==1){
            change_color(2);
            cout << "* ";
            change_color(0);
        }
        change_color(10);
        cout << "[MINES]  ";
        change_color(0); 
        cout << ((MINES==1)?"":"< ") << MINES << ((MINES==SIZE*SIZE-1)?"":" >");

        len = ((SHOW_NUMBERS)?3:2) + 19;
        cursor_to_pos(row+4,(cols-((POSITION==2)?len+4:len))/2);
        if(POSITION==2){
            change_color(2);
            cout << "* ";
            change_color(0);
        }
        change_color(10);
        cout << "[SHOW NUMBERS]  ";
        change_color(0); 
        cout << "< " <<((SHOW_NUMBERS)?"YES":"NO") << " >";

        len = ((TIME_LENGTH==0)?0:2) + 9 + ((TIME_LENGTH!=0)?get_digits(TIME_LENGTH):1);
        cursor_to_pos(row+6,(cols-((POSITION==3)?len+4:len))/2);
        if(POSITION==3){
            change_color(2);
            cout << "* ";
            change_color(0);
        }

        change_color(10);
        cout << "[TIME]  ";
        change_color(0); 
        cout << ((TIME_LENGTH==0)?"":"< ");
        if(TIME_LENGTH>0) cout << TIME_LENGTH << " >";
        else cout << "∞ >";

        cursor_to_pos(0, 0);
        input = getch();
        if(input==65){
            if(POSITION==0){
                play_beep();
                POSITION=3;
            }
            else{
                play_beep();
                POSITION--;
            }
        }else if(input==66){
            if(POSITION==3){
                play_beep();
                POSITION=0;
            }else{
                play_beep();
                POSITION++;
            }
        }else if(input==67){
            if(POSITION==0 && SIZE<20){
                SIZE++;
            }else if(POSITION==1 && MINES<SIZE*SIZE-1){
                MINES++;
            }else if(POSITION==2){
                SHOW_NUMBERS = !SHOW_NUMBERS;
            }else if(POSITION==3){
                TIME_LENGTH++;
            }
        }else if(input==68){
            if(POSITION==0 && SIZE>5){
                SIZE--;
                if(MINES>SIZE*SIZE-1) MINES=SIZE*SIZE-1;
            }else if(POSITION==1 && MINES>1){
                MINES--;
            }else if(POSITION==2){
                SHOW_NUMBERS = !SHOW_NUMBERS;
            }else if(POSITION==3 && TIME_LENGTH>0){
                TIME_LENGTH--;
            }
        }else if(input==10){
            if(TIME_LENGTH==0) IS_TIMING=false;
            else IS_TIMING=true;
            break;
        }
        flush();
    }
}
void initial(){
    check_screen_size();
    SCORE = 0;
    NUMBER_OF_FLAGS = 0;
    IS_LOST=false;
    IS_WON=false;
    row = -1;
    cols = -1;
    input = -1;
    if(IS_TIMING){
        START_TIME = time(0)+TIME_LENGTH;
        LOST_BY_TIME = false;
    }
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            table[i][j]=DEFAULT_CHAR;
        }
    }
    int n = 0;
    int i,j;
    while(n!=MINES){
        i = rand()%(SIZE);
        j = rand()%(SIZE);
        if(table[i][j]==DEFAULT_CHAR){
            //cursor_to_pos(n+1,0);
            //cout << "[" << i << "][" << j << "]";
            MINES_LOCATION[n][0]=i;
            MINES_LOCATION[n][1]=j;
            table[i][j] = MINE_CHAR;
            n++;
        }
    }
    WIDTH_SPACE = (get_window_cols()-(SIZE*5-(SIZE-1)+DEFAULT_SPACE))/2;
    HEIGHT_SPACE = ((get_window_rows()-2)-(SIZE*2+2))/2;
    SELECTED_ITEM[0] = SELECTED_ITEM[1] = SIZE/2;
}
bool is_mine(int i,int j){
    for(int k=0;k<MINES;k++){
        if(MINES_LOCATION[k][0]==i&&MINES_LOCATION[k][1]==j) return true;
    }
    return false;
}
bool is_flag(int i,int j){
    return ((table[i][j]==FLAG_CHAR)?true:false);
}
void print_table(bool showMines){
    cursor_to_pos(HEIGHT_SPACE, WIDTH_SPACE);
    for(int j=0;j<DEFAULT_SPACE+2;j++) cout << " ";
    char end = 96 + SIZE;
    change_color(ALPHABET_COLOR);
    for(char i='a';i<=end;i++) cout << i << "   ";
    change_color(0);
    cout << endl;
    int n = 1;
    for(int i=0;i<SIZE;i++){
        cursor_to_pos(HEIGHT_SPACE+i+n, WIDTH_SPACE);
        n++;
        for(int j=0;j<DEFAULT_SPACE;j++) cout << " ";
        for(int j=0;j<SIZE*5-(SIZE-1);j++){
            cout << "-";
        }
        cursor_to_pos(HEIGHT_SPACE+i+n, WIDTH_SPACE);
        change_color(NUMBERS_COLOR);
        cout << i+1;
        change_color(0);
        for(int j=0;j<DEFAULT_SPACE-((i+1>=10)?2:1);j++) cout << " ";
        for(int j=0;j<SIZE;j++){
            if(i==SELECTED_ITEM[0]&&j==SELECTED_ITEM[1] && !showMines){
                if(table[i][j]==DEFAULT_CHAR || table[i][j]==MINE_CHAR){
                    cout << "|";
                    change_color(SELECTED_COLOR);
                    cout << " ⦿ ";
                    change_color(0);
                    continue;
                }else{
                    cout << "| ";
                    change_color(SELECTED_COLOR);
                    if(table[i][j]==FLAG_CHAR||SHOW_NUMBERS){
                        cout << table[i][j];
                    }else cout << "✓";
                    cout << " ";
                    change_color(0);
                }
            }
            else if(table[i][j]==DEFAULT_CHAR) cout << "| . ";
            else if(table[i][j]==MINE_CHAR){
                if(showMines){
                    cout << "| ";
                    change_color(MINEs_COLOR);
                    cout << MINE_CHAR;
                    cout << " ";
                    change_color(0);
                }else cout << "| . ";
            }
            else if(table[i][j]==FLAG_CHAR){
                cout << "| ";
                change_color(FLAG_COLOR);
                if(is_mine(i,j) && showMines){
                    cout << MINE_CHAR;
                }else cout << FLAG_CHAR;
                cout << " ";
                change_color(0);
            }else{
                cout << "| ";
                change_color(EMPTYs_COLOR);
                if(SHOW_NUMBERS){
                    cout << table[i][j];
                }else cout << "✓";
                cout << " ";
                change_color(0);
            }
        }
        cout << "|";
    }
    cursor_to_pos(HEIGHT_SPACE+SIZE+n, WIDTH_SPACE);
    cout << "    ";
    for(int j=0;j<SIZE*5-(SIZE-1);j++) cout << "-";
}
int mines_nearby(int i, int j){
    int n = 0;
    if(j-1>=0){if(is_mine(i,j-1)) n++;}
    if(j+1<=SIZE-1){if(is_mine(i,j+1)) n++;}
    if(i-1>=0){if(is_mine(i-1,j)) n++;}
    if(i+1<=SIZE-1){if(is_mine(i+1,j)) n++;}
    if(j-1>=0 && i-1>=0){if(is_mine(i-1,j-1)) n++;}
    if(j+1<=SIZE-1 && i+1<=SIZE-1){if(is_mine(i+1,j+1)) n++;}
    if(j+1<=SIZE-1 && i-1>=0){if(is_mine(i-1,j+1)) n++;}
    if(j-1>=0 && i+1<=SIZE-1){if(is_mine(i+1,j-1)) n++;}
    return n;
}
int flags_nearby(int i, int j){
    int n = 0;
    if(j-1>=0){if(is_flag(i,j-1)) n++;}
    if(j+1<=SIZE-1){if(is_flag(i,j+1)) n++;}
    if(i-1>=0){if(is_flag(i-1,j)) n++;}
    if(i+1<=SIZE-1){if(is_flag(i+1,j)) n++;}
    if(j-1>=0 && i-1>=0){if(is_flag(i-1,j-1)) n++;}
    if(j+1<=SIZE-1 && i+1<=SIZE-1){if(is_flag(i+1,j+1)) n++;}
    if(j+1<=SIZE-1 && i-1>=0){if(is_flag(i-1,j+1)) n++;}
    if(j-1>=0 && i+1<=SIZE-1){if(is_flag(i+1,j-1)) n++;}
    return n;
}
void show_message(int message){
    if(message==0){
        cursor_to_pos(get_window_rows()-3, (get_window_cols()-24)/2);
        change_color(GOOD_NEWS_COLOR);
        cout << "Congratulations, You won!";
        cursor_to_pos(get_window_rows()-2, (get_window_cols()-33)/2);
        cout << "Hit R to play again or hit ← to exit";
        change_color(0);
        MESSAGE_SHOWN = true;
    }else if(message==1){
        cursor_to_pos(get_window_rows()-3, (get_window_cols()-16)/2);
        change_color(BAD_NEWS_COLOR);
        cout << "Sorry, You lost!";
        cursor_to_pos(get_window_rows()-2, (get_window_cols()-33)/2);
        cout << "Hit R to play again or hit ← to exit";
        change_color(0);
        MESSAGE_SHOWN = true;
    }else if(message==2){
        cursor_to_pos(get_window_rows()-3, (get_window_cols()-28)/2);
        change_color(ERRORS_COLOR);
        cout << "Where are you going dude? :)";
        change_color(0);
        MESSAGE_SHOWN = true;
    }else if(message==3){
        cursor_to_pos(get_window_rows()-3, (get_window_cols()-21)/2);
        change_color(ERRORS_COLOR);
        cout << "All the flags are used";
        change_color(0);
        MESSAGE_SHOWN = true;
    }else if(message==4){
        cursor_to_pos(get_window_rows()-3, (get_window_cols()-18)/2);
        change_color(ERRORS_COLOR);
        cout << "Kidding me, Right?";
        change_color(0);
        MESSAGE_SHOWN = true;
    }else if(message==5){
        cursor_to_pos(get_window_rows()-3, (get_window_cols()-21)/2);
        change_color(ERRORS_COLOR);
        cout << "Remove the flag first";
        change_color(0);
        MESSAGE_SHOWN = true;
    }else if(message==6){
        cursor_to_pos(get_window_rows()-3, (get_window_cols()-26)/2);
        change_color(ERRORS_COLOR);
        cout << "You already checked that :/";
        change_color(0);
        MESSAGE_SHOWN = true;
    }else if(message==7){
        cursor_to_pos(get_window_rows()-3, (get_window_cols()-28)/2);
        change_color(BAD_NEWS_COLOR);
        cout << "Sorry, Your time has run out!";
        cursor_to_pos(get_window_rows()-2, (get_window_cols()-33)/2);
        cout << "Hit R to play again or hit ← to exit";
        change_color(0);
        MESSAGE_SHOWN = true;
    }
}
void clear_message(){
    cursor_to_pos(get_window_rows()-3, 0);
    for(int i=0;i<get_window_cols();i++) cout<<" ";
    MESSAGE_SHOWN = false;
}
void change_position(int input){
    if(input==65){
        if(SELECTED_ITEM[0]-1>=0){
            SELECTED_ITEM[0]--;
            print_table();
            }
        else{play_beep();show_message(2);}
    }
    if(input==66){
        if(SELECTED_ITEM[0]+1<=SIZE-1){
            SELECTED_ITEM[0]++;
            print_table();
            }
        else{play_beep();show_message(2);}
    }
    if(input==67){
        if(SELECTED_ITEM[1]+1<=SIZE-1){
            SELECTED_ITEM[1]++;
            print_table();
        }
        else{play_beep();show_message(2);}
        }
    if(input==68){
        if(SELECTED_ITEM[1]-1>=0){
            SELECTED_ITEM[1]--;
            print_table();
        }
        else{play_beep();show_message(2);}
    }
}
void flag_item(){
    if(table[SELECTED_ITEM[0]][SELECTED_ITEM[1]]==FLAG_CHAR){
        for(int i=0;i<MINES;i++){
            if(SELECTED_ITEM[0]==MINES_LOCATION[i][0]&&SELECTED_ITEM[1]==MINES_LOCATION[i][1]){
                table[SELECTED_ITEM[0]][SELECTED_ITEM[1]]=MINE_CHAR;
                break;
            }else table[SELECTED_ITEM[0]][SELECTED_ITEM[1]]=DEFAULT_CHAR;
        }
        NUMBER_OF_FLAGS--;}
    else if(table[SELECTED_ITEM[0]][SELECTED_ITEM[1]]==DEFAULT_CHAR || table[SELECTED_ITEM[0]][SELECTED_ITEM[1]]==MINE_CHAR){
        if(NUMBER_OF_FLAGS<MINES){
            table[SELECTED_ITEM[0]][SELECTED_ITEM[1]]=FLAG_CHAR;
            NUMBER_OF_FLAGS++;
        }else{play_beep();show_message(3);}
    }else{play_beep();show_message(4);}
    print_table();

}
void end_game(){
    if(IS_LOST){
        show_message(1);
        table[SELECTED_ITEM[0]][SELECTED_ITEM[1]]=MINE_CHAR;
    }else if(IS_WON){
        show_message(0);
        table[SELECTED_ITEM[0]][SELECTED_ITEM[1]]=int_to_char(mines_nearby(SELECTED_ITEM[0],SELECTED_ITEM[1]));
    }else if(LOST_BY_TIME){
        show_message(7);
    }
    print_table(true);
}
void select(int i, int j){
    if(table[i][j]==DEFAULT_CHAR){
        int nearMines = mines_nearby(i,j);
        int nearFlags = flags_nearby(i,j);
        if(nearMines==0&&nearFlags==0){
            if(j-1>=0 && table[i][j-1]==DEFAULT_CHAR){table[i][j-1] = int_to_char(mines_nearby(i,j-1));SCORE++;}
            if(j+1<=SIZE-1 && table[i][j+1]==DEFAULT_CHAR){table[i][j+1] = int_to_char(mines_nearby(i,j+1));SCORE++;}
            if(i-1>=0 && table[i-1][j]==DEFAULT_CHAR){table[i-1][j] = int_to_char(mines_nearby(i-1,j));SCORE++;}
            if(i+1<=SIZE-1 && table[i+1][j]==DEFAULT_CHAR){table[i+1][j] = int_to_char(mines_nearby(i+1,j));SCORE++;}
            if(j-1>=0 && i-1>=0 && table[i-1][j-1]==DEFAULT_CHAR){table[i-1][j-1] = int_to_char(mines_nearby(i-1,j-1));SCORE++;}
            if(j+1<=SIZE-1 && i+1<=SIZE-1 && table[i+1][j+1]==DEFAULT_CHAR){table[i+1][j+1] = int_to_char(mines_nearby(i+1,j+1));SCORE++;}
            if(j+1<=SIZE-1 && i-1>=0 && table[i-1][j+1]==DEFAULT_CHAR){table[i-1][j+1] = int_to_char(mines_nearby(i-1,j+1));SCORE++;}
            if(j-1>=0 && i+1<=SIZE-1 && table[i+1][j-1]==DEFAULT_CHAR){table[i+1][j-1] = int_to_char(mines_nearby(i+1,j-1));SCORE++;}
        }
        table[i][j] = int_to_char(nearMines);
        SCORE++;
        print_table();
    }else if(table[i][j]==FLAG_CHAR){
        show_message(5);
        play_beep();
    }else if(table[i][j]==MINE_CHAR){
        IS_LOST = true;
        end_game();
    }else{
        show_message(6);
        play_beep();
    }
}
void update_footer(){
    int len;
    if(SCORE==SIZE*SIZE-MINES){
        IS_WON=true;
        end_game();
    }else if(START_TIME-time(0)<=0 && IS_TIMING){
        LOST_BY_TIME = true;
        end_game();
    }
    cursor_to_pos(get_window_rows(), 0);
    len = 0;
    if(MENU_POSITION==0){
        change_color(GOOD_NEWS_COLOR);
        cout << "(EASY)    ";
        len += 10;
    }else if(MENU_POSITION==1){
        change_color(ERRORS_COLOR);
        cout << "(MEDIUM)    ";
        len += 12;
    }else if(MENU_POSITION==2){
        change_color(BAD_NEWS_COLOR);
        cout << "(HARD)    ";
        len += 10;
    }else{
        change_color(6);
        cout << "(CUSTOM)    ";
        len += 12;
    }
    change_color(SELECTED_COLOR);
    char pos = SELECTED_ITEM[1]+97;
    cout << "Position: " << pos << SELECTED_ITEM[0]+1 << "   ";
    len += 10+get_digits(SELECTED_ITEM[0]+1) + 1 + 3;
    change_color(EMPTYs_COLOR);
    cout << "Score: " << SCORE << "/" << SIZE*SIZE-MINES << "   ";
    change_color(0);
    len += 7+get_digits(SCORE)+1+get_digits(SIZE*SIZE-MINES)+3;
    change_color(FLAG_COLOR);
    cout << "Flags: " << NUMBER_OF_FLAGS << "/" << MINES << "   ";
    change_color(0);
    len += 7+get_digits(NUMBER_OF_FLAGS)+1+get_digits(MINES)+3;
    cursor_to_pos(get_window_rows(), len);
    for(int i=len;i<get_window_cols();i++) cout << " ";
    if(IS_TIMING && !IS_WON && !IS_LOST){
        if(START_TIME-time(0)<=0){
            len = 14 + 1;
            cursor_to_pos(get_window_rows(), get_window_cols()-len);
            cout << "Time: ";
            change_color(2);
            cout << "0 seconds";
            change_color(0);
        }else{
            len = 14 + get_digits(START_TIME-time(0));
            cursor_to_pos(get_window_rows(), get_window_cols()-len);
            cout << "Time: ";
            if(START_TIME-time(0)<10){
                change_color(2);
                cout << START_TIME-time(0) << " seconds";
                change_color(0);
            }else cout << START_TIME-time(0) << " seconds";
        }
    }
    cursor_to_pos(0, 0);
}
void start_game(){
    initial();
    while(true){
        if(get_window_rows()!=row || get_window_cols()!=cols){
            row = get_window_rows();
            cols = get_window_cols();
            clear_screen();
            print_table();
            cursor_to_pos(row - 1, 1);
            for(int i=0;i<cols;i++) cout << "-";
        }
        update_footer();
        input = getch();
        if(!IS_LOST && !IS_WON && !LOST_BY_TIME){
            if(MESSAGE_SHOWN) clear_message();
            if(input==10) select(SELECTED_ITEM[0],SELECTED_ITEM[1]);
            if(input==32) flag_item();
            if(65<=input&&input<=68) change_position(input);
            if(input==114 || input==82){
                REPEAT_GAME = true;
                break;
            }
            if(input==127){
                REPEAT_GAME = false;
                break;
            }
        }else if((IS_LOST||IS_WON||LOST_BY_TIME) && (input==114 || input==82)){
            REPEAT_GAME = true;
            break;
        }else if((IS_LOST||IS_WON||LOST_BY_TIME) && input==127){
            REPEAT_GAME = false;
            break;
        }else play_beep();
        flush();
    }
}
void init_menu(){
    clear_screen();
    init_header();
    int row = (get_window_rows()-7)/2;
    int cols = get_window_cols();
    cursor_to_pos(row,(cols-((MENU_POSITION==0)?10:6))/2);
    if(MENU_POSITION==0){
        change_color(2);
        cout << "* ";
        change_color(0);
    }
    cout << "[EASY]";
    cursor_to_pos(row+2,(cols-((MENU_POSITION==1)?12:8))/2);
    if(MENU_POSITION==1){
        change_color(2);
        cout << "* ";
        change_color(0);
    }
    cout << "[MEDIUM]";
    cursor_to_pos(row+4,(cols-((MENU_POSITION==2)?10:6))/2);
    if(MENU_POSITION==2){
        change_color(2);
        cout << "* ";
        change_color(0);
    }
    cout << "[HARD]";
    cursor_to_pos(row+6,(cols-((MENU_POSITION==3)?12:8))/2);
    if(MENU_POSITION==3){
        change_color(2);
        cout << "* ";
        change_color(0);
    }
    cout << "[CUSTOM]";
    cursor_to_pos(0, 0);
}