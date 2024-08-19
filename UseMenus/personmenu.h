// personmenu.h
// UseMenus
//
// Created by Thomas Wetmore on 5 August 2024.
// Last changed on 15 August 2024.

#ifndef personmenu_h
#define personmenu_h

#include "standard.h"
#include "sequence.h"
#include "gnode.h"
#include "lineage.h"
#include "name.h"

// BrowseReturn is the type of return codes from some menu operations.
typedef enum {
	browseQuit,
	browseFamily,
	browsePerson,
	browsePedigree,
	browseTandem,
} BrowseReturn;

#endif // personmenu_h
