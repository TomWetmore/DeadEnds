//
// menu.c
// MenuLibrary
//
// Created by Thomas Wetmore on 31 July 2024.
// Last changed on 8 August 2024.

#include <stdio.h>
#include "menu.h"

static void handleAction(Menu*, String);

// createMenuItem creates a MenuItem.
MenuItem* createMenuItem(String code, String command, Action action) {
	MenuItem* item = (MenuItem*) stdalloc(sizeof(MenuItem));
	item->code = code;
	item->command = strsave(command);
	item->action = action;
	return item;
}

// createMenu creates a Menu.
Menu* createMenu(String title, List* items) {
	Menu* menu = (Menu*) stdalloc(sizeof(Menu));
	menu->title = title;
	menu->items = items;
	return menu;
}

// menuMachine runs a Menu.
void menuMachine(Menu* menu) {
	char selection[32];
	while (true) {
		showMenu(menu);
		if (fgets(selection, sizeof(selection), stdin) != null) {
			selection[strcspn(selection, "\n")] = 0;
			handleAction(menu, selection);
		}
	}
}

// showMenu shows a menu. Used alone it is useful for testing and debugging. Eventually it will
// be used in tandem with code that will interact with the user and do things.
void showMenu(Menu* menu) {
	printf("%s\n", menu->title);
	FORLIST(menu->items, item)
		MenuItem* el = (MenuItem*) item;
		printf("  %s: %s\n", el->code, el->command);
	ENDLIST
	printf("Choose: ");
}

 // handleAction handles the selected action in a Menu.
 void handleAction(Menu* menu, String selection) {
	 FORLIST(menu->items, item)
		MenuItem* el = (MenuItem*) item;
		if (eqstr(el->code, selection)) {
			if (el->action) {
				(*(el->action))(menu, selection);
				return;
			}
			if (el->next) {
				menu = el->next;
			}
			return;
		}
	 ENDLIST
	 printf("%s invalid selection; please try again.\n", selection);
 }
 /*Fourth CHATGPT version that using strings for the codes.


 // Define the action to return to the main menu
 void returnToMainMenu(Menu **currentMenu) {
	 *currentMenu = &menu1;
 }

 // Define the main menu
 MenuItem menu1Items[] = {
	 {"1", "Execute Action One", actionOne, NULL},
	 {"2", "Execute Action Two", actionTwo, NULL},
	 {"n", "Go to Next Menu", NULL, &menu2},
	 {"x", "Exit", actionExit, NULL}
 };
 Menu menu1 = {"Main Menu", menu1Items, sizeof(menu1Items) / sizeof(MenuItem)};

 int main() {
	 // Main loop to display the menu and handle user input
	 Menu *currentMenu = &menu1;
	 char selection[10];  // Assuming the selection code will be less than 10 characters
	 while (1) {
		 showMenu(currentMenu);
		 if (fgets(selection, sizeof(selection), stdin) != NULL) {
			 selection[strcspn(selection, "\n")] = '\0';  // Remove newline character
			 handleAction(&currentMenu, selection);
		 }
	 }

	 return 0;
 }
 */
