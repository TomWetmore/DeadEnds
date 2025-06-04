//
//  DeadEnds Library
//
//  stringset.h is the header file for the StringSet data type.
//
//  Created by Thomas Wetmore on 20 April 2024.
//  Last changed on 4 June 2025.
//

#ifndef stringset_h
#define stringset_h

typedef struct Set Set;
typedef Set StringSet;

StringSet* createStringSet(void);
void deleteStringSet(StringSet*, bool del);
void showStringSet(StringSet*);

#endif // stringset_h
