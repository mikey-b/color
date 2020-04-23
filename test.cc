#include <iostream>
#include "color.hh"

int main() {
	int errors = 0;
	// Convertion Test
	
	#pragma omp parallel for
	for(unsigned r = 0; r < 256; ++r) {
		for(unsigned g = 0; g < 256; ++g) {
			for(unsigned b = 0; b < 256; ++b) {				
				Color<colorspace::RGB> q{r,g,b};
				Color<colorspace::XYZ> x{q};
				Color<colorspace::LAB> t{q};
				Color<colorspace::XYZ> x2{t};
				Color<colorspace::RGB> q2{x2};
				
				#define diff(x, y) ( ( x > y ? x - y: y - x) > 0)
	
				if ( diff(q2.r, q.r) || diff(q2.g, q.g) || diff(q2.b, q.b) ) {
					//std::cout << "RGB " << q.r << "," << q.g << "," << q.b << "\n";
					//std::cout << "XYZ " << x.x << "," << x.y << "," << x.z << "\n";
					//std::cout << "LAB " << t.l << "," << t.a << "," << t.b << "\n";
					//std::cout << "XYZ " << x2.x << "," << x2.y << "," << x2.z << "\n";
					//std::cout << "RGB " << q2.r << "," << q2.g << "," << q2.b << "\n";    
					//std::cout << "\n";
					errors+=1;
				}
			}	
		}
	}
	
	std::cout << "\nErrors = " << errors << "\n";

    return 0;
}
