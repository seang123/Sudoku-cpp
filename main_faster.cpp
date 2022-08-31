#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "sudoku.h"
#include <iterator>
#include <chrono>
#include <stdexcept> // exceptions

/* Approach
 *
 *    1. Simple backtracker
 *    2. Rules
 *      - if a value already exists in a row/col/box remove it form the allowed_values set for that row/col/box
 *      - fill in any values which can only be in one place
 *      - no other value allowed according to allowed_values set
 *      - certain value allowed in no other square in the same box
 *      - certain value allowed in no other square in the same row/col
 *
 * 
 * Timings (for 1 puzzle)
 *    1. Initial implementation == 4000 ms (4sec) (-O2)
 *    2. Checking only modified row/col/box = 600 ms (-O2)
 *    3. (2) + Using single dynamic array for values[9] which gets overwritten each time = 657ms (-O2)
 *    4. Same as (2) but with -O3 = 570 ms
 *    5. Rule 1 (place value if its the only one allowed in the cell) = 60 ms (-O3)
 *    6. Rule 2.1 (check if a value isn't possible anywhere else in the row) = 30ms (-O3)
 *    
 * TODO:
 *    change allowed_values from unordered_map<int, vector> to array
 *
 */


/* 0  1  2  3  4  5  6  7  8
*  9 10 11 12 13 14 15 16 17
* 18 19 20 21 22 23 24 25 26
* 27 28 29 30 31 32 33 34 35
* 36 37 38 39 40 41 42 43 44
* 45 46 47 48 49 50 51 52 53
* 54 55 56 57 58 59 60 61 62
* 63 64 65 66 67 68 69 70 71
* 72 73 74 75 67 77 78 79 80
*/


// New allowed values array
// [[1,2,3,4,5,6,7,8,9], [0,2,0,0,5,6,0,8,0]]  - zero if a value isn't allowed, value otherwise



class Sudoku{
    public:
        int size = 81;
        std::vector<int> grid = std::vector<int>(81, 0); // board 
        std::vector<int> solution;
        //std::unordered_set <int> constant_idx{}; // store index of values that don't change
        int backtracks = 0;
        int counts = 0;

        int allowed_values[81][9];

        void solver(){
            // Read a board from the text file
            // solve it
            // store it 
        
        }

        void print(std::vector<int> grid){
            // Prints a 1D 9x9 sudoku grid
            for(int i = 0; i < 9; i++){
                for(int k = 0; k < 9; k++){
                    std::cout << grid[(i*9)+k] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "-------------------\n";
        };

        void print_grid(){
            std::cout << "--------------------\n";
            for(int i = 0; i < 9; i++){
                for(int k = 0; k < 9; k++){
                    if ((k % 3 == 0) & (k > 0))
                        std::cout << "|";
                    std::cout << grid[(i*9)+k] << " ";
                }
                std::cout << "\n";
                if ((i == 2) | (i == 5) | (i == 8))
                    std::cout << "--------------------\n";
            }
            //std::cout << "--------------------\n";
        };

        std::string solve(std::string grid_string){
            // solve a grid (given as string)
            for(int i = 0; i < 81; i++){
                grid[i] = grid_string[i] - '0';
                for(int k = 1; k<10; k++){
                    allowed_values[i][k-1] = k;
                }
            }
            

            // print board
            print_grid();
            std::cout << "\n\n";

            // Rule based solver
            smart_solve();

            // Print after applying rules
            /*if(!validate()){
                throw std::logic_error("board is not valid after smart solve");
            }*/

            // Brute-force solver
            backtrack();
            std::cout << "Backtracks: " << backtracks << "\n";

            std::stringstream ss;
            for(auto it = solution.begin(); it != solution.end(); it++){
                ss << *it;
            }
            return ss.str();
        }


    private:

        int idx_to_row(int i){
            return (i / 9);
        }
        int idx_to_col(int i){
            return (i % 9);
        }


        void clean_allowed_values_(int idx_){
            // Only clean row/col/box of current idx

            const int row_idx = idx_to_row(idx_);
            const int col_idx = idx_to_col(idx_);
            const int box_idx = BOXS[idx_];

            std::vector<int> idx_vals{};
            for(int i = 0; i < 81; i++){
                if( ROWS[i] == row_idx ){
                    idx_vals.push_back(i);
                }
                else if( COLS[i] == col_idx ){
                    idx_vals.push_back(i);
                }
                else if( BOXS[i] == box_idx ){
                    idx_vals.push_back(i);
                }
            }


            for(uint i = 0; i < idx_vals.size(); i++){
                int idx = idx_vals[i];
                int row_vals[9]{};
                int col_vals[9]{};
                int box_vals[9]{};

                if(grid[idx] == 0){
                    int row_idx = idx_to_row(idx);
                    int col_idx = idx_to_col(idx);
                    int box_idx = BOXS[idx];
                    for(int k=0;k<9;k++){
                        row_vals[k] = grid[(row_idx * 9) + k];
                    }
                    for(int i = 0; i < 9; i++ ){
                        col_vals[i] = grid[(i * 9) + col_idx];
                    }
                    int x = 0;
                    for(int i = 0; i<81; i++){
                        if(BOXS[i] == box_idx){
                            box_vals[x] = grid[i];
                            x++;
                        }
                    }

                    // Remove the values from allowed list
                    for(int i = 0; i < 9; i++){
                        const int row_value = row_vals[i];
                        const int col_value = col_vals[i];
                        const int box_value = box_vals[i];
                        if(row_value > 0)
                            allowed_values[idx][row_value-1] = 0;
                        if(col_value > 0)
                            allowed_values[idx][col_value-1] = 0;
                        if(box_value > 0)
                            allowed_values[idx][box_value-1] = 0;
                    }
                }
                else{
                    for(int i = 0; i < 9; i++){
                        allowed_values[idx][i] = 0;
                    }
                }
            }
            
        }

        void clean_allowed_values(){

            for(int idx = 0; idx<81; idx++){
                int row_vals[9]{}; // hold the values in the row of the current idx
                int col_vals[9]{}; 
                int box_vals[9]{};

                if(grid[idx] == 0){
                    const int row_idx = ROWS[idx];
                    const int col_idx = COLS[idx];
                    const int box_idx = BOXS[idx];

                    for(int i = 0; i < 9; i++){
                        row_vals[i] = grid[(row_idx * 9) + i];
                    }
                    for(int i = 0; i < 9; i++){
                        col_vals[i] = grid[(i * 9) + col_idx];
                    }
                    int x = 0;
                    for(int i = 0; i < 81; i++){
                        if(BOXS[i] == box_idx){
                            box_vals[i] = grid[i];
                            x++;
                        }
                    }
                    for(int i = 0; i < 9; i++){
                        const int row_value = row_vals[i];
                        const int col_value = col_vals[i];
                        const int box_value = box_vals[i];
                        if(row_value > 0)
                            allowed_values[idx][row_value-1] = 0;
                        if(col_value > 0)
                            allowed_values[idx][col_value-1] = 0;
                        if(box_value > 0)
                            allowed_values[idx][box_value-1] = 0;
                    }
                }
                else{
                    for(int i = 0; i < 9; i++){
                        allowed_values[idx][i] = 0;
                    }
                }
            }
        }


        bool check_box(int idx){
            // Check if a value isn't possibel anywhere else in the square
            
            int box_idx = BOXS[idx];

            int box_values[9]{};
            for(int i = 0; i < 81; i++){
                if(i == idx)
                    continue;
                if(BOXS[i] == box_idx){
                    for(int k = 0; k < 9; k++){
                        if(allowed_values[i][k] > 0){
                            box_values[k] = 1;
                        }
                    }
                }
            }
            for(int i = 0; i < 9; i++){
                if(box_values[i] == 0 & allowed_values[idx][i] > 0){
                    grid[idx] = i+1;
                    //std::cout << "value (" << i+1 << ") only possible at (" << idx << ")\n";
                    return true;
                }
            }
            return false;
        }

        bool check_col(int idx){
            // Check if a value isn't possible anywhere else in the col
            
            int row_idx = idx_to_row(idx);
            int col_idx = idx_to_col(idx);

            int col_values[9]{};
            for(int i = 0; i < 9; i++){
                if( i == row_idx ){
                    continue;
                }
                else{
                    int cell_idx = (i * 9) + col_idx;
                    for(int k = 0; k < 9; k++){
                        if(allowed_values[cell_idx][k] > 0){
                            col_values[k] = 1;
                        }
                    }
                }
            }
            for(int i = 0; i < 9; i++){
                if(col_values[i] == 0 & allowed_values[idx][i] > 0){
                    grid[idx] = i+1;
                    //std::cout << "value (" << i+1 << ") only possible at (" << idx << ")\n";
                    return true;
                }
            }

            return false;
        }

        bool in(int value, int idx){
            // check if value is present in allowed_values[idx]
            return (allowed_values[idx][value-1] > 0);
        }

        void print_av(int idx){
            for(int i = 0; i < 9; i++){
                std::cout << allowed_values[idx][i] << " ";
            }; std::cout << "\n";
        }

        bool check_row(int idx){
            // Check if a value isn't possible anywhere else in the row
            
            int row_idx = idx_to_row(idx);
            int col_idx = idx_to_col(idx);

            //std::vector<int> row_values;  // values possible in the row
            int row_values[9]{};
            for(int i = 0; i < 9; i++){
                if( i == col_idx ){
                    continue;
                }
                else{
                    int cell_idx = (row_idx * 9) + i;
                    for(int k = 0; k < 9; k++){
                        if(allowed_values[cell_idx][k] > 0){
                            row_values[k] = 1;
                        }
                    }
                }
            }
            for(int i = 0; i < 9; i++){
                if(row_values[i] == 0 & allowed_values[idx][i] > 0){
                    grid[idx] = i+1;
                    //std::cout << "value (" << i+1 << ") only possible at (" << idx << ")\n";
                    return true;
                }
            }
           return false;
        }

        int single_value(int idx){
            int count = 0;
            int value = 0;
            for(int i = 0; i < 9; i++){
                const int v = allowed_values[idx][i];
                if(v > 0){
                    value = v;
                    count++;
                }
            }
            if(count > 1)
                return 0;
            else
                return value;
        }

        void remove_all(int idx){
            for(int i = 0; i < 9; i++){
                allowed_values[idx][i] = 0;
            }
        }

        void smart_solve(){
            //std::cout << "Smart solve.\n";
            // Apply the rules until no change happens
            // If the board is complete exit, otherwise, backtrack

            int count{};
            int changes{};
            do{
                count++;
                changes = 0;

                for(uint idx = 0; idx < 81; idx++){
                    //clean_allowed_values();
                    clean_allowed_values_(idx);
                    int single_value_ = single_value(idx);  // check if only one value is avail.
                    /*for(int i = 0; i < 81; i++){
                        std::cout << i << ">";
                        for(int k = 0; k < 9; k++){
                            std::cout << allowed_values[i][k] << " ";
                        }; std::cout << "\n";
                        if(i % 9 == 0 & i != 0)
                            std::cout << "------------\n";
                    }
                    throw "Error";*/

                    if( grid[idx] != 0 ){
                        continue;
                    }
                    else if(single_value_ > 0){
                        // RULE 1 :: Add this value to the grid -- its the only possible value
                        grid[idx] = single_value_;
                        allowed_values[idx][single_value_-1] = 0;
                        //remove_all(idx);
                        changes++;
                        //std::cout << "filling in only allowed value " << single_value_ << "\n";
                    }
                    else if(check_row(idx)){
                        changes++;
                    }
                    else if(check_col(idx)){
                        changes++;
                    }
                    else if(check_box(idx)){
                        changes++;
                    }
                }
            }while(changes != 0);
            //std::cout << "Count: " << count << "\n";
        }

        int find_free(){
            for(int i=0;i<=80;i++){
                if(grid[i] == 0){
                    return i;
                }
            }
            return 81;
        }

        bool no_duplicates(int values[]){
            std::unordered_set <int> duplicates;
            for(int i=0;i<9;i++){
                int val = values[i];
                int x = duplicates.count(val);
                if( x == 0 ){
                    if( val != 0 ){
                        duplicates.insert(val);
                    }
                }
                else{
                    return false;
                }
            }
            return true;
        }

        bool is_row_valid(int idx){
            int row = idx_to_row(idx);
            // check just 1 row
            int values[9]{};
            for(int k=0;k<9;k++){
                values[k] = grid[(row * 9) + k];
                //*(values + k) = grid[(row * 9) + k];
            }
            return no_duplicates(values);
        }
        
        bool is_col_valid(int idx){
            int col = idx_to_col(idx);
            // check just 1 col
            int values[9]{};
            for(int i = 0; i < 9; i++ ){
                values[i] = grid[(i * 9) + col];
                //*(values + i) = grid[(i * 9) + col];
            }
            return no_duplicates(values);
        }

        bool is_box_valid(int idx){
            // check if 1 box is valid

            int values[9]{};
            int box_idx = BOXS[idx];
            int x = 0;

            for(int i = 0; i<81; i++){
                if(BOXS[i] == box_idx){
                    values[x] = grid[i];
                    //*(values + x) = grid[i];
                    x++;
                }
                if( x > 9){
                    break;
                }
            }
            return no_duplicates(values);
        }

        bool validate(){
            // Check if board is valid
            for(int i = 0; i < 81; i++){
                bool r = is_row_valid(i);
                bool c = is_col_valid(i);
                bool b = is_box_valid(i);
                if( !r | !c | !b )
                    return false;
            }
            return true;
        }

        bool is_valid(int idx){
            // Check rows/cols/boxes
            bool a = is_row_valid(idx);
            bool b = is_col_valid(idx);
            bool c = is_box_valid(idx);
            return (a & b & c) ? true : false;
        };


        void backtrack(){
            // Backtrack through the grid to solve it
            counts++;

            int free_ = find_free();
            if(free_ == 81){
                print_grid();
                solution = (grid);
            }
            else{
                for(int num = 1; num<=9; num++){
                    grid[free_] = num;
                    if(is_valid(free_)){
                        backtrack();
                    }
                    grid[free_] = 0;
                }
            }
            backtracks++;
        }


}; // Sudoku class -  end



void challenge(){
    
}


void test(int av[3][9]){

    av[2][2] = 8;

    for(int k = 0; k < 3; k++){
        for(int i = 0; i < 9; i++){
            std::cout << av[k][i] << " ";
        }; std::cout <<"\n";
    }

}


int main(){
    std::cout << "-- main --\n";


    //std::string test_pre = "000075400000000008080190000300001060000000034000068170204000603900000020530200000";
    std::string test = "000075400000000008080190000300001060000000034000068170204000603900000020530200000";
    std::string test_s = "693875412145632798782194356357421869816957234429368175274519683968743521531286947";

    //std::string test   = "300000000050703008000028070700000043000000000003904105400300800100040000968000200";
    //std::string test_s = "387419526259763418641528379716285943594631782823974165472396851135842697968157234";

    //std::string test =   "302609005500730000000000900000940000000000109000057060008500006000000003019082040";
    //std::string test_s = "382619475594738621176425938863941752457263189921857364738594216245176893619382547";

    /*
    std::string test =   "530000008007000030200006901000500200090370004000981000300040560000090000000007080";
    std::string test_s = "536419728917825436248736951781564293695372814423981675379148562852693147164257389";

    std::string test =   "008310900095000160000000005000400000000080049006072000000001030000240607001008200";
    std::string test_s = "468315972795824163132697485819453726257186349346972851924761538583249617671538294";
    */
    //std::string test =   "030605000600090002070100006090000000810050069000000080400003020900020005000908030";
    //std::string test_s=  "239645817641897352578132496796281543812354769354769281467513928983426175125978634";

    //std::string test =   "000000000000003085001020000000507000004000100090000000500000073002010000000040009"; // anti-backtracking board
    //std::string test_s=  "987654321246173985351928746128537694634892157795461832519286473472319568863745219";



    Sudoku s;
    //s.solve(test_pre);
    auto start = std::chrono::high_resolution_clock::now();
    std::string solution = s.solve(test);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    auto duration_milli = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    std::cout << "Time taken by function: " << duration_.count() << " microseconds" << "(" << duration_milli.count() << " ms)" << "\n";

    std::cout << "Solution: "<< solution << "\n";
    std::cout << "Target:   "<< test_s << "\n";
    std::cout << (solution == test_s) << "\n";

}
