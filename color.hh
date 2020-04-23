#pragma once

enum class colorspace {
    RGB, LAB, XYZ
};

template<colorspace store_as = colorspace::RGB> struct Color; 

template<> struct Color<colorspace::RGB> {
    double r,g,b;
    constexpr Color(const unsigned red, const unsigned green, const unsigned blue) noexcept: r(red),g(green),b(blue) {}
    constexpr Color(const unsigned hex) noexcept: r(hex >> 16), g( (hex >> 8) & 0xFF ), b( (hex & 0xFF)) { }		
    constexpr explicit Color() noexcept: r(0), g(0), b(0) { }		

    Color(const Color<colorspace::XYZ>& xyz_input) noexcept;

	Color<colorspace::RGB> blend(const Color<colorspace::RGB>& withColor, const double ratio) const noexcept;
    
    double Weighted_ColorCompare(const Color<colorspace::RGB>& withColor) const noexcept;
    double El_ColorCompare(const Color<colorspace::RGB>& withColor) const noexcept {
		double 	diffR = (double(this->r) - double(withColor.r)), 
		diffG = (double(this->g) - double(withColor.g)),
		diffB = (double(this->b) - double(withColor.b));
		return (diffR*diffR + diffG*diffG + diffB*diffB);
	}
    
    double ColorCompare(const Color<colorspace::RGB>& withColor) const noexcept {
		return Weighted_ColorCompare(withColor);
	}
};

template<> struct Color<colorspace::XYZ> {
    double x,y,z;
    Color(const Color<colorspace::RGB>& rgb_input) noexcept;
    Color(const Color<colorspace::LAB>& lab_input) noexcept;
    constexpr explicit Color() noexcept: x(0),y(0),z(0) {};
    
    Color<colorspace::XYZ> blend(const Color<colorspace::XYZ>& withColor, const double ratio) const noexcept;   
};

template<> struct Color<colorspace::LAB> {
    double l,a,b,c,h;
    Color(const Color<colorspace::XYZ>& xyz_input) noexcept;
	constexpr explicit Color() noexcept: l(0), a(0), b(0), c(0), h(0) { }

    Color<colorspace::LAB> blend(const Color<colorspace::LAB>& withColor, const double ratio) const noexcept;
    
    double CIE76_ColorCompare(const Color<colorspace::LAB>& withColor) const noexcept;
    double CIE94_ColorCompare(const Color<colorspace::LAB>& withColor) const noexcept;
    
    double ColorCompare(const Color<colorspace::LAB>& withColor) const noexcept { return CIE76_ColorCompare(withColor); }
};
