#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <queue>
#include <fstream>
#include <windows.h>
#include <filesystem>

enum directions {up, right, down, left}; // reminder of directions values (0 - up, 1 - right, 2 - down, 3 - left)

int clamp (int v, int mn, int mx) {return v > mx ? mn : (v < mn ? mx : v);} // limit the value between minimum and maximum value

int cells; // size of the grid
int maxlen; // the limit of steps in the path
std::string downloadspath; // path to the downloads folder
std::string wininit; // initial key-obstacle destination
std::string init; // initial obstacles coordinates

// the position in the grid
struct pos {
    int x, y;

    pos() : x(-1), y(-1) {}
    pos(int x, int y) : x(x), y(y) {}
};

// an obstacle struct
struct obst {
    pos begin; // begin point
    pos end; // end point
    bool horizontal; // if obstacle horizontal or vertical
    bool iskey; // if the obstacle is key obstacle

    obst () : begin(pos()), end(pos()), horizontal(false), iskey(false) {}
    obst (int a, int b, int c, int d, bool key = false) : begin(pos(a, b)), end(pos(c, d)), iskey(key) {
        if (a == c) { // obstacle vertical if the begin x and the end x are the same 
            horizontal = false; 
            if (a > c) std::swap(begin, end); // swap the begin and end points if the end point is above the begin point
        }else { // otherwise the obstacle is always horizontal
            horizontal = true;
            if (b > d) std::swap(begin, end); // swap the begin and end points if the end point is to the left than the begin point
        }
    }
};

// struct for keeping the information about the obstacle move (used for the visualiser)
struct moved {
    int id; // the position of the obstacle in the array
    int dir; // what direction the obstacle was moved
    int len; // the length of the step

    moved() : id(-1), dir(-1), len(-1) {}
    moved(int id, int dir, int len) : id(id), dir(dir), len(len) {}

    std::string stringify() { return  std::to_string(id) + "~" + std::to_string(dir) + "~" + std::to_string(len) + ";"; } // convert the struct data to string
};

// struct for keeping the path to the unique grid states
struct state {
    state* prev; // pointer to the previous state
    moved mv; // information about obstacle that was moved to reach current state from previous - used to represent the sequence of moves for the third party visualiser (no use in algorithm)
    int len; // the amount of moves that has been taken to reach the current state from initial

    state () : prev(nullptr), len(0), mv(moved()) {}
    state (state* prev, int len, moved mv) : prev(prev), len(len), mv(mv) {}
    
    static std::string ltos (std::vector<obst>& arr) { // convert list of obstacles to key-string
        std::string res = "";

        for (obst o : arr)
            res += std::to_string(o.begin.x) + std::to_string(o.begin.y) + std::to_string(o.end.x) + std::to_string(o.end.y); 

        return res;
    }

    static std::vector<obst> stol (std::string s) { // convert key-string to list of obstacles
        std::vector<obst> res(s.size()/4, obst());

        for (int i = 0, j = 0; j < s.size() - 3; i++, j+=4)
            res[i] = obst(s[j] - 48, s[j+1] - 48, s[j+2] - 48, s[j+3] - 48, i==0);

        return res;
    }
};

// move the obstacle at arr[id] to dir (0 - up, 1 - right, 2 - down, 3 - left) by step
bool move (std::vector<obst>& arr, int id, int dir, int step = 1) {
    obst curr(arr[id]); // the obstacle which is supposed to be moved

    int diff; // the bias

    // check the grid boundaries ↓↓↓

    if (arr[id].horizontal) { // move horizontal obstacles
        if (dir % 2 == 0) return false; // restrict shifting horizontal obstacle along the Y axis
        diff = (dir == 1 ? 1 : -1)*step; // calculate the direction of the shift (1 - right, 3 - left)

        // shift
        curr.begin.x += diff;
        curr.end.x += diff;
    }else { // move vertical obstacles
        if (dir % 2 == 1) return false; // restrict shifting vertical obstacle along the X axis
        diff = (dir == 0 ? -1 : 1)*step; // calculate the direction of the shift (0 - up, 2 - down)

        // shift
        curr.begin.y += diff;
        curr.end.y += diff;
    }
    
    if (curr.begin.x >= cells || curr.begin.y >= cells || curr.begin.x < 0 || curr.begin.y < 0 || curr.end.x >= cells || curr.end.y >= cells || curr.end.x < 0 || curr.end.y < 0) return false;

    // check grid boundaries end ↑↑↑


    // search for collisions ↓↓↓

    for (int a = 0; a < arr.size(); a++) { // go through each obstacle and check if there's collision with the current one

        if (a == id) continue; // avoid searching for collisions with the obstacle that was moved

        obst* co = &arr[a]; // pointer to the obstacle being checked for collisions

        for (int i = co->begin.x; i <= co->end.x; i++) { // brute search for collisions
            for (int j = co->begin.y; j <= co->end.y; j++) {
                for (int k = curr.begin.x; k <= curr.end.x; k++) {
                    for (int n = curr.begin.y; n <= curr.end.y; n++) {
                        if (i == k && j == n) return false; // collision found - the obstacle can not be moved - return false
                    }
                }
            }
        }

    }

    // search for collisions end ↑↑↑

    arr[id] = obst(curr); // move the obstacle if there are no collisions or beyond boundaries 
    return true;          // and return true as sign that step has been taken
}

// check for the equality of the begin and end points of two obstacles 
bool operator == (obst a, obst b) { 
    return (a.begin.x == b.begin.x && a.end.x == b.end.x && a.begin.y == b.begin.y && a.end.y == b.end.y);
}

// dfs implemintation to search for the shortest path
void search (std::vector<obst> arr, state& res, obst winpos, int maxlen) {
    std::map<std::string, state> states; // paths to different states of the grid
    std::queue<std::vector<obst>> q; // queue
    std::set<std::string> checked; // the list of grid states that was visited before

    q.push(arr); // fill the queue 

    while (!q.empty()) {
        std::vector<obst> curr = q.front(); q.pop(); // get the current grid state
        
        std::string ckey = state::ltos(curr); // key of the current grid state for map
        state* cstate = &states[ckey]; // current state from map

        if (cstate->len > maxlen) continue; // stop a computation branch if it requires more steps than defined as the maximum number of steps (maxlen)

        if (curr[0] == winpos) { // if key obstacle has reached its destination
            std::cout << "The solution was found within " << cstate->len << " steps\n";
            if (res.len == -1 || res.len > cstate->len) res = state(*cstate); // replace existing result with current path if it's shorter
            return; // remove if you need to search for all solutions that take fewer steps than maxlen, otherwise only one solution will be found
        } else if (checked.find(ckey) == checked.end()) { // otherwise keep taking a steps
            checked.insert(ckey); // mark this state of grid as visited to avoid extra moves from this state

            for (int o = 0; o < curr.size(); o++) { // go through each obstacle 
                for (int dir = 0; dir < 4; dir++) { // try to take a step to the each direction
                    int step = 1; // define the length of the step

                    trymove:
                    std::vector<obst> temp(curr); // copy of the current grid state
                    if (move(temp, o, dir, step)) { // check if the step is possible
                        std::string nkey = state::ltos(temp); // define key for new grid state
                        if (states.find(nkey) == states.end()) states[nkey] = state(cstate, cstate->len+1, moved(o, dir, step)); // define this path to the new grid state as the shortest if there're no other paths 
                        else if (states[nkey].len > cstate->len+1) states[nkey] = state(cstate, cstate->len+1, moved(o, dir, step)); // otherwise define this path as the shortest if its shorter than existing
                        
                        q.push(temp); // append new state to the queue
                        step++; goto trymove; // try to make a longer step
                    }
                }
            }
        } 
    }
}

void readConfig() {
    std::ifstream cfg("config.ini");

    std::getline(cfg, init);
    cells = std::stoi(init);
    
    std::getline(cfg, init);
    maxlen = std::stoi(init);

    std::getline(cfg, downloadspath);
    std::getline(cfg, wininit);

    cfg.close();
}

int getInitData() {
    system("visualiser\\picker.html"); // open picker for creating an initial grid
    
    std::filesystem::path filepath = downloadspath + "\\HamsterKombatAssistantInitData.txt"; // path to the file which is downloading by visualiser

    while (!std::filesystem::exists(filepath)) continue; // wait for user to click the export button and download txt with the init data

    std::ifstream initfile(filepath); // open this file
    std::getline(initfile, init); // read init data
    initfile.close(); // close the file

    std::filesystem::remove(filepath); // remove file from downloads as it is a temporary file

    for (char c : init) if (!(c >= '0' && c <= '9')) return 1; // return true if data is invalid 
    return 0; // return false if data is right
}

void outputresults(std::vector<obst>& arr, state* res) {
    std::string output[res->len]; // string for keeping moves for the visualiser
    int i = res->len-1; 
    state* ptr = res; // current recovered path state

    while (ptr != nullptr && i >= 0) { // do until back to the first initial state
        output[i] = ptr->mv.stringify(); // insert moves for the visualiser
        ptr = ptr->prev; // get to the previous state
        i--;
    }

    // send data to visualiser and open it ↓↓↓
    std::ifstream source("visualiser\\code.js"); // open code.js
    std::ofstream temp; // future edited code.js
    temp.open("visualiser\\temp.js"); // create temp file

    std::string s; // string for read lines of the file
    
    std::getline(source, s); // remove two first
    std::getline(source, s); // lines of the file

    temp << "let init = " << "\"" << init << "\"" << std::endl;  // fill the first   
    temp << "let moves = " << "\"";                              // two lines with
    for (int i = 0; i < res->len; i++) temp << output[i];         // init data and
    temp << "\"" << std::endl;                                   // required moves

    while (std::getline(source, s)) temp << s << std::endl; // write other non-modified lines

    source.close();  // close
    temp.close();    // files

    const char* p = "visualiser\\code.js"; // pointer string of the path
    
    remove(p); // remove code.js
    rename("visualiser\\temp.js", p); // rename temp file into new code.js

    system("visualiser\\visualiser.html"); // launch up the visualiser
    // send data to visualiser and open it end ↑↑↑
}

int main () {  
    // input config data
    readConfig();

    // input initial data
    if (getInitData()) return 0; // stop program if input data is invalid

    std::cout << "Searching for solution up to " << maxlen << " steps" << std::endl;

    std::vector<obst> arr = state::stol(init); // convert input data to array

    state res(nullptr, -1, moved()); // container for result

    search(arr, res, state::stol(wininit)[0], maxlen); // launch up the programm

    if (res.len == -1) return 0; // if solution not found stop the program

    // output solution steps
    outputresults(arr, &res);
    
    return 0;
}


// print the grid to console
// void draw (std::vector<obst> arr) {
//     std::cout << "____________________\n"; // splitter
    
//     char grid[cells][cells]; // array of letters to be printed later

//     for (int i = 0; i < cells; i++) // fill the grid with empty cells
//         for (int j = 0; j < cells; j++) 
//             grid[i][j] = '.'; 
    
//     for (obst o : arr) { // fill the grid with the horizontal/vertical/key obstacles from array
//         char s = o.iskey ? '*' : (o.horizontal ? '=' : '|'); // [|] - horizontal cell, [=] - vertical cell, [*] - key cell 

//         for (int i = o.begin.x; i <= o.end.x; i++) {        // move from begin point to 
//             for (int j = o.begin.y; j <= o.end.y; j++) {    // the end point and fill the 
//                 grid[j][i] = s;                         // grid with the obstacle letters
//             }
//         }
//     }

//     for (int i = 0; i < cells; i++) { // print the grid to the console
//         for (int j = 0; j < cells; j++) 
//             std::cout << grid[i][j] << " ";
//         std::cout << std::endl;
//     }

//     std::cout << "\n____________________\n"; // splitter
// }