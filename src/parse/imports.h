/*
 *  This file is part of First Step.
 *  
 *  First Step is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software 
 *  Foundation, either version 3 of the License, or (at your option) any later version. 
 *
 *  First Step is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with First Step.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Copyright (C) Marco Cilloni <marco.cilloni@yahoo.com> 2014
 *
 */

#if !defined(_IMPORTS_H)
#define _IMPORTS_H

#include <treemap/map.h>

typedef StringMap Imports;

struct pnode;

extern Imports* (*imports_new)(void);
bool imports_register(Imports *imps, const char *string, struct pnode *pnode);
extern bool (*imports_exists)(Imports *imps, const char *key);
struct pnode* imports_get(Imports *imps, const char *string);

#endif
