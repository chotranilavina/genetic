/*
 * image.cpp
 * Evolve - Image generating environment
 *
 * Copyright (c) 2009 Tim Besard <tim.besard@gmail.com>
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
DNA format specification
	- 4 bits describing the colour (R, G, B, and alpha)
	- All following pairs of 2 bytes describe a point, in which
	  a coordinate = byte/254 * image_width or image_height

Thus, a minimum of 9 bytes is required, and should be odd!

REMARK: ignoring invalidity and skipping the byte (might get corrected
on later mutation) could be better.
*/

///////////////////
// CONFIGURATION //
///////////////////

//
// Essential stuff
//

// Headers
#include "../src/population.h"
#include "../src/environment.h"
#include <cmath>
#include <queue>
#include <string>
#include <sstream>
#include <iostream>
#include <cairo/cairo.h>

//
// Constants
//

// Polygon limits
const int LIMIT_POLYGONS = 50;
const int LIMIT_POLYGON_POINTS = 5;

// Image settings
const bool IMAGE_ALPHA = false;


//
// Class definitions
//

// Environment
class EnvImage : public Environment
{
	public:
		// Required functons
		double fitness(std::list<std::list<int> > inputList);
		int alphabet();
		void update(std::list<std::list<int> > inputList);
		void output(cairo_surface_t* inputSurface, double fitness);

		// Image functions
		bool loadImage(const std::string& inputFile);
		bool valid_limits(std::list<std::list<int> >& inputList);
		void draw(cairo_surface_t* inputSurface, std::list<std::list<int> >& inputList);
		double compare(cairo_surface_t* inputSurface);

	private:
		std::string dataInputFile;
		unsigned char* dataInputRGB24;
		unsigned int dataInputWidth, dataInputHeight;
		int counter;
};




/////////////////
// ENVIRONMENT //
/////////////////

//
// Required functons
//

// Fitness function
double EnvImage::fitness(std::list<std::list<int> > inputList)
{
	// Check the DNA's limit's
	if (!valid_limits(inputList))
		return -1;

	// Create a DC for the generated image
    cairo_surface_t* tempSurface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, dataInputWidth, dataInputHeight);

	// Draw the DNA onto the DC
	draw(tempSurface, inputList);

	// Compare them
	double resemblance = compare(tempSurface);

	// Finish
	cairo_surface_destroy(tempSurface);
	return resemblance;
}

// Alphabet (maximal amount of instructions)
int EnvImage::alphabet()
{
	return 254;
}

// Update call
void EnvImage::update(std::list<std::list<int> > inputList)
{
    // Create surface
    cairo_surface_t* tempSurface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, dataInputWidth, dataInputHeight);

	// Draw the DNA onto the DC
	draw(tempSurface, inputList);

    // Let the application output the bitmap
    output(tempSurface, fitness(inputList));

    // Finish
    cairo_surface_destroy(tempSurface);
}

// Output call
void EnvImage::output(cairo_surface_t* inputSurface, double inputFitness)
{
    // Generate an output tag
    std::stringstream convert;
    convert << dataInputFile.substr( dataInputFile.find_last_of(".") ) << "-";
    convert << counter++ << ".png";

    // Save the file
    cairo_surface_write_to_png(inputSurface, convert.str().c_str());

    // Output a message
    std::cout << "- Successfully mutated (new fitness: " << int(10000*inputFitness)/100.0 << "%)" << std::endl;
}



//
// Image functions
//

// Set input image
bool EnvImage::loadImage(const std::string& inputFile)
{
    // Update variables
	dataInputFile = inputFile;
	counter = 0;

	// Create Cairo surface from file
	std::cout << "DEBUG: loading " << dataInputFile << std::endl;
	cairo_surface_t* tempSurface = cairo_image_surface_create_from_png(dataInputFile.c_str());
	if (tempSurface == NULL)
	{
	    std::cout << "ERROR: could not load PNG image" << std::endl;
	    return false;
	}
	if (cairo_image_surface_get_format(tempSurface) != CAIRO_FORMAT_RGB24)
	{
	    std::cout << "ERROR: resulting surface is not RGB24 type" << std::endl;
	    return false;
	}

	// Save data
	dataInputRGB24 = cairo_image_surface_get_data(tempSurface);

	// Save size
	dataInputWidth = cairo_image_surface_get_width(tempSurface);
	dataInputHeight = cairo_image_surface_get_height(tempSurface);

	// Return
	return true;
}

// Validity function
bool EnvImage::valid_limits(std::list<std::list<int> >& inputList)
{
	// Check amount of polygons
	if (inputList.size() < 1 || inputList.size() > LIMIT_POLYGONS)
		return false;

	// Check points per polygon
	std::list<std::list<int> >::iterator it = inputList.begin();
	while (it != inputList.end())
	{
		int size = (it++)->size();

		// 10 bytes at minimum
		if ((size-4)/2 > LIMIT_POLYGON_POINTS || size < 10)
			return false;

		// Should be even
		if (size%2 != 0)
			return false;
	}

	// All checked, seems valid
	return true;
}

// Render the DNA code onto a draw container
void EnvImage::draw(cairo_surface_t* inputSurface, std::list<std::list<int> >& inputList)
{
    // Create a Cairo context
    cairo_t *cr = cairo_create(inputSurface);

    // Draw white background
    cairo_set_source_rgb(cr, 255, 255, 255);
    cairo_rectangle(cr, 0, 0, dataInputWidth, dataInputHeight);
    cairo_fill(cr);

	// Loop all genes
	std::list<std::list<int> >::iterator it = inputList.begin();
	while (it != inputList.end())
	{
		std::list<int>::iterator it2 = it->begin();

		// Get colour code
		int r = *(it2++), g = *(it2++), b = *(it2++), a = *(it2++);
		if (IMAGE_ALPHA)
            cairo_set_source_rgba(cr, r, g, b, a);
        else
            cairo_set_source_rgb(cr, r, g, b);

        // Move to start point
        int x = *(it2++)-1, y = *(it2++)-1;
        cairo_move_to(cr, dataInputWidth*x/253.0, dataInputHeight*y/253.0);

		// Load coördinates
		while (it2 != it->end())
		{
			// Points vary between 1 and 254, so let 1 be the lower bound and 254 the upper one
			x = *(it2++)-1;
			y = *(it2++)-1;
			cairo_line_to(cr, dataInputWidth*x/253.0, dataInputHeight*y/253.0);
		}

		// Draw
		cairo_close_path(cr);
		cairo_fill(cr);
		++it;
	}
	cairo_destroy(cr);
}

// Compare two images
double EnvImage::compare(cairo_surface_t* inputSurface)
{
	// Get and verify size
	if ((cairo_image_surface_get_width(inputSurface) != dataInputWidth) || (cairo_image_surface_get_height(inputSurface) != dataInputHeight))
	{
		std::cout << "WARNING: cannot calculate resemblance between two different-sized images" << std::endl;
		return 0;
	}

	// Verify formats
	if (cairo_image_surface_get_format(inputSurface) != CAIRO_FORMAT_RGB24)
	{
		std::cout << "WARNING: can only process RGB24 data" << std::endl;
		return 0;
	}

	// Get the raw data of the given surface
	unsigned char* tempData1 = dataInputRGB24;
	unsigned char* tempData2 = cairo_image_surface_get_data(inputSurface);

	// Compare them
	long double difference = 0;
	for (int i = 0; i < dataInputWidth*dataInputHeight; i++)
	{
		// RGB (GetData always returns RGB, without Alpha)
		int db = *(tempData1++) - *(tempData2++);
		int dg = *(tempData1++) - *(tempData2++);
		int dr = *(tempData1++) - *(tempData2++);
		int da = 0;
		if (IMAGE_ALPHA)
            da = *(tempData1++) - *(tempData2++);
        else
            ++tempData1 && ++tempData2;

		// Calculate difference
		difference += sqrt(abs(dr) + abs(dg) + abs(db) + abs(da));
	}

	// Get resemblance
	double resemblance = dataInputWidth*dataInputHeight / difference;
	return resemblance;
}



//////////
// MAIN //
//////////

int main(int argc, char** argv)
{
	//
	// Load image
	//

	// Check input
	if (argc != 2)
	{
        std::cout << "ERROR: input filename missing or too many parameters" << std::endl;
        return 1;
	}

	// Save filename
	std::string inputFile = argv[1];

	// Message
	std::cout << "NOTE: configured" << std::endl;


	//
	// Create environment
	//

	// Create object
	EnvImage dataEnvironment;

	// Load base image
	if (!dataEnvironment.loadImage(inputFile))
	{
	    std::cout << "ERROR: could not load image" << std::endl;
	    return 1;
	}

	// Message
	std::cout << "NOTE: environment created" << std::endl;


	//
	// Create population
	//

	// Initial DNA (black window)
	std::queue<int> tempDNA;
	tempDNA.push(255);	// Start of DNA
	tempDNA.push(254);	// Semi transparent black brush (RGBa = 254 254 254 128)
	tempDNA.push(254);
	tempDNA.push(254);
	tempDNA.push(128);
	tempDNA.push(1);	// Point one: (1, 1)
	tempDNA.push(1);
	tempDNA.push(254);	// Point two: (254, 1)
	tempDNA.push(1);
	tempDNA.push(254);	// Point three: (254, 254)
	tempDNA.push(254);
	tempDNA.push(1);	// Point four: (1, 254)
	tempDNA.push(254);
	tempDNA.push(255);	// End of DNA

	// Create object
	Population dataPopulation(&dataEnvironment, tempDNA);

	// Message
	std::cout << "NOTE: population created" << std::endl;

	dataPopulation.evolve_single_straight(10000);

	return 0;
}

