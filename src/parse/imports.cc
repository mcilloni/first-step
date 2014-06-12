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

#include "imports.h"

#include "ptree.h"

Imports* (*imports_new)(void) = (Imports* (*)(void)) strmap_new;

bool imports_register(Imports *imps, const char *string, struct pnode *pnode) {
  return (bool) map_put(imps, string, pnode, FREE_KEY | FREE_VALUE);
}

bool (*imports_exists)(Imports *imps, const char *key) = (bool (*)(Imports*,const char*)) map_contains;

struct pnode* imports_get(Imports *imps, const char *string) {
  struct pnode *ret;
  if (map_get(imps, string, (void**) &ret)) {
    return ret;
  }

  return nullptr;
}

