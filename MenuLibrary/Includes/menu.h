// MenuLibrary
//
// menu.h is the header file for the Menu Library.
//
// Created by Thomas Wetmore on 31 July 2024.
// Last changed on 2 August 2024.

#ifndef menu_h
#define menu_h

#include "standard.h"
#include "list.h"

typedef struct Menu Menu;
typedef void* (*Action)(Menu*, String);

// MenuItem is an item in a menu, duh.
typedef struct MenuItem {
	String code;
	String command;
	Action action;
	Menu* next;
	Menu* previous;
} MenuItem;

// Menu consists of a title and list of MenuItems.
typedef struct Menu {
	String title;
	List* items;
} Menu;

extern Menu* createMenu(String, List*);
extern MenuItem* createMenuItem(String, String, Action); // Replace void* with Action.
extern void menuMachine(Menu*);
extern void showMenu(Menu*);

#endif //
