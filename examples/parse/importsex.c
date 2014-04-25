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

#include <parse/importer.h>
#include <parse/ptree.h>

#include <stdlib.h>

int main(void) {
  Pool *pool = pool_new();
  struct importer *importer = importer_new(pool);

  struct pnode* module = importer_import(importer, "lal");

  pnode_free(module);
  pool_release(pool, (void (*)(void*)) type_free);
  importer_free(importer);
  return EXIT_SUCCESS;
}
