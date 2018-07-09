#include <stdio.h>
#include <string>
#include <iostream>
#include <random>
#include <math.h>
#include <algorithm>
#include <vector>
#include <list>
#include <time.h>
#include "Halide.h"
#include "halide_image_io.h"
#include "temporal.h"
#include <map>
#include <thread>

using namespace std;
using namespace Halide;
using namespace Halide::ConciseCasts;

extern const int K = 10;
const int n_frames = 5;

Func D, I;

int width = 100;
int height = 100;
int n_channels = 3;
Buffer<uint8_t> input[n_frames];
map<vector<short>, vector<short>> neighbor_map;


Buffer<short> neighbor_b(width, height, 2000, 2000, n_channels);


Var x, y, x_i, y_i, c, i, xo, yo, xi, yi;


int main(int argc, char** argv) {

    //neighbor_b.set_min(0, 0, -1000, -1000, 0);
    clock_t t = clock();
    get_input();
    
    init_neighbors();
    print_buffer(99, 99, 50, -28, 0);
    cout<<"Main"<<endl;

//    for(short j = 1; j < 5; j++) {
//        for(short i = 1; i < 5; i++) {
//
            print_neighbors(9, 10);

//        }
//    }
//


    t = clock() - t;
    cout<<(float)t/CLOCKS_PER_SEC<<" seconds"<<endl;
}


void print_neighbors(short x, short y) {
    map<vector<short>, vector<short>>::iterator it = neighbor_map.begin();   
    for(it = neighbor_map.begin(); it != neighbor_map.end(); ++it) {
        cout<<"x "<<it->first.at(0)<<" y "<<it->first.at(1)<<" k "<<it->first.at(2)<<" x_i "<<it->first.at(2)<<" y_i "<<it->first.at(2)<<endl;
  
 
        //print_buffer(it->first.at(0), it->first.at(1), it->second.at(0), it->second.at(1), 0);
    }
    cout<<"printing at get_buffer_values()"<<endl;
   


}

void init_neighbors() {
    srand(17);
        for(short y = 0; y < height; y++) {
        for(short x = 0; x < width; x++) {
            vector<vector<short>> unsorted_neighbors; 
            //vector<vector<short>> sorted_neighbors; 

            //get coordinates and ssd of each neighbor
            for(short i = 0; i < K; i++) {
                vector<short> v_i_ssd = get_neighbor_ssd(x, y);
                unsorted_neighbors.push_back(v_i_ssd);
            }

            //sorted_neighbors = sort_neighbors(unsorted_neighbors);   
            fill_buffer(x, y, unsorted_neighbors);
                         
        }
       
    }
}


vector<vector<short>> sort_neighbors(vector<vector<short>> neighbors) {
    sort_heap_last_element(&neighbors);
    return neighbors;
}

//static int count_ = 0;
void fill_buffer(short x, short y, vector<vector<short>> neighbors) {

    for(short i = 0; i < K; i++) {
        assert(typeid(neighbors.at(i).at(0)) == typeid(short));
        short x_i = neighbors.at(i).at(0);
        short y_i = neighbors.at(i).at(1);
        short d = neighbors.at(i).at(2);
        vector<short> c = {x, y, i};
        vector<short> c_i = {x_i, y_i};
        //neighbor_map.insert(pair <vector<short>, vector<short>> (c, c_i));


	    // I'm using memory that I don't have permission to access. 
        neighbor_b(x, y, x_i, y_i, 0) = d; //x, y, x_i, y_i, channels. 
        //if(x == 1 && y == 1) {
     //       cout<<"In the loop"<<endl;
     //       print_buffer(x, y, x_i, y_i, 0);
     //   //}

//        if(!neighbor_b.contains(x, y, x_i, y_i, 0) && count_ < 100) {
//        	print_buffer(x, y, x_i, y_i, 0);
//        	count_++;
//        }
//


    }

    //short temp1 = neighbors.at(2).at(0);
    //short temp2 = neighbors.at(2).at(1);

   

    // if(x == 1 && y == 1) {
    //     cout<<"printing at fill_buffer()"<<endl;
    //     print_buffer_values(x, y);
    // }
}


vector<short> get_rand_coord() {
    vector<short> coord;
    coord.push_back(get_rand_x());
    coord.push_back(get_rand_y());
    //coord[1] = get_rand_y();
    // print_coord(coord);
    return coord;
}



short get_rand_x() {
    float rand_x;
    while(true) {
       
        rand_x = width / 3 * box_muller_trans((float) rand() / RAND_MAX);   
        if(rand_x < width) break;
    }
   
    return (short) rand_x;
}

short get_rand_y() {
    float rand_y;
        while(true) {
           
            rand_y = height / 3 * box_muller_trans((float) rand() / RAND_MAX);   
            if(rand_y < height) break;
        }
       
        return (short) rand_y;
}


//Buffer<short> pix(1, 1, 1, 1, 3);
vector<short> get_neighbor_ssd(short x, short y) {

    Buffer<short> pix(1, 1, 1, 1, 3);

    //short coord[2];
    vector<short> coord = get_rand_coord();
    // cout<<"Printing coordinate before realizing"<<endl;
    // print_coord(coord);
   
       
    // cout<<"x "<<x<<" y "<<y<<" x+coord[0] "<<x+coord[0]<<" y+coord[1] "<<y+coord[1]<<endl;

    pix.set_min(x, y, x + coord[0], y + coord[1]);   
    D.realize(pix);
  
    short d_i = pix(x, y, x + coord[0], y + coord[1], 0);
    // cout<<D_i<<endl;
    coord.push_back(d_i); //pushing ssd
    
    // cout<<"Printing coordinate after realizing and pushing"<<endl;
    // print_coord(coord);
    return coord;

}



// converts a uniform random variable into a standard normal variable
float box_muller_trans(float x) {
    return sqrt(-2 * log(x)) * cos(2 * M_PI * x);;
}

void get_input() {

    string path;

    for(int i = 0; i < n_frames; i++) {

        path = "./frames/f_" + to_string(i + 1) + ".jpg";
        input[i] = Tools::load_image(path);   

    }

    //width = input[0].width();
    //height = input[0].height();
    //n_channels = input[0].channels();
   
    // sig_s = width / 3;



    const int s = 10;
   
    cout<<"s="<<s<<endl;
    RDom u(-s, s, -s, s);

    I(x, y, c) = input[0](clamp(x, s, width - s), clamp(y, s, height - s), c);   
    D(x, y, x_i, y_i, c) = i16(sum(pow((I(x + u.x, y + u.y, c) - I(x_i + u.x, y_i + u.y, c)), 2)));

   


}

void print_buffer(short x, short y, short x_i, short y_i, short c) {
    cout<<"neighbor_b("<<x<<", "<<y<<", "<<x_i<<", "<<y_i<<", "<<c<<")="<< neighbor_b(x, y, x_i, y_i, c)<<endl;
}
