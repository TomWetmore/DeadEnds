//
//  ask.h
//  CloneOne
//
//  Created by Thomas Wetmore on 8/9/24.
//

#include "standard.h"
#include "list.h"

#ifndef ask_h
#define ask_h

typedef enum AskReturn {
	askOkay = 1,
	askFail = 2,
	askQuit = 4,
} AskReturn;

AskReturn askForInteger(String, int, int*);
AskReturn askForStringInSet(String, List*, int, String*);
AskReturn askForPattern(String, String pattern, int, String*);

List* createStringList(String, ...);


#endif /* ask_h */
