#pragma once
//#include <bit>

constexpr inline double pow(const double a, const double b) {
    union {
        double d;
        int x[2];
    } u = { a };

    //if constexpr (std::endian::native == std::endian::little) {
        u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
        u.x[0] = 0;
    //} else {
        // TODO:
    //}    


    return u.d;
}

constexpr inline double cbrt(const double x) {	
    return pow(x, (1/3.0));
} 

constexpr inline double sqrt(const double x) {
	// This could be faster
	return pow( x, (1/2.0) );
}

constexpr inline double atan2(const double a, const double b) {
	// TODO:
	return 0;
}

constexpr inline int round(const double d) {
    const int trunc = (int) d;
    const double diff = d - (double) trunc;
    return trunc + (diff >= 0.5);
}
