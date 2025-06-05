//
//  DeadEnds Library
//  deadends.h is the 'umbrella' header file giving access to the DeadEnds library.
//
//  Created by Thomas Wetmore on 5 June 2025.
//  Last changed on 5 June 2025.

#ifndef deadends_h
#define deadends_h

// Top-level include file for using the DeadEnds library.

// General data types
#include "block.h"
#include "hashtable.h"
#include "integertable.h"
#include "list.h"
#include "set.h"
#include "stringset.h"
#include "stringtable.h"

// Gedcom parsing and tree-building
#include "gedcom.h"
#include "gedpath.h"
#include "gnode.h"
#include "gnodeindex.h"
#include "gnodelist.h"
#include "import.h"
#include "parse.h"

// Programming language engine
#include "interp.h"
#include "pnode.h"
#include "pvalue.h"
#include "sequence.h"

// Context and database
#include "context.h"
#include "database.h"
#include "nameindex.h"
#include "recordindex.h"
#include "rootlist.h"
#include "errors.h"

// Utilities
#include "file.h"
#include "path.h"
#include "standard.h"
#include "utils.h"

#endif // deadends_h
