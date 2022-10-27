/*
 * Delayed.cpp
 *
 *  Created on: Oct 4, 2017
 *      Author: ayachan
 */

#include "Delayed.h"

std::ostream& operator<< (std::ostream& os, const util::Delayed& me)
{ return me.print(os); }
std::ostream& operator<< (std::ostream& os, const util::Delayed* pme)
{ return pme->print(os); }
