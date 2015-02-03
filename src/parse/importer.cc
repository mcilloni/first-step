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
#include "importer.h"
#include "parse.h"

#include <utils/env.h>
#include <utils/splitter.h>
#include <utils/utils.h>

#include <treemap/map.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>

#include <fts.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>

struct importer {
  Pool *pool;
  Imports *imported;
  Map *fordFiles;
};

Pool* importer_getPool(struct importer *impr) {
  return impr->pool;
}

struct importer* importer_new(Pool *pool) {
  struct importer *importer = new struct importer;
  *importer = { pool, imports_new(), strmap_new() }; 
  
  std::vector<char*> paths;
  FTS *fts;
  paths.push_back(str_clone(getenv("PWD")));
  const char *modPath = getenv("FORDPATHS");

  if (modPath) {
    char *path;
    struct splitter *spl = splitter_new(modPath, ':');

    for (; (path = splitter_next(spl));) {
      paths.push_back(path);
    }

    splitter_free(spl);
  }

  paths.push_back(nullptr);

  if (!(fts = fts_open(&paths[0], FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR, nullptr))) {
    env.fail("Error while parsing module path %s: %s", modPath, strerror(errno));
  }  
 
  FTSENT *entry, *children = fts_children(fts, 0);

  if (children) {
    while ((entry = fts_read(fts))) {
      switch (entry->fts_info) {
        case FTS_F: {
          char *buf = str_clone(entry->fts_path);
          char *base = basename(buf);
          size_t len = strlen(base);
          if (len > 6 && !strcmp(base + len - 5, ".ford")) { //name + '.ford' at should be at least 6
            base[len - 5] = '\0';
            map_put(importer->fordFiles, str_clone(base), str_clone(entry->fts_path), FREE_KEY | FREE_VALUE);
          }
          free(buf);
          break;
        }
        case FTS_D:
          if (entry->fts_level > 0) {
            fts_set(fts, entry, FTS_SKIP);
          }
          break;
        default:
          break;
      }
    }
  } 

  if (errno) {
    env.fail("Error while parsing module path %s: %s", modPath, strerror(errno));
  }
  
  fts_close(fts);

  for (auto str : paths) {
    if (str) {
      free(str);
    }
  }

  return importer;
}

struct pnode* importer_import(struct importer *impr, char *name) {
  struct pnode *ret;

  if ((ret = imports_get(impr->imported, name))) {
    return ret;
  }

  char *path; 

  if (!map_get(impr->fordFiles, name, (void**) &path)) {
    return nullptr;
  }

  struct parser *prs = parser_new(path, impr);
  ret = parser_parse(prs, nullptr);

  if (!ret) {
    env.fail("Empty module");
  }

  struct proot *proot = (struct proot*) ret;

  if (strcmp(proot->module, name)) {
    env.fail("Module mismatch - named %s, but declared as %s", name, strlen(proot->module) ? proot->module : "<main>");
  }

  imports_register(impr->imported, str_clone(name), ret);

  parser_close(prs);

  return ret;
}

//This is necessary because of treemap limitations
void fakeFree(void *ignored) {
}

void importer_free(struct importer *impr) {
  map_freeSpec(impr->imported, nullptr, fakeFree);
  map_free(impr->fordFiles);
  delete impr;  
}

