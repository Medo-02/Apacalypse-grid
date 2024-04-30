/*
Author: Medo Alzowaidi
Date: 03/27/2023
Description: Zombie apacalypse. The program will simulate a zombie apacalypse first it will take height and width from the user
for the grid and couple of probabilities. After that the program(run_iteration) will do iterations showing the humans and zombies. at each iteration
the infected will convert to zombies(convert_zombie), and the zombies bite humans, and the zombies convert to humans(convert_infected).
After the iteration the number of zombies is shown with the new grid.
Usage: <exe> [-s seed] 
*/

#include <iostream>
#include <stdlib.h>
#include <stack>
#include <cstring>
#include "termcolor.hpp" 
//include the termcolor.hpp file for outputing the map with colors
using namespace std;
//functions for running simulation
void print_map(char** , int, int);//print the grid of map
int run_iteration(int**, char**, float**, int, int, float, float);//convert the status and bite action
bool is_berserker(float);//check if zombie is berserker
int valid_bite(char**, int, int, int, int, int, int);//check if it is possible to bite
int bite(float**, int, float, int, int);//bite attempt 
void convert_infected(int**, char**, float**, int, int, float);//convert the infecteds to zombie
void convert_zombie(int**, char**, int, int);//convert the zombie to human

int main(int argc, char** argv)
{       
        //portion for selecting seed number
        if(argc==3 && strcmp(argv[1], "-s")==0){//check the parameters
                for (int i = 0; i<strlen(argv[2]); i++){
                        if(!(isdigit(argv[2][i]))){
                                cout << "Usage: <exe> [-s seed]";
                                return 0;
                        }
                }
        }else if(argc!=1){
                cout << "Usage: <exe> [-s seed]";
                return 0;
        }else
                srand(time(0));
        //initialize the variables
        int width;
        int height;
        float p_bite;
        float p_berserker;
        //Assigning the inputs
        do{
                cout << "Enter map width (between 4 and 40): "; 
                cin >> width;
                if(width<4 || width>40){//if thh number out of bonds prompt error and exit
                        cout<<"Error: Out of range number"<<endl;
                        continue;
                }
                cout << "Enter map height (between 4 and 40): "; 
                cin >> height;
                if(height<4 || height>40){
                        cout<<"Error: Out of range number"<<endl;
                        continue;
                }
                cout << "Enter zombie bite accurcy (between float 0 and 1): "; 
                cin >> p_bite;
                if(p_bite<0 || p_bite>1){
                        cout<<"Error: Out of range number"<<endl;
                        continue;
                }
                cout << "Enter berserker probability(between float 0 and 1): "; 
                cin >> p_berserker;
                if(p_berserker<0 || p_berserker>1){
                        cout<<"Error: Out of range number"<<endl;
                        continue;
                } 
                break;
        }while(true);

        //generate a random number
        int random_w = rand() % width;
        int random_h = rand() % height;
        //create 2D arrays   
        char** array_states = new char*[height];
        int** array_zombie = new int*[height];
        float** bite_attempts = new float*[height];
        //fill the arrays
        for(int i = 0; i < height; i++){
                array_zombie[i] = new int[width];
                array_states[i] = new char[width];
                bite_attempts[i] = new float[width];
                for(int j=0; j<width; j++){
                        array_states[i][j]='.';
                        array_zombie[i][j]=0;
                        bite_attempts[i][j]=0.0;
                }
        }
        //create the first zombie
        array_zombie[random_h][random_w] = 5;
        array_states[random_h][random_w] = '#';
        cout << endl;
        cout << "Strain dropped at coordinate (" << random_h << ", " << random_w <<"). Zombie of strength 5 spawned.\n" <<endl;
        //run the iterations
        for(int i=0; i<4; i++){
                cout<< "Iteration " << i << endl;
                if(i==0){//for the zero iteration, the is no iteration run
                        cout << "Number of zombies in this iteration is 1 \n";
                }else{
                        cout << "Number of zombies in this iteration is " << run_iteration(array_zombie, array_states, bite_attempts, width, height, p_bite, p_berserker) <<endl;
                }
                print_map(array_states, width, height);//print the grid
        }

        for(int i=0; i<height; i++){
                        delete [] array_states[i];
                        delete [] array_zombie[i];
                        delete [] bite_attempts[i];
        }
        delete [] array_states;
        delete [] array_zombie;
        delete [] bite_attempts;

}
//this function print each element from the status to a grid
void print_map(char** array_states, int width, int height){
        for(int i=0; i<height; i++){
                for(int j=0; j<width; j++){
                        if(array_states[i][j]=='*'){
                                cout << termcolor::green << array_states[i][j] << termcolor::reset <<"  ";
                        }else if(array_states[i][j]=='#'){
                                cout << termcolor::red << array_states[i][j] << termcolor::reset <<"  ";
                        }else if (array_states[i][j]=='0'){
                                cout << termcolor::magenta << array_states[i][j] << termcolor::reset <<"  ";
                        }else
                                                                cout << termcolor::bright_grey << array_states[i][j] << termcolor::reset <<"  ";
                }
                cout<<endl;
        }
        cout<<endl;
}
//convert all the infected zombies, and human and do bite action
int run_iteration(int** array_zombie, char** array_states, float** bite_attempts, int width, int height, float p_of_bite, float p_of_berserk){
        int direction_x;
        int direction_y;
        int berserker_count = 0;
        int zombies_count = 0;
        convert_zombie(array_zombie, array_states, width, height);//convert all the zombies to human
        convert_infected(array_zombie, array_states, bite_attempts, width, height, p_of_berserk);//convert all the infecteds to zombies

        //Do the bite action and infect human
        for(int i = 0; i < height; i++){
                int j = 0;
                do{//loop each status until the zombie find a valid bite position, if position isn't found go to next status
                        //generate random coordinates
                        direction_x = ((rand() %3)-1)+i;
                        direction_y = ((rand() %3)-1)+j;
                        if (array_states[i][j] == '#'){//in case of the zombie status check if the bite coordinate is valid and the attempt bite action
                                if(valid_bite(array_states, width, height, direction_x, direction_y, i, j)==1){
                                        if(bite(bite_attempts, array_zombie[i][j], p_of_bite, direction_x, direction_y)!=0){
                                                array_states[direction_x][direction_y]='*';
                                        }
                                        j++;
                                }else if(valid_bite(array_states, width, height, direction_x, direction_y, i, j)==-1){//if there is no coordinate available for bite
                                        j++;
                                }
                        }else if (array_states[i][j] == '0'){//in case of berserker check for valid bite position if found do bite attempt

                                if(valid_bite(array_states, width, height, direction_x, direction_y, i, j)==1){
                                        if(bite(bite_attempts, array_zombie[i][j], p_of_bite, direction_x, direction_y)!=0){
                                                array_states[direction_x][direction_y]='*';
                                        }
                                        berserker_count++;
                                }else if (valid_bite(array_states, width, height, direction_x, direction_y, i, j)==-1){//if no position were available for bite go to next status 
                                        j++;
                                        berserker_count=0;
                                }
                                if (berserker_count == 3){//if we have reached 3 bite attempts go to next status
                                        j++;
                                        berserker_count=0;
                                }
                        }else {
                                j++;
                        }
                }while(j!= width);
        }
        for (int i = 0; i < height; i++){//count the number of zombies in the grid
                for (int j = 0; j < width; j++)
                        if(array_states[i][j] == '0' || array_states[i][j] == '#'){zombies_count++;}
        }
        
        return zombies_count;
}
//increase the strength for the zombies in the loop and if the strength is 10 return zombie to human
void convert_zombie(int** array_zombie, char** array_states, int width, int height){
        for(int i=0; i<height; i++){
                for(int j=0; j<width; j++){
                        if(array_states[i][j]=='#' || array_states[i][j]=='0'){//check if the status is zombie, then increase strength
                                array_zombie[i][j]+=1;
                        }
                        if(array_zombie[i][j]==10){//convert to human when the strength in 10
                                array_zombie[i][j]=0;
                                array_states[i][j]='.';
                        }
                }

        }
}
//convert the infected to zombie and give them strength
void convert_infected(int** array_zombie, char** array_states, float** bite_attempts, int width, int height, float p_of_berserk){
        int strength;

        for(int i=0; i<height; i++){//loop through the status and determine the strength of bite attempt then assign it to zombie
                for(int j=0; j<width; j++)
                        if(array_states[i][j]=='*'){
                                strength = int(bite_attempts[i][j]*10);
                                if(is_berserker(p_of_berserk)){
                                        array_zombie[i][j]=strength;
                                        array_states[i][j]='0';
                                }else{
                                        array_zombie[i][j]=strength;
                                        array_states[i][j]='#';
                                }

                        }
        }
}
//check if the zombie is berserker 
bool is_berserker(float p_of_berserk){
        float berserk_attempt = (float)rand() / (float)RAND_MAX;
        if(berserk_attempt <= p_of_berserk){return false;}
        return true;
}

//check if the bite attempt is valid
int valid_bite(char** array_status, int width, int height, int neighbor_x, int neighbor_y, int row, int column){
                //check if the zombie is surrounded by any human
                bool space_condition = false;
                if(((row+1)==height || (row-1) < 0)){//check next x and back x coordinate
                        if(row+1==height){
                                if(array_status[row-1][column] == '.'){space_condition = true;}
                        }else{
                                if(array_status[row+1][column] == '.'){space_condition = true;}
                        }
                }else{
                        if(array_status[row+1][column] == '.' || array_status[row-1][column] == '.'){
                                space_condition = true;
                        }
                }
                if(((column+1)==width || (column-1) < 0)){//check they next and y back coordinate
                        if(column+1==width){
                                if(array_status[row][column-1] == '.'){space_condition = true;}
                        }else{
                                if(array_status[row][column+1] == '.'){space_condition = true;}
                        }
                } else{
                        if(array_status[row][column+1] == '.' || array_status[row][column-1] == '.'){
                              space_condition = true;  
                        }
                }
        if(space_condition){//if the zombie is surrounded by atleast one human check if the random coordinate is correct
                        if((neighbor_x<height && neighbor_x>=0) && (neighbor_y<width && neighbor_y>=0)){//check if the neighbour not out of bonds
                                if((array_status[neighbor_x][neighbor_y] == '.') && ((abs(neighbor_x-row) + abs(neighbor_y-column)) == 1)){//check if the position of random coordinate is right next to the zombie
                                        return 1;
                                }
                        }
                        return 0;
        }else{
                return -1;// return -1 if there the zombie is not surroudned by any zombie
        }
}

//generate random bite attmept and check if it successful or not
int bite(float** bite_attempts, int zombie_strength, float p_of_bite, int row, int column){
        float successful_bite = zombie_strength*p_of_bite/10;
        float bite_attempt = (float)rand() / (float)RAND_MAX;
        if(bite_attempt-successful_bite>=0){
                bite_attempts[row][column]=bite_attempt;
                return 1;
        }
        return 0;

}

