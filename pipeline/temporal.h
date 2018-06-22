#include <vector>
extern const int K;

using namespace std;

void get_input();
float box_muller_trans(float x);
int i_parent(int i);
int i_left_child(int i);
int i_right_child(int i);
void swap_element(vector<vector<int16_t>>* a, int b, int c);
void sift_down(vector<vector<int16_t>> *a, int start, int end);
void heapify(vector<vector<int16_t>>* a);
void sort_heap_last_element(vector<vector<int16_t>>* a);
void print_heap(vector<vector<int16_t>> heap, int x, int y);
void set_v_i(uint16_t x, uint16_t y, vector<int16_t>* c);
void clear_v_i(vector<int16_t>* c);
int16_t get_rand_x_y();
int16_t* get_rand_coord(int16_t* coord);