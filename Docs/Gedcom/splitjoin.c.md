# splitjoin.c

The functions in this file put Gedcom records, as GNode trees, into *standard format*. Standard format does not remove or add any nodes to the trees, nor does it change the contents of any node. It puts certain level 1 nodes into a standard order. In persons these nodes are the NAME, SEX, REFN, FAMC and FAMS nodes. In family records these nodes are the REFN, HUSB, WIFE and CHIL nodes.

|Component|Description|
|:---|:---|
|void splitPerson(GNode *indi, GNode **pname, GNode **prefn, GNode **psex, GNode **pbody, GNode **pfamc, GNode **pfams)|Split a person GNode tree into parts. The nodes are not copied; they are separated into different lists.|
|void joinPerson (GNode *indi, GNode *name, GNode *refn, GNode *sex, GNode *body, GNode *famc, GNode *fams)|Join a person GNode tree from parts. This neither allocates nor frees any nodes.|
|GNode void splitFamily(GNode* fam, GNode** prefn, GNode** phusb, GNode** pwife, GNode** pchil, GNode** prest)|Split a family GNode tree into parts. This neither allocates nor frees nodes.|
|void joinFamily (GNode *fam, GNode *refn, GNode *husb, GNode *wife, GNode *chil, GNode *rest)|Join a family GNode tree from parts.|
|GNode *normalizePerson(GNode *indi)|Normalize a person GNode tree by calling splitPerson and joinPerson.|
|GNode *normalizeFamily(GNode *fam)|Normalize a family GNode tree by calling splitFamily and joinFamily.|
|GNode *normalizeEvent(GNode *event)|Normalize an event GNode tree. *Currently a no-op*.|
|GNode *normalizeSource(GNode *source)|*Currently a no-op*.|
|GNode *normalizeOther(GNode *other)|*Currently a no-op*.|