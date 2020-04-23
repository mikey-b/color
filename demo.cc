#include <gd.h>
#include <stdio.h>
#include <cmath>
#include <cassert>
#include <array>
#include "color.hh"

/* 8x8 threshold map */
#define d(x) x / 64.0
constexpr double map[8*8] = {
d( 0), d(48), d(12), d(60), d( 3), d(51), d(15), d(63),
d(32), d(16), d(44), d(28), d(35), d(19), d(47), d(31),
d( 8), d(56), d( 4), d(52), d(11), d(59), d( 7), d(55),
d(40), d(24), d(36), d(20), d(43), d(27), d(39), d(23),
d( 2), d(50), d(14), d(62), d( 1), d(49), d(13), d(61),
d(34), d(18), d(46), d(30), d(33), d(17), d(45), d(29),
d(10), d(58), d( 6), d(54), d( 9), d(57), d( 5), d(53),
d(42), d(26), d(38), d(22), d(41), d(25), d(37), d(21) };
#undef d

/* Palette */
constexpr Color<colorspace::RGB> pal[16] =
{0x080000,0x201A0B,0x432817,0x492910,
 0x234309,0x5D4F1E,0x9C6B20,0xA9220F,
 0x2B347C,0x2B7409,0xD0CA40,0xE8A077,
 0x6A94AB,0xD5C4B3,0xFCE76E,0xFCFAE2 };

struct const_s {
	Color<colorspace::RGB> c;
	double comp;
};

const std::array< std::array<std::array<const_s, 64>, 16>, 16> CompareConst() {
	std::array< std::array<std::array<const_s, 64>, 16>, 16> result;

#pragma omp parallel for
	for(unsigned index1 = 0; index1 < 16; ++index1) {
		for(unsigned index2 = index1; index2 < 16; ++index2) {
			const Color<colorspace::RGB> color1{pal[index1]}, color2{pal[index2]};
						
			for(unsigned ratio=0; ratio < 64; ++ratio)
			{
				// Determine what mixing them in this proportion will produce
				result[index1][index2][ratio] = { 
					color1.blend(color2, (ratio / 63.0)),
					color1.ColorCompare(color2) * (0.1 * (fabs((ratio / 63.0) - 0.5)+0.5))
				};
			}
		}
	}
	return result;
}

template<typename Cs = Color<colorspace::RGB> >
double EvaluateMixingError(const Cs& col1,    // Desired color
                           const Cs& col2, // Mathematical mix product
                           const Cs& col3, // Mix component 1
                           const Cs& col4, // Mix component 2
                           double ratio)         // Mixing ratio
{
	double temp = col3.ColorCompare(col4) * (0.1 * (fabs(ratio - 0.5)+0.5));
    return col1.ColorCompare(col2) + temp;
}

template<typename Cs = Color<colorspace::RGB> >
double EvaluateMixingError(const Color<colorspace::RGB>& col1,    // Desired color
                           const Color<colorspace::RGB>& col2, // Mathematical mix product
                           const double precomputed)    
{
    return col1.ColorCompare(col2) + precomputed;
}

struct MixingPlan
{
    unsigned colors[2]; // Indexes
    double ratio; /* 0 = always index1, 1 = always index2, 0.5 = 50% of both */
};

MixingPlan DeviseBestMixingPlan(const Color<colorspace::RGB>& color)
{
    MixingPlan result = { {0,0}, 0.5 };
    double least_penalty = 1e99;
    
    static const auto ratio_lookup = CompareConst();
    
    #pragma omp parallel for
    for(unsigned index1 = 0; index1 < 16; ++index1)
    for(unsigned index2 = index1; index2 < 16; ++index2)
    {       
		for(unsigned ratio=0; ratio < 64; ++ratio)
		{
			// Determine what mixing them in this proportion will produce
			const auto color0 = ratio_lookup[index1][index2][ratio];
			
			// Determine how well that matches what we want to accomplish
			double penalty = EvaluateMixingError<>(color, color0.c, color0.comp );

			if(penalty < least_penalty)
			{
				// Keep the result that has the smallest error
				least_penalty = penalty;
				result.colors[0] = index1;
				result.colors[1] = index2;
				result.ratio = (ratio / 63.0);
			}
		}        

    }
    return result;
}

int main()
{
    FILE* fp = fopen("scene.png", "rb");
    gdImagePtr srcim = gdImageCreateFromPng(fp);
    fclose(fp);

    unsigned w = gdImageSX(srcim), h = gdImageSY(srcim);
    gdImagePtr im = gdImageCreate(w, h);
     
    for(unsigned c=0; c<16; ++c) gdImageColorAllocate(im, pal[c].r, pal[c].g, pal[c].b);
        
#pragma omp parallel for
    for(unsigned y=0; y<h; ++y)
        for(unsigned x=0; x<w; ++x)
        {
          double map_value = map[(x & 7) + ((y & 7) << 3)];
          
          unsigned tcolor = gdImageGetTrueColorPixel(srcim, x, y);         
          const Color<colorspace::RGB> c{tcolor};
		  
          MixingPlan plan = DeviseBestMixingPlan(c);
               
          gdImageSetPixel(im, x,y, plan.colors[ map_value < plan.ratio ? 1 : 0 ] );
        }
    fp = fopen("scenedither.png", "wb");
    gdImagePng(im, fp);
    fclose(fp); gdImageDestroy(im); gdImageDestroy(srcim);
}
