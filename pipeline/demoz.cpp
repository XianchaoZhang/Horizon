#include "Halide.h" 
#include<stdio.h>
#include<stdlib.h>
#include "halide_image_io.h"
#define OBR_HEIGHT 30
using namespace Halide;

// Color channels: 0-red, 1-green, 2-blue

/* Bayer filter: 
    
    b g
    g r

*/ 


/*

http://www.arl.army.mil/arlreports/2010/arl-tr-5061.pdf


*/ 

int main(int argc, char **argv) {
    

    Buffer<uint8_t> input = Tools::load_image(argv[1]);
    
    RDom r(2, input.width() - 4, 2, input.height() - 4);
    r.where((r.x % 2 == 0 && r.y % 2 == 0) || (r.x % 2 == 1 && r.y % 2 == 1));
    
    Var x("x"), y("y");
    
    Func g_n, g_e, g_s, g_w, w_n, w_e, w_s, w_w, g_n_est, g_e_est, g_s_est, g_w_est, green("green"), clamped;

    g_n(x, y) = cast<float> (0);
    /*g_e(x, y) = cast<uint8_t> (0);
    g_s(x, y) = cast<uint8_t> (0);
    g_w(x, y) = cast<uint8_t> (0);
 */
    w_n(x, y) = cast<float> (0);
   /* w_e(x, y) = cast<uint8_t> (0);
    w_s(x, y) = cast<uint8_t> (0);
    w_w(x, y) = cast<uint8_t> (0);
    */
    g_n_est(x, y) = cast<float> (0);
    /*g_e_est(x, y) = cast<uint8_t> (0);
    g_s_est(x, y) = cast<uint8_t> (0);
    g_w_est(x, y) = cast<uint8_t> (0);
*/
    green(x, y) = cast<uint8_t> (0);
    

    printf("width: %d\n", input.width()); 
    printf("height: %d\n", input.height());
    printf("channel: %d\n", input.channels());

    g_n(r.x, r.y) = abs(cast<float>(input(r.x, r.y + 1) - input(r.x, r.y - 1))) + abs(cast<float>(input(r.x, r.y) - input(r.x, r.y - 2)));
    /*g_e(r.x, r.y) = abs(cast<int8_t>(input(r.x - 1, r.y) - input(r.x + 1, r.y))) + abs(cast<int8_t>(input(r.x, r.y) - input(r.x + 2, r.y)));
    g_s(r.x, r.y) = abs(cast<int8_t>(input(r.x, r.y - 1) - input(r.x, r.y + 1))) + abs(cast<int8_t>(input(r.x, r.y) - input(r.x, r.y + 2)));
    g_w(r.x, r.y) = abs(cast<int8_t>(input(r.x + 1, r.y) - input(r.x - 1, r.y))) + abs(cast<int8_t>(input(r.x, r.y) - input(r.x - 2, r.y)));
*/
    w_n(r.x, r.y) = cast<float>(1 / (1 + g_n(r.x, r.y)));
  /*  w_e(r.x, r.y) = 1 / (1 + g_e(r.x, r.y));
    w_s(r.x, r.y) = 1 / (1 + g_s(r.x, r.y));
    w_w(r.x, r.y) = 1 / (1 + g_w(r.x, r.y));
*/
    g_n_est(r.x, r.y) = cast<float>(input(r.x, r.y - 1) + (input(r.x, r.y) - input(r.x, r.y - 2))) / 2;
  /*  g_e_est(r.x, r.y) = input(r.x + 1, r.y) + (input(r.x, r.y) - input(r.x + 2, r.y)) / 2;
    g_s_est(r.x, r.y) = input(r.x, r.y + 1) + (input(r.x, r.y) - input(r.x, r.y + 2)) / 2;
    g_w_est(r.x, r.y) = input(r.x - 1, r.y) + (input(r.x, r.y) - input(r.x - 2, r.y)) / 2;
*/
    green(r.x, r.y) = cast<uint8_t>(w_n(r.x, r.y) * g_n_est(r.x, r.y))/*+ w_e(r.x, r.y) * g_e_est(r.x, r.y) + w_s(r.x, r.y) * g_s_est(r.x, r.y)+ w_w(r.x, r.y) * g_w_est(r.x, r.y)*/;
    green.trace_stores();
     

    printf("stuck\n");
    Buffer<uint8_t> temp = green.realize(input.width(), input.height());
    printf("after realize\n");
    Tools::save_image(temp, "result.png");
    

/* Excerpt from the paper: "Following interpolation of all missing G values, B/R values are interpolated at R/B pixels using the full green color pane."*/
    {
        Func blue("blue"), b_ne, b_se, b_sw, b_nw, w_ne, w_se, w_sw, w_nw, b_ne_est, b_se_est, b_sw_est, b_nw_est;
        
        b_ne(x, y) = cast<float> (0);
        /*b_se(x, y) = cast<uint8_t> (0);
        b_sw(x, y) = cast<uint8_t> (0);
        b_nw(x, y) = cast<uint8_t> (0);
*/
        w_ne(x, y) = cast<float> (0);
       /* w_se(x, y) = cast<uint8_t> (0);
        w_sw(x, y) = cast<uint8_t> (0);
        w_nw(x, y) = cast<uint8_t> (0);
*/
        b_ne_est(x, y) = cast<float> (0);
  /*      b_se_est(x, y) = cast<uint8_t> (0);
        b_sw_est(x, y) = cast<uint8_t> (0);
        b_nw_est(x, y) = cast<uint8_t> (0);
*/
        blue(x, y) = cast<uint8_t> (0);

        b_ne(r.x, r.y) = abs(cast<float>(input(r.x - 1, r.y + 1) - input(r.x + 1, r.y - 1))) /*+ abs(cast<float>(green(r.x, r.y) - green(r.x + 1, r.y - 1)))*/;
    /*    b_se(r.x, r.y) = abs(cast<int8_t>(input(r.x - 1, r.y - 1) - input(r.x + 1, r.y + 1))) + abs(cast<int8_t>(green(r.x, r.y) - green(r.x + 1, r.y + 1)));
        b_sw(r.x, r.y) = abs(cast<int8_t>(input(r.x + 1, r.y - 1) - input(r.x - 1, r.y + 1))) + abs(cast<int8_t>(green(r.x, r.y) - green(r.x - 1, r.y + 1)));
        b_nw(r.x, r.y) = abs(cast<int8_t>(input(r.x + 1, r.y + 1) - input(r.x - 1, r.y - 1))) + abs(cast<int8_t>(green(r.x, r.y) - green(r.x - 1, r.y - 1)));
*/

        w_ne(r.x, r.y) = cast<float>(1 / (1 + b_ne(r.x, r.y)));
  /*      w_se(r.x, r.y) = 1 / (1 + b_se(r.x, r.y));
        w_sw(r.x, r.y) = 1 / (1 + b_nw(r.x, r.y));
        w_nw(r.x, r.y) = 1 / (1 + b_sw(r.x, r.y));
*/
        b_ne_est(r.x, r.y) = cast<float>(input(r.x    , r.y    ))/*+ (green(r.x, r.y) - green(r.x + 1, r.y - 1)) / 2*/;
        /*b_se_est(r.x, r.y) = input(r.x + 1, r.y + 1) + (green(r.x, r.y) - green(r.x + 1, r.y + 1)) / 2;
        b_sw_est(r.x, r.y) = input(r.x - 1, r.y + 1) + (green(r.x, r.y) - green(r.x - 1, r.y + 1)) / 2;
        b_nw_est(r.x, r.y) = input(r.x - 1, r.y - 1) + (green(r.x, r.y) - green(r.x - 1, r.y - 1)) / 2;
*/
        blue(r.x, r.y) = cast<uint8_t>(w_ne(r.x, r.y) * b_ne_est(r.x, r.y))/* + w_se(r.x, r.y) * b_se_est(r.x, r.y) + w_sw(r.x, r.y) * b_sw_est(r.x, r.y) + w_nw(r.x, r.y) * b_nw_est(r.x, r.y)*/;
        blue.trace_stores();
        blue.parallel(y).parallel(x).vectorize(x, 16);
        Buffer<uint8_t> result1 = blue.realize(input.width(), input.height());
        printf("after blue realize");
        Tools::save_image(result1, "result1.png"); 

    }


   
} 
