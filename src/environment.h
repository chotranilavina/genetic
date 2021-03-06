/*
 * environment.h
 * Evolve - Starting environment
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

///////////////////
// CONFIGURATION //
///////////////////

//
// Essential stuff
//

// Include guard
#ifndef __ENVIRONMENT
#define __ENVIRONMENT

// Headers
#include "dna.h"



//////////////////////
// CLASS DEFINITION //
//////////////////////

class Environment
{
	public:
		// Required functions
		virtual double fitness(const DNA* inputDNA) = 0;
		virtual int alphabet() const = 0;
		virtual void update(const DNA* inputDNA) = 0;
		virtual bool condition() = 0;

                // TODO: explain function
};


// Include guard
#endif
