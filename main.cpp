#include <iostream>
#include "clui.h"
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <string.h>
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
bool START_GAME = true;
int SCORE;
int MENU_POSITION = 0;
int TIME_LENGTH = 0;
int START_TIME;
int row,cols,input;
bool LOST_BY_TIME = false;
char USERNAME[11];
int USERNAME_LENGTH = 15;
int GAME_MODE = 0;
string SAVED_DATE;
bool IS_LOADED_GAME;
int SLOT;
bool CHANGING_USERNAME = true;
string KEY = "DONT EVEN THINK ABOUT IT";



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
void select_game();//Select a saved game
void get_username();//Get username from user :)
void change_username();//Change the username
int size_of();//Get length of the username
void add_score(int score);//Add score to the username
int get_score(const char* line);//Get score of a username
void show_leaderboard();//Show leaderboard from a file
bool is_file_exist(const char *file);//Check if a file exists or not
char* substring(char* s, int begin, int size);//Get part of a string
int char_to_int(char* s);//Return int value of an int character
void load_game();//Saved game menu
void load_slot(int slot);//Load a specific save slot
void save_game(int slot=0);//Save this game
const string current_time();//Get current time
void remove_slot(int slot);//Remove a save slot
string encrypt(string msg);//Encrypt a string with a key
void secure(string file_name);//Secure a file to prevent cheating

int main(){
    srand(time(0));
    init_clui();
    get_username();
    clear_screen();
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
                START_GAME = true;
                select_game();
            }else if(MENU_POSITION==1){
                START_GAME = false;
                get_username();
            }else if(MENU_POSITION==2){
                START_GAME = false;
                show_leaderboard();
            }else if(MENU_POSITION==3){
                break;
            }
            if(START_GAME){
                start_game();
                while(REPEAT_GAME){
                    start_game();
                }
            }
            init_menu();
        }
    }
    
}


string encrypt(string msg){
    string key = KEY;
    string tmp(key);
    while (key.size() < msg.size())
        key += tmp;
    for (string::size_type i = 0; i < msg.size(); ++i)
        msg[i] ^= key[i];
    return msg;
}
void secure(string file_name){
    ifstream file(file_name);
    ofstream tmp_file("tmp");
    string line;
    while(getline(file,line)){
        tmp_file << encrypt(line) << endl;
    }
    file.close();
    tmp_file.close();

    ofstream file2(file_name);
    ifstream tmp_file2("tmp");
    while(getline(tmp_file2,line)){
        file2 << line << endl;
    }
    file2.close();
    tmp_file2.close();
    remove("tmp");
}

const string current_time() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

bool is_file_exist(string file){
    std::ifstream infile(file);
    return infile.good();
}

void remove_slot(int slot){
    remove((string("files/")+string(USERNAME)+string("_save_")+to_string(slot)+string(".txt")).c_str());
    int i = slot+1;
    while(is_file_exist(string("files/")+string(USERNAME)+string("_save_")+to_string(i)+string(".txt"))){
        rename((string("files/")+string(USERNAME)+string("_save_")+to_string(i)+string(".txt")).c_str(),(string("files/")+string(USERNAME)+string("_save_")+to_string(i-1)+string(".txt")).c_str());
        i++;
    }
}

void load_slot(int slot){
    secure(string("files/")+string(USERNAME)+string("_save_")+to_string(slot)+string(".txt"));
    ifstream save_slot(string("files/")+string(USERNAME)+string("_save_")+to_string(slot)+string(".txt"));
    string line;
    int i = 0;
    IS_LOST = false;
    IS_WON = false;
    while(getline(save_slot,line)){
        if(i==0) SAVED_DATE = line;
        else if(i==1) GAME_MODE = stoi(line);
        else if(i==2) SCORE = stoi(line);
        else if(i==3) NUMBER_OF_FLAGS = stoi(line);
        else if(i==4) SIZE = stoi(line);
        else if(i==5) MINES = stoi(line);
        else if(i==6) SHOW_NUMBERS = stoi(line);
        else if(i==7){
            if(stoi(line)==-1) IS_TIMING = false;
            else{
                IS_TIMING = true;
                START_TIME = time(0) + stoi(line) + 1;
            }
        }else if(i==8){
            int x = 0;
            int y = 0;
            int k = 0;
            bool first = true;
            int j = 0;
            while(j<MINES){
                if(line[k]==','){
                    MINES_LOCATION[i][0]=x;
                    first = false;
                }else if(line[k]=='/'){
                    MINES_LOCATION[i][1]=y;
                    x = 0;
                    y = 0;
                    j++;
                    first = true;
                }else{
                    if(first){
                        x *= 10;
                        x += ((int)line[k])-48;
                    }else{
                        y *= 10;
                        y += ((int)line[k])-48;
                    }
                }
                k++;
            }
        }else if(i==9){
            for(int i=0;i<SIZE;i++){
                for(int j=0;j<SIZE;j++){
                    table[i][j] = line[i*SIZE+j];
                }
            }
        }
        i++;
    }
    save_slot.close();
    secure(string("files/")+string(USERNAME)+string("_save_")+to_string(slot)+string(".txt"));
    WIDTH_SPACE = (get_window_cols()-(SIZE*5-(SIZE-1)+DEFAULT_SPACE))/2;
    HEIGHT_SPACE = ((get_window_rows()-2)-(SIZE*2+2))/2;
    SELECTED_ITEM[0] = SELECTED_ITEM[1] = SIZE/2;
}

void load_game(){
    check_screen_size();
    int slots = 0;
    while(is_file_exist(string("files/")+string(USERNAME)+string("_save_")+to_string(slots+1)+string(".txt"))){
        slots++;
    }
    int slot = 1;
    while(true){
        clear_screen();
        init_header();
        if(slots==0){
            int row = (get_window_rows())/2;
            int cols = (get_window_cols()-8)/2;
            cursor_to_pos(row,cols);
            change_color(6);
            cout << "Nothing!";
            change_color(0);
        }else{
            load_slot(slot);
            int game_mode_len;
            if(GAME_MODE==0) game_mode_len=6;
            if(GAME_MODE==1) game_mode_len=8;
            if(GAME_MODE==2) game_mode_len=6;
            if(GAME_MODE==3) game_mode_len=8;
            int row = (get_window_rows())/2;
            int cols = (get_window_cols()-(7+get_digits(slot)+strlen(SAVED_DATE.c_str())+game_mode_len))/2;
            cursor_to_pos(row,cols);
            change_color(0);
            cout << "< ";
            change_color(2);
            cout << "#" << slot;
            change_color(12);
            cout << " " << SAVED_DATE << " ";
            change_color(9);
            if(GAME_MODE==0) cout << "(EASY)";
            if(GAME_MODE==1) cout << "(MEDIUM)";
            if(GAME_MODE==2) cout << "(HARD)";
            if(GAME_MODE==3) cout << "(CUSTOM)";
            change_color(0);
            cout << " >";
        }
        if(slots>0){
            cursor_to_pos(get_window_rows()-3, (get_window_cols()-59)/2);
            change_color(ERRORS_COLOR);
            cout << "Saved games will autosave automatically to prevent cheating";
        }
        cursor_to_pos(get_window_rows()-2, (get_window_cols()-27)/2);
        change_color(MENU_COLOR);
        cout << "Press ← to back to the menu";
        change_color(0);
        cursor_to_pos(0,0);
        input = getch();
        if(input==67 && slots!=0){
            if(slot==slots) slot=1;
            else slot++;
        }else if(input==68 && slots!=0){
            if(slot==1) slot=slots;
            else slot--;
        }else if(input==10 && slots!=0){
            SLOT = slot;
            break;
        }else if(input==127){
            START_GAME = false;
            break;
        }
        flush();
    }
}
void save_game(int slot){
    if(slot==0){
        slot=1;
        while(is_file_exist(string("files/")+string(USERNAME)+string("_save_")+to_string(slot)+string(".txt"))){
            slot++;
        }
    }
    ofstream save_slot(string("files/")+string(USERNAME)+string("_save_")+to_string(slot)+string(".txt"));
    save_slot << current_time() << "\n";
    save_slot << GAME_MODE << "\n";
    save_slot << SCORE << "\n";
    save_slot << NUMBER_OF_FLAGS << "\n";
    save_slot << SIZE << "\n";
    save_slot << MINES << "\n";
    save_slot << SHOW_NUMBERS << "\n";
    if(IS_TIMING) save_slot << START_TIME-time(0) << "\n";
    else save_slot << -1 << "\n";
    for(int i=0;i<MINES;i++){
        save_slot << MINES_LOCATION[i][0] << "," << MINES_LOCATION[i][1] << "/";
    }
    save_slot << "\n";
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            save_slot << table[i][j];
        }
    }
    save_slot << "\n";
    save_slot.close();
    secure(string("files/")+string(USERNAME)+string("_save_")+to_string(slot)+string(".txt"));
}
char* substring(const char* s, int begin, int size){ 
    char* news = new char[size+1];
    for (int i=0, j = begin; i < size; ++i, j++)
        news[i] = s[j]; 
    news[size] = '\0';
    return news;
}
int char_to_int(char* s){
    int number = 0;
    int i = 0;
    while(true){
        if(s[i]=='\0') break;
        number*=10;
        number+=(int)s[i]-48;
        i++;
    }
    return number;
}
void bubble_sort(int scores[],char* names[], int size){
    for(int i=0;i<size-1;i++)
        for(int j=0;j<size-i-1;j++)
            if(scores[j]<scores[j+1]){
                int tmp = scores[j];
                scores[j] = scores[j+1];
                scores[j+1] = tmp;
                char* tmpName = names[j];
                names[j] = names[j+1];
                names[j+1] = tmpName;
            }
}
void show_leaderboard(){
    int n = 0;
    int i = 0;
    string line;
    ifstream read("files/leaderboard.txt");
    while(getline(read,line)) n++;
    read.clear();
    read.seekg(0, read.beg);    
    char* names[n];
    int scores[n];
    while(getline(read,line)){
        names[i] = substring(line.c_str(),0,strchr(line.c_str(), '=')-line.c_str());
        scores[i] = char_to_int(substring(line.c_str(),strchr(line.c_str(), '=')-line.c_str()+1,strlen(line.c_str())));
        i++;
    }
    bubble_sort(scores,names,n);
    while(true){
        clear_screen();
        init_header();
        if(n!=0){
            int row = (get_window_rows()-(2*n))/2;
            int cols = get_window_cols();
            int spcaes = 1;
            cursor_to_pos(row,(cols-13)/2);
            change_color(6);
            cout << "[Leaderboard]";
            change_color(0);
            for(int i=0;i<((n<=10)?n:10);i++){
                cursor_to_pos(row+i+1+spcaes,(cols-(4+strlen(names[i])+get_digits(i+1)+get_digits(scores[i])))/2);
                change_color(5);
                cout << i+1 << ") ";
                change_color(11);
                cout << names[i] << ": ";
                change_color(4);
                cout << scores[i];
                spcaes++;
            }            
        }else{
            int row = (get_window_rows())/2;
            int cols = (get_window_cols()-34)/2;
            cursor_to_pos(row,cols);
            change_color(6);
            cout << "Play some games and then return :)";
            change_color(0);
        }
        cursor_to_pos(get_window_rows()-3, (get_window_cols()-31)/2);
        change_color(ERRORS_COLOR);
        cout << "Press R to reset the leaderboard";
        cursor_to_pos(get_window_rows()-2, (get_window_cols()-27)/2);
        change_color(MENU_COLOR);
        cout << "Press ← to back to the menu";
        change_color(0);
        cursor_to_pos(0,0);
        input = getch();
        if(input==127) break;
        if(input==114 || input==82){
            remove("files/leaderboard.txt");
            break;
        }
    }

}

int size_of(char* username){
    for(int i=0;i<USERNAME_LENGTH+1;i++) if(username[i]=='\0') return i;
    return 0;
}
void change_username(char* username, char f, char c='a'){
    int size = size_of(username);
    if(f=='a'){
        if(size<USERNAME_LENGTH){
            username[size] = c;
            username[size+1] = '\0';
        }
    }else if(f=='r'){
        username[size-1] = '\0';
    }
}

int get_score(const char* line){
    int score=0;
    int i=0;
    while(true){
        if(line[i]=='\n'||line[i]=='\0') break;
        if(line[i]>='0'&&line[i]<='9'){
            score *= 10;
            score += (int)line[i]-48;
        }
        i++;
    }
    return score;
}

void add_score(int score){
    ifstream read_backup("files/leaderboard.txt");
    ofstream write_backup("files/tmp");
    string line;
    bool is_found = false;
    while(getline(read_backup,line)){
        if(strstr(line.c_str(),USERNAME)){
            write_backup << USERNAME << "=" << get_score(line.c_str())+score << '\n';
            is_found = true;
        }else{
            write_backup << line << '\n';
        }
    }
    if(!is_found) write_backup << USERNAME << "=" << get_score(line.c_str())+score << '\n';
    write_backup.close();
    read_backup.close();

    ifstream read("files/tmp");
    ofstream write("files/leaderboard.txt");
    while(getline(read,line)){
        write << line << '\n';
    }
    write.close();
    read.close();
    remove("files/tmp");
}
void get_username(){
    while(true){
        clear_screen();
        init_header();
        CHANGING_USERNAME = true;
        int row = (get_window_rows()-3)/2;
        int cols = get_window_cols();
        cursor_to_pos(row,(cols-20)/2);
        cout << "Enter your username:";
        cursor_to_pos(row+2,(cols-size_of(USERNAME))/2);
        change_color(2);
        cout << USERNAME;

        cursor_to_pos(get_window_rows()-3, (get_window_cols()-27)/2);
        change_color(MENU_COLOR);
        cout << "Type your name and hit ENTER";
        change_color(0);
        cursor_to_pos(0,0);

        input = getch();
        if(input==10&&size_of(USERNAME)!=0){
            CHANGING_USERNAME = false;
            break;
        }else if(input==127){
            change_username(USERNAME,'r');
        }else if((input>='a'&&input<='z')||(input>='A'&&input<='Z')||input=='_'||input=='.'){
            change_username(USERNAME,'a',input);
        }

    }
}
void select_game(){
    int POSITION = 0;
    while(true){
        clear_screen();
        init_header();
        int row = (get_window_rows()-9)/2;
        int cols = get_window_cols();
        cursor_to_pos(row,(cols-((POSITION==0)?10:6))/2);
        if(POSITION==0){
            change_color(2);
            cout << "* ";
            change_color(0);
        }
        cout << "[EASY]";
        cursor_to_pos(row+2,(cols-((POSITION==1)?12:8))/2);
        if(POSITION==1){
            change_color(2);
            cout << "* ";
            change_color(0);
        }
        cout << "[MEDIUM]";
        cursor_to_pos(row+4,(cols-((POSITION==2)?10:6))/2);
        if(POSITION==2){
            change_color(2);
            cout << "* ";
            change_color(0);
        }
        cout << "[HARD]";
        cursor_to_pos(row+6,(cols-((POSITION==3)?12:8))/2);
        if(POSITION==3){
            change_color(2);
            cout << "* ";
            change_color(0);
        }
        cout << "[CUSTOM]";
        cursor_to_pos(row+8,(cols-((POSITION==4)?17:13))/2);
        if(POSITION==4){
            change_color(2);
            cout << "* ";
            change_color(0);
        }
        cout << "[SAVED GAMES]";
        

        cursor_to_pos(1, 1);
        cout << endl;
        cursor_to_pos(0, 0);
        input = getch();
        if(input==10){
            START_GAME = true;
            if(POSITION==0){
                SIZE = 5;
                MINES = 4;
                SHOW_NUMBERS = true;
                IS_TIMING = false;
                GAME_MODE = 0;
                IS_LOADED_GAME = false;
            }else if(POSITION==1){
                SIZE = 12;
                MINES = 28;
                SHOW_NUMBERS = true;
                IS_TIMING = false;
                GAME_MODE = 1;
                IS_LOADED_GAME = false;
            }else if(POSITION==2){
                SIZE = 20;
                MINES = 96;
                SHOW_NUMBERS = true;
                IS_TIMING = false;
                GAME_MODE = 2;
                IS_LOADED_GAME = false;
            }else if(POSITION==3){
                GAME_MODE = 3;
                get_settings();
                IS_LOADED_GAME = false;
            }else if(POSITION==4){
                IS_LOADED_GAME = true;
                load_game();
            }
            if(START_GAME){
                start_game();
                while(REPEAT_GAME){
                    start_game();
                }
            }
        }else if(input==65){
            if(POSITION==0){
                play_beep();
                POSITION=4;
            }
            else{
                play_beep();
                POSITION--;
            }
        }else if(input==66){
            if(POSITION==4){
                play_beep();
                POSITION=0;
            }else{
                play_beep();
                POSITION++;
            }
        }else if(input==127){
            START_GAME = false;
            break;
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
    if(!CHANGING_USERNAME){
        cursor_to_pos(6,(cols-(13+strlen(USERNAME)))/2);
        change_color(2);
        cout << "Playing as (" << USERNAME << ")";
    }
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
        }else if(input==127){
            START_GAME = false;
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
        add_score(MINES+((IS_TIMING)?(START_TIME-time(0))/5:0));
        show_message(0);
        table[SELECTED_ITEM[0]][SELECTED_ITEM[1]]=int_to_char(mines_nearby(SELECTED_ITEM[0],SELECTED_ITEM[1]));
    }else if(LOST_BY_TIME){
        show_message(7);
    }
    if(IS_LOADED_GAME) remove_slot(SLOT);
    print_table(true);
}
void select(int i, int j){
    if(table[i][j]==DEFAULT_CHAR){
        int nearMines = mines_nearby(i,j);
        int nearFlags = flags_nearby(i,j);
        table[i][j] = int_to_char(nearMines);
        if(nearMines==0&&nearFlags==0){
            if(j-1>=0 && table[i][j-1]==DEFAULT_CHAR){
                if(mines_nearby(i,j-1)==0) select(i,j-1);
                if(table[i][j-1]==DEFAULT_CHAR){
                    table[i][j-1] = int_to_char(mines_nearby(i,j-1));
                    SCORE++;
                }
            }if(j+1<=SIZE-1 && table[i][j+1]==DEFAULT_CHAR){
                if(mines_nearby(i,j+1)==0) select(i,j+1);
                if(table[i][j+1]==DEFAULT_CHAR){
                    table[i][j+1] = int_to_char(mines_nearby(i,j+1));
                    SCORE++;
                }
            }if(i-1>=0 && table[i-1][j]==DEFAULT_CHAR){
                if(mines_nearby(i-1,j)==0) select(i-1,j);
                if(table[i-1][j]==DEFAULT_CHAR){
                    table[i-1][j] = int_to_char(mines_nearby(i-1,j));
                    SCORE++;
                }
            }if(i+1<=SIZE-1 && table[i+1][j]==DEFAULT_CHAR){
                if(mines_nearby(i+1,j)==0) select(i+1,j);
                if(table[i+1][j]==DEFAULT_CHAR){
                    table[i+1][j] = int_to_char(mines_nearby(i+1,j));
                    SCORE++;
                }
            }if(j-1>=0 && i-1>=0 && table[i-1][j-1]==DEFAULT_CHAR){
                if(mines_nearby(i-1,j-1)==0) select(i-1,j-1);
                if(table[i-1][j-1]==DEFAULT_CHAR){
                    table[i-1][j-1] = int_to_char(mines_nearby(i-1,j-1));
                    SCORE++;
                }
            }if(j+1<=SIZE-1 && i+1<=SIZE-1 && table[i+1][j+1]==DEFAULT_CHAR){
                if(mines_nearby(i+1,j+1)==0) select(i+1,j+1);
                if(table[i+1][j+1]==DEFAULT_CHAR){
                    table[i+1][j+1] = int_to_char(mines_nearby(i+1,j+1));
                    SCORE++;
                }
            }if(j+1<=SIZE-1 && i-1>=0 && table[i-1][j+1]==DEFAULT_CHAR){
                if(mines_nearby(i-1,j+1)==0) select(i-1,j+1);
                if(table[i-1][j+1]==DEFAULT_CHAR){
                    table[i-1][j+1] = int_to_char(mines_nearby(i-1,j+1));
                    SCORE++;
                }
            }if(j-1>=0 && i+1<=SIZE-1 && table[i+1][j-1]==DEFAULT_CHAR){
                if(mines_nearby(i+1,j-1)==0) select(i+1,j-1);
                if(table[i+1][j-1]==DEFAULT_CHAR){
                    table[i+1][j-1] = int_to_char(mines_nearby(i+1,j-1));
                    SCORE++;
                }
            }
        }
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
    if(GAME_MODE==0){
        change_color(GOOD_NEWS_COLOR);
        cout << "(EASY)    ";
        len += 10;
    }else if(GAME_MODE==1){
        change_color(ERRORS_COLOR);
        cout << "(MEDIUM)    ";
        len += 12;
    }else if(GAME_MODE==2){
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
    if(!IS_LOADED_GAME) initial();
    clear_screen();
    print_table();
    row = get_window_rows();
    cols = get_window_cols();
    cursor_to_pos(row - 1, 1);
    for(int i=0;i<cols;i++) cout << "-";
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
        cursor_to_pos(0,0);
        if(IS_LOADED_GAME && !IS_WON && !IS_LOST && !LOST_BY_TIME) save_game(SLOT);
        if(START_TIME-time(0)<=0 && IS_TIMING){
             LOST_BY_TIME=true;
             end_game();
        }
        if(!IS_LOST && !IS_WON && !LOST_BY_TIME){
            if(MESSAGE_SHOWN) clear_message();
            if(input==10) select(SELECTED_ITEM[0],SELECTED_ITEM[1]);
            else if(input==32) flag_item();
            else if(65<=input&&input<=68) change_position(input);
            else if((input==114 || input==82)&& !IS_LOADED_GAME){
                REPEAT_GAME = true;
                break;
            }
            else if(input==127){
                REPEAT_GAME = false;
                break;
            }else if((input==83 || input==115)&&!IS_LOADED_GAME){
                save_game();
                REPEAT_GAME = false;
                break;
            }
        }else if((IS_LOST||IS_WON||LOST_BY_TIME) && (input==114 || input==82) && !IS_LOADED_GAME){
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
    cursor_to_pos(row,(cols-((MENU_POSITION==0)?15:11))/2);
    if(MENU_POSITION==0){
        change_color(2);
        cout << "* ";
        change_color(0);
    }
    cout << "[Play Game]";
    cursor_to_pos(row+2,(cols-((MENU_POSITION==1)?17:13))/2);
    if(MENU_POSITION==1){
        change_color(2);
        cout << "* ";
        change_color(0);
    }
    cout << "[Change Name]";
    cursor_to_pos(row+4,(cols-((MENU_POSITION==2)?17:13))/2);
    if(MENU_POSITION==2){
        change_color(2);
        cout << "* ";
        change_color(0);
    }
    cout << "[Leaderboard]";
    cursor_to_pos(row+6,(cols-((MENU_POSITION==3)?10:6))/2);
    if(MENU_POSITION==3){
        change_color(2);
        cout << "* ";
        change_color(0);
    }
    cout << "[Quit]";
    cursor_to_pos(0, 0);
}