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


using namespace std;
using namespace Halide; 
using namespace Halide::ConciseCasts;

extern const int K = 11;
const int n_frames = 5;



int width;
int height;
int n_channels;
Buffer<uint8_t> input[n_frames];
multimap<vector<int>, vector<int>> coord_m;

Buffer<short> pix(1, 1, 1, 1, 1); //x, y, x_i, y_i, c
Buffer<short> b(width);


Var x, y, x_i, y_i, c, i, xo, yo, xi, yi;
class NeighborPipeline {
public: 
	Func D, I;
	Buffer<uint8_t> input;
	const int s = 10;
	
	NeighborPipeline(Buffer<uint8_t> in) : input(in) {
		RDom u(-s, s, -s, s);

		I(x, y, c) = input(clamp(x, s, width - s), clamp(y, s, height - s), c);	
		D(x, y, x_i, y_i, c) = i16(sum(pow((I(x + u.x, y + u.y, c) - I(x_i + u.x, y_i + u.y, c)), 2)));
	} 

	void schedule_for_gpu() {
		Var block, thread;

		// D.split(i, block, thread, 16);
		//D.gpu_blocks(block)
			// .gpu_threads(thread);
		
		D.reorder(c, x_i, y_i, x, y).bound(c, 0, 3).unroll(c);
		D.gpu_tile(x, y, xo, yo, xi, yi, 32, 32);


	    // Start with a target suitable for the machine you're running
	    // this on.
	    Target target = get_host_target();

	    // Then enable OpenCL or Metal, depending on which platform
	    // we're on. OS X doesn't update its OpenCL drivers, so they
	    // tend to be broken. CUDA would also be a fine choice on
	    // machines with NVidia GPUs.
	    if (target.os == Target::OSX) {
	        target.set_feature(Target::Metal);
	    } else {
	        target.set_feature(Target::OpenCL);
	    }

	    D.compile_jit(target);
	}
};

int main(int argc, char** argv) {
	
	clock_t t = clock();
	get_input();
	NeighborPipeline p1(input[0]);
	// p1.schedule_for_gpu();
	
	b = init_neighbors(p1);
	cout<<"b(10, 10, -506, 36, 0) "<<b(10, 10, -506, 36, 0)<<endl;

	for(int j = 1; j < height; j++) {
		for(int i = 1; i < width; i++) {

			get_buffer_values(b, i, j);

		}
	}
	t = clock() - t;
	cout<<(float)t/CLOCKS_PER_SEC<<" seconds";
}


void get_buffer_values(Buffer<short> b, int i, int j) {
	auto range = coord_m.equal_range({i, j}); //get all neighbor coordinates of i, j
	
	for(auto k = range.first; k != range.second; ++k) {
		if(i == 10 && j == 10) {
			cout<<"printing at get_buffer_values()"<<endl;
			print_buffer(b, i, j, k->second.at(0), k->second.at(1), 0);
		}
	}
}

Buffer<short> init_neighbors(NeighborPipeline p) {
	srand(17);
	Buffer<short> b(width);
	vector<short> v_i;
	ushort coord[2];
	vector<vector<short>> neighbors;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			for(int i = 0; i < K; i++) {
				set_v_i(p, x, y, &v_i);
				neighbors.push_back(v_i);
				clear_v_i(&v_i);
			}
			
			sort_heap_last_element(&neighbors);	
			// if(x<3 && y<3) print_heap(neighbors, x, y);
			fill_buffer(b, x, y, neighbors);
			clear_neighbors(&neighbors);
				
		}
	}
	cout<<"at init_neighbors()"<<endl;
	print_buffer(b, 10, 10, -506, 36, 0);
	return b;
}


void clear_neighbors(vector<vector<short>>* neighbors) {
	for(int i = 0; i < K; i++) {
		neighbors->pop_back();
	}
	// cout<<"size "<<neighbors->size()<<endl;
}

void fill_buffer(Buffer<short> b, short x, short y, vector<vector<short>> neighbors) {
	
	for(int i = 0; i < K; i++) {
		short x_i = neighbors.at(i).at(0);
		short y_i = neighbors.at(i).at(1);
		short d = neighbors.at(i).at(2);
		// if(x_i > width) cout<<"x_i > width "<<x_i<<endl;
		// if(y_i > height) cout<<"y_i > width "<<y_i<<endl;
		b(x, y, x_i, y_i, 0) = d; //x, y, x_i, y_i, channels.  
		vector<int> c = {x, y};
		vector<int> c_i = {x_i, y_i};
		coord_m.insert(pair <vector<int>, vector<int>> (c, c_i));
		if(x == 10 && y == 10) {
			cout<<"printing at fill_buffer()"<<endl;
			print_buffer(b, x, y, x_i, y_i, 0);
			
		}
			
	}

}

short* get_rand_coord(short* coord) {
	
	coord[0] = get_rand_x();
	coord[1] = get_rand_y();
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


void set_v_i(NeighborPipeline p, ushort x, ushort y, vector<short>* v_i) {
	
	short coord[2];
	get_rand_coord(coord);
	// cout<<"Printing coordinate before realizing"<<endl;
	// print_coord(coord);
	pix.set_min(x, y, x + coord[0], y + coord[1], 0);	
	cout<<"x "<<x<<" y "<<y<<" x+coord[0] "<<x+coord[0]<<" y+coord[1] "<<y+coord[1]<<endl;
	if(have_opencl_or_metal()) {
		cout<<"not coming here"<<endl;
		
		p.D.realize(pix);
	} else {
		cout<<"not found"<<endl;
	}
	

	
	int D_i = pix(x, y, x + coord[0], y + coord[1], 0);
	// cout<<D_i<<endl;

	v_i->push_back(coord[0]); //x
	v_i->push_back(coord[1]); //y
	v_i->push_back(D_i);
	// cout<<"Printing coordinate after realizing and pushing"<<endl;
	// print_coord(coord);

}

void clear_v_i(vector<short>* v_i) {
	v_i->pop_back();
	v_i->pop_back();
	v_i->pop_back();
}

// converts a uniform random variable into a standard normal variable
float box_muller_trans(float x) {
	return sqrt(-2 * log(x)) * cos(2 * M_PI * x);;
}

void get_input() {

	string path;

	for(int i = 0; i < n_frames; i++) {

		path = "./frames/f_" + to_string(i + 1) + ".png";
		input[i] = Tools::load_image(path);	

	}

	width = input[0].width();
	height = input[0].height();
	n_channels = input[0].channels();
	
	// sig_s = width / 3;
	
	// D.trace_stores();

}

void print_buffer(Buffer<short> b, int x, int y, int x_i, int y_i, int c) {
	cout<<"b("<<x<<", "<<y<<", "<<x_i<<", "<<y_i<<", "<<c<<")="<< b(x, y, x_i, y_i, c)<<endl;
}






bool have_opencl_or_metal();



#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif



// A helper function to check if OpenCL seems to exist on this machine.

bool have_opencl_or_metal() {
#ifdef _WIN32
    return LoadLibrary("OpenCL.dll") != NULL;
#elif __APPLE__
    return dlopen("/System/Library/Frameworks/Metal.framework/Versions/Current/Metal", RTLD_LAZY) != NULL;
#else
    return dlopen("libOpenCL.so", RTLD_LAZY) != NULL;
#endif
}