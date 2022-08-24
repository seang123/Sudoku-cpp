#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "sudoku.h"
#include <iterator>
#include <chrono>

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
 *    
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


class Sudoku{
    public:
        int size = 81;
        std::vector<int> grid = std::vector<int>(81, 0); // board 
        //std::unordered_set <int> constant_idx{}; // store index of values that don't change
        int backtracks = 0;
        int counts = 0;

        std::unordered_map <int, std::vector<int> > allowed_values; // map{ idx->allowed_values }

        void solver(){
            // Read a board from the text file
            // solve it
            // store it 
        }

        void print(std::vector<int> grid){
            for(int i = 0; i < 9; i++){
                for(int k = 0; k < 9; k++){
                    std::cout << grid[(i*9)+k] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "-------------------------\n";
        };

        void print_grid(){
            for(int i = 0; i < 9; i++){
                for(int k = 0; k < 9; k++){
                    std::cout << grid[(i*9)+k] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "-------------------------\n";
        };

        std::string solve(std::string grid_string){
            // solve a grid (given as string)
            for(int i = 0; i < 81; i++){
                grid[i] = grid_string[i] - '0';
                std::vector<int> av {1, 2, 3, 4, 5, 6, 7, 8, 9}; 
                allowed_values[i] = av;
                //allowed_values[i] = remove(av, grid_string[i]-'0');
            }
            

            // print board
            print_grid();
            std::cout << "\n\n";

            // Rule based solver
            smart_solve();

            // Print after applying rules
            print_grid();
            std::cout << "\n\n";
            //throw "Error";

            // Brute-force solver
            backtrack();
            std::cout << "Backtracks: " << backtracks << "\n";

            std::string solution = "";
            return solution;
        }


    private:

        int idx_to_row(int i){
            return (i / 9);
        }
        int idx_to_col(int i){
            return (i % 9);
        }

        std::vector<int> remove(std::vector<int> x, int val){
            // Remove the first instance of val from vector x
            std::vector<int>::iterator position = std::find(x.begin(), x.end(), val);
            if(position != x.end())
                x.erase(position);
            return x;
        }

        void clean_allowed_values(){
            // Remove elements from av which can't be in that idx position
            
            // Loop through cells
            // remove row/col/box elements from allowed_values(cell) 
            
            for(int idx = 0; idx<81; idx++){
                int row_vals[9]{};
                int col_vals[9]{};
                int box_vals[9]{};

                if(grid[idx] == 0){
                    int row_idx = idx_to_row(idx);
                    int col_idx = idx_to_col(idx);
                    int box_idx = box[idx];
                    for(int k=0;k<9;k++){
                        row_vals[k] = grid[(row_idx * 9) + k];
                    }
                    for(int i = 0; i < 9; i++ ){
                        col_vals[i] = grid[(i * 9) + col_idx];
                    }
                    int x = 0;
                    for(int i = 0; i<81; i++){
                        if(box[i] == box_idx){
                            box_vals[x] = grid[i];
                            x++;
                        }
                    }

                    // Remove the values from allowed list
                    for(int i = 0; i<9; i++){
                        allowed_values[idx] = remove(allowed_values[idx], row_vals[i]);
                        allowed_values[idx] = remove(allowed_values[idx], col_vals[i]);
                        allowed_values[idx] = remove(allowed_values[idx], box_vals[i]);
                    }
                }
                else{
                    // Grid has a value in it so remove all allowed values for that idx
                    for(int i = 0; i<9; i++){
                        allowed_values[idx] = std::vector<int>{};
                    }
                }

            }
        }

        void smart_solve(){
            std::cout << "Smart solve.\n";
            // Apply the rules until no change happens
            // If the board is complete exit, otherwise, backtrack


            int count{};
            int changes{};
            do{
                count++;
                changes = 0;
                clean_allowed_values();
                // Print allowed values for each cell
                for(int idx = 0; idx < allowed_values.size(); idx++){
                    std::cout << "idx: " << idx << " -- ";
                    for(int i = 0; i<allowed_values[idx].size(); i++){
                        std::cout << allowed_values[idx][i] << " ";
                    }
                    std::cout << "\n";
                }
                for(int idx = 0; idx < allowed_values.size(); idx++){
                    if(allowed_values[idx].size() == 1){
                        // RULE 1 :: Add this value to the grid -- its the only possible value
                        grid[idx] = allowed_values[idx][0];
                        changes++;
                        std::cout << "Adding only possible value (" << allowed_values[idx][0] << ") to grid @ " << idx << "\n";
                        allowed_values[idx] = std::vector<int>{};
                    }
                }
                //if(count > 1){break;}
                std::cout << "Changes: "<< changes << "\n";
            }
            while(changes != 0);
            std::cout << "Count: " << count << "\n";

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
            int box_idx = box[idx];
            int x = 0;

            for(int i = 0; i<81; i++){
                if(box[i] == box_idx){
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

        bool are_rows_valid(){
            int values[9]{}; 
            int idx = 0;
            int x = 0;
            while (x <= 80){
                for( int j=0;j<9;j++){
                    values[idx] = grid[x+j];
                    //*(values + idx) = grid[x+j];
                    idx++;
                }
                if(!no_duplicates(values)){
                    return false;
                }
                idx = 0;
                x = x + 9;
            }
            return true;
        }

        bool are_cols_valid(){
            int values[9]{};
            int idx = 0;
            int x = 0;
            while( x < 9 ){
                for( int i = 0; i < 9; i++ ){
                    values[idx] = grid[i * 9 + x];
                    //*(values + idx) = grid[i * 9 + x];
                    idx++;
                }
                if(no_duplicates(values) == false){
                    return false;
                }
                idx = 0;
                x = x + 1;
            }
            return true;
        }

        bool are_boxes_valid(){
            int values[9]{};
            int idx = 0;
            for( int i = 0; i < 9; i = i + 3){
                for(int k = 0; k < 9; k = k + 3){
                    for(int x = 0; x < 3; x++){
                        for(int y = 0; y < 3; y++){
                            values[idx] = grid[(i + x) * 9 + (k + y)];
                            //*(values + idx) = grid[(i + x) * 9 + (k + y)];
                            idx++;
                        }
                    }
                    if(no_duplicates(values) == false){
                        return false;
                    }
                    idx = 0;
                }
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




int main(){
    std::cout << "-- main --\n";

    std::string test1 = "000075400000000008080190000300001060000000034000068170204000603900000020530200000";
    std::string test1_s = "693875412145632798782194356357421869816957234429368175274519683968743521531286947";

    std::string test2   = "300000000050703008000028070700000043000000000003904105400300800100040000968000200";
    std::string test2_s = "387419526259763418641528379716285943594631782823974165472396851135842697968157234";


    Sudoku s;
    auto start = std::chrono::high_resolution_clock::now();
    s.solve(test1);
    // s.solve(test2);
    //s.test_checks(test1_s);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    auto duration_milli = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    std::cout << "Time taken by function: " << duration_.count() << " microseconds" << "(" << duration_milli.count() << " ms)" << "\n";

}
