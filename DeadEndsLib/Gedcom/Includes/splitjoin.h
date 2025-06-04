//  DeadEnds Library
//
//  splitjoin.h defines the functions that split, join and normalize GNode records.
//
//  Created by Thomas Wetmore on 7 November 2022.
//  Last changed on 4 June 2025.

#ifndef splitjoin_h
#define splitjoin_h

typedef struct GNode GNode;

// Prototypes of the functions found in splitjoin.c
void splitPerson(GNode*, GNode**, GNode**, GNode**, GNode**, GNode**, GNode**);
void joinPerson(GNode*, GNode*, GNode*, GNode*, GNode*, GNode*, GNode*);
void splitFamily(GNode*, GNode**, GNode**, GNode**, GNode**, GNode**);
void joinFamily(GNode*, GNode*, GNode*, GNode*, GNode*, GNode*);

GNode *normalizeRecord(GNode*);
GNode *normalizePerson(GNode*);
GNode *normalizeFamily(GNode*);
GNode *normalizeEvent(GNode*);
GNode *normalizeSource(GNode*);
GNode *normalizeOther(GNode*);

#endif // splitjoin_h
