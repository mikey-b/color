# Color
INCOMPLETE

Conversion / learning experience from https://bisqwit.iki.fi/story/howto/dither/jy/

# API
Currently, Color spaces are separated and you can convert between them. The idea is to create the colorspace you want to store your colors in. E.g.

Color<colorspace::RGB> red{255,0,0};
Color<colorspace::LAB> lab_red{red};

# Compiling
Requires g++, type make test to build the tests. or make demo for the demo application.

# Demo application
This is based on https://bisqwit.iki.fi/story/howto/dither/jy/ - Implementing the "Yliluoma's ordered dithering algorithm 1 + Psychovisual model (Slow Version)", The application requires a copy scene.png to function which can be downloaded at https://bisqwit.iki.fi/jutut/kuvat/ordered_dither/scene.png

# Test Results
	With fastmath;
	Time = 3.8s
	Conversions with difference of 1 = 3 / 16777216 (0.00001%)
	Conversions with difference of 2 = 1 / 16777216 (0.000005%)
	Conversions with difference of 3+ = 0
	
	With cmath;
	Time = 21.4s
	Conversions with diff of 1 = 0 (0%)
	Conversions with diff of 2+ = 0
