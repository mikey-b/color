#include "fastmath.hh"
//#include <cmath>
#include "color.hh"
#include <cassert>

// Based on
// https://web.archive.org/web/20120502065620/http://cookbooks.adobe.com/post_Useful_color_equations__RGB_to_LAB_converter-14227.html
// http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_Lab.html

constexpr struct {
    const double REF_X = 95.047; // Observer= 2°, Illuminant= D65
    const double REF_Y = 100.000; 
    const double REF_Z = 108.883;
} illuminant_D65;

Color<colorspace::RGB>::Color(const Color<colorspace::XYZ>& xyz_input) noexcept {
    double x = xyz_input.x / 100;
    double y = xyz_input.y / 100;
    double z = xyz_input.z / 100;
	//assert((0 <= x) && (x <= 1));
	//assert((0 <= y) && (y <= 1));
	//assert((0 <= z) && (z <= 1));
	
	//RGB/XYZ Matrices
    double r = x * 3.2406 + y * -1.5372 + z * -0.4986;
    double g = x * -0.9689 + y * 1.8758 + z * 0.0415;
    double b = x * 0.0557 + y * -0.2040 + z * 1.0570;

    if ( r <= 0.0031308 ) { r = r * 12.92; } else { r = 1.055 * pow( r , ( 1 / 2.4 ) ) - 0.055; }
    if ( g <= 0.0031308 ) { g = g * 12.92; } else { g = 1.055 * pow( g , ( 1 / 2.4 ) ) - 0.055; }
    if ( b <= 0.0031308 ) { b = b * 12.92; } else { b = 1.055 * pow( b , ( 1 / 2.4 ) ) - 0.055; }
    
    #define cap(x) ((x) >= 255 ? 255 : round(x))
    this->r = cap(r * 255.0) ;
    this->g = cap(g * 255.0) ;
    this->b = cap(b * 255.0) ;
    #undef cap
}
double Color<colorspace::RGB>::Weighted_ColorCompare(const Color<colorspace::RGB>& withColor) const noexcept {
		double luma1 = (double(this->r)*299.0 + double(this->g)*587.0 + double(this->b)*114.0) / (255.0*1000.0);
		double luma2 = (withColor.r*299.0 + withColor.g*587.0 + withColor.b*114.0) / (255.0*1000.0);
		double lumadiff = luma1-luma2;
		double 	diffR = (double(this->r)-withColor.r) / 255.0, 
				diffG = (double(this->g)-double(withColor.g)) / 255.0,
				diffB = (double(this->b)-double(withColor.b)) / 255.0;
		return (diffR*diffR*0.299 + diffG*diffG*0.587 + diffB*diffB*0.114)*0.75
			 + lumadiff*lumadiff;
}

Color<colorspace::RGB> Color<colorspace::RGB>::blend(const Color<colorspace::RGB>& withColor, const double ratio) const noexcept {
	// ratio 0 = 100%, 0%
	// ratio 0.5 = 50%, 50%
	// ratio 1 = 0%, 100%
	assert((0 <= ratio) && (ratio <= 1));
	
	Color<colorspace::RGB> result{};
	result.r = (double(this->r) * (1.0 - ratio)) + (double(withColor.r) * ratio);
	result.g = (double(this->g) * (1.0 - ratio)) + (double(withColor.g) * ratio);
	result.b = (double(this->b) * (1.0 - ratio)) + (double(withColor.b) * ratio);
	return result;
}

Color<colorspace::XYZ>::Color(const Color<colorspace::RGB>& rgb_input) noexcept {
    double r = double(rgb_input.r) / 255.0;
    double g = double(rgb_input.g) / 255.0;
    double b = double(rgb_input.b) / 255.0;
	assert((0 <= r) && (r <= 1));
	assert((0 <= g) && (g <= 1));
	assert((0 <= b) && (b <= 1));    

    if (r > 0.04045) { r = pow((r + 0.055) / 1.055, 2.4); }
    else { r = r / 12.92; }
    
    if ( g > 0.04045){ g = pow((g + 0.055) / 1.055, 2.4); }
    else { g = g / 12.92; }
    
    if (b > 0.04045){ b = pow((b + 0.055) / 1.055, 2.4); }
    else {	b = b / 12.92; }
        
    r = r * 100.0;
    g = g * 100.0;
    b = b * 100.0;
	
    //Observer. = 2°, Illuminant = D65
    #define cap(x) ((x) > 255 ? 255 : (x))
    this->x = cap(r * 0.4124 + g * 0.3576 + b * 0.1805);
    this->y = cap(r * 0.2126 + g * 0.7152 + b * 0.0722);
    this->z = cap(r * 0.0193 + g * 0.1192 + b * 0.9505);
    #undef cap
}

Color<colorspace::XYZ>::Color(const Color<colorspace::LAB>& lab_input) noexcept {
    constexpr double E = 216.0 / 24389.0;
    constexpr double K = 24389.0 / 27.0;

    double y = (lab_input.l + 16.0) / 116.0;
	double z = y - (lab_input.b / 200.0);
    double x = (lab_input.a / 500.0) + y;

	// zr
    if ( pow( z , 3 ) > E ) { z = pow( z , 3 ); }
    else { z = ((116.0 * z) - 16.0) / K; }
    
	// yr
    if ( lab_input.l > (K*E) ) { y = pow( y , 3 ); }
    else { y = lab_input.l / K; }

	// xr
    if ( pow( x , 3 ) > E ) { x = pow( x , 3 ); }
    else { x = (( 116.0 * x ) - 16.0) / K; }

    this->x = illuminant_D65.REF_X * x;     
    this->y = illuminant_D65.REF_Y * y; 
    this->z = illuminant_D65.REF_Z * z;
}

Color<colorspace::XYZ> Color<colorspace::XYZ>::blend(const Color<colorspace::XYZ>& withColor, const double ratio) const noexcept {
	// ratio 0 = 100%, 0%
	// ratio 0.5 = 50%, 50%
	// ratio 1 = 0%, 100%
	Color<colorspace::XYZ> result{};
	result.x = ((this->x * (1.0 - ratio)) + (withColor.x * ratio));
	result.y = ((this->y * (1.0 - ratio)) + (withColor.y * ratio));
	result.z = ((this->z * (1.0 - ratio)) + (withColor.z * ratio));
	
	return result;
}

Color<colorspace::LAB>::Color(const Color<colorspace::XYZ>& xyz_input) noexcept {
    constexpr double E = 216.0/24389;
    constexpr double K = 24389.0/27.0;
    
    double z = xyz_input.z / illuminant_D65.REF_Z;  
    double y = xyz_input.y / illuminant_D65.REF_Y;  
    double x = xyz_input.x / illuminant_D65.REF_X;   
    
    if ( z > E ) { z = cbrt( z ); } else { z = ( (K * z) + 16.0) / 116.0; }
    if ( y > E ) { y = cbrt( y ); } else { y = ( (K * y) + 16.0) / 116.0; }
    if ( x > E ) { x = cbrt( x ); } else { x = ( (K * x) + 16.0) / 116.0; }
    
    this->b = 200.0 * ( y - z );
    this->a = 500.0 * ( x - y );
    this->l = ( 116.0 * y ) - 16.0;
       
    // CH
	this->c = sqrt( (this->a * this->a) + (this->b * this->b) );
	
	double temp_h = atan2(this->b, this->a);
	if (temp_h < 0) h += 360.0;
	this->h = temp_h;
}

Color<colorspace::LAB> Color<colorspace::LAB>::blend(const Color<colorspace::LAB>& withColor, const double ratio) const noexcept {
	Color<colorspace::RGB> tmp1{*this};
	Color<colorspace::RGB> tmp2{withColor};
	auto result = tmp1.blend(tmp2, ratio);
	
	return Color<colorspace::LAB>{result};
}

double Color<colorspace::LAB>::CIE76_ColorCompare(const Color<colorspace::LAB>& withColor) const noexcept {
	double 	diffL = withColor.l - this->l,
			diffA = withColor.a - this->a,
			diffB = withColor.b - this->b;
	//return sqrt( (diffL * diffL) + (diffA * diffA) + (diffB * diffB) );
	return (diffL * diffL) + (diffA * diffA) + (diffB * diffB);
}

double Color<colorspace::LAB>::CIE94_ColorCompare(const Color<colorspace::LAB>& withColor) const noexcept {
	constexpr bool textile_weights = true;
	
	constexpr double K[2][3] = { 
		{1.0, 0.045, 0.015},
		{2.0, 0.048, 0.014}
	};
	
	double C1 = sqrt( (this->a * this->a) + (this->b * this->b) );
	double C2 = sqrt( (withColor.a * withColor.a) + (withColor.b * withColor.b) );
	double dCab = C1 - C2;
	
	double dL = this->l - withColor.l;
	
	double dA = this->a - withColor.a;
	double dB = this->b - withColor.b;
	
	double dHab = sqrt( (dA * dA) + (dB * dB) - (dCab * dCab) );
	constexpr double Sl = 1.0;
	double Sc = 1.0 + K[textile_weights][1] * C1;
	double Sh = 1.0 + K[textile_weights][2] * C1;
	
	constexpr double Kl = K[textile_weights][0];
	constexpr double Kc = 1.0; //K[textile_weights][1];
	constexpr double Kh = 1.0; //K[textile_weights][2];
	
	double coefL = dL / (Kl * Sl);
	double coefCab = dCab / (Kc * Sc);
	double coefH = dHab / (Kh * Sh);
	
	return sqrt( (coefL * coefL) + (coefCab * coefCab) + (coefH * coefH) );
}
