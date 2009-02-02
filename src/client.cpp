/*
 * client.cpp
 * Evolve - Evolutionary client
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

// Headers
#include "client.h"


////////////////////
// CLASS ROUTINES //
////////////////////

//
// Construction and destruction
//

// Create client with given DNA
Client::Client(std::queue<int>& inputQueue)
{
	dataParser.setQueue(inputQueue);
	dataDNA = dataParser.getList();
}
Client::Client(std::list<std::vector<int> >& inputList)
{
	dataDNA = inputList;
}


//
// DNA alteration
//

// Mutate the DNA
void Client::mutate()
{
	// Amount of mutations
	int tempAmount = random_range(MUTATE_AMOUNT_LOWER, MUTATE_AMOUNT_UPPER);
}

// Combine the DNA with another client
void Client::crossover(Client&)
{
}


//
// DNA output
//

std::list<std::vector<int> > Client::getList()
{
	return dataDNA;
}

std::queue<int> Client::getQueue()
{
	dataParser.setList(dataDNA);
	return dataParser.getQueue();
}
