## Command Interface

#### Goal

Define a command line interface to DeadEnds, and find a clean interface between genealogical operations as "model operations" that have interfaces that can be used by other user interface styles.

The first UI to LifeLines was a CLI. It served to develop and debug the rest of LifeLines. It was untlimately replaced with an ncurses interface.

My goal is to do the same for DeadEnds. Create a CLI interface that serves to help define the interface layer between the model and user interface.

What are the operations on the model.

Browsing. At the model layer given a name find the person with that name. Many persons may match the same name, so getPersonsWithName(String name) will return a List of GNode roots. Then selectPersonFromList(List) will have to be callable from within to .

```c
extern GNode* selectPersonFromList(RootList*);
extern GNode* getPersonWithName(String name, GNode*(*)(RootList*));
extern RootList* getPersonsWithName(String name);
extern GNode* getPersonWithKey(String key);
```

`PersonWithName(String name, selectPersonFromList);`

Editing. Seems an issue beacause editing can be done in so many ways. On quick thought this seems to be a big issue.

What the user wants to do: change the data. To change data they have to change data. How? There are maybe two kings of changes:

one. Those that manage the lineage linking, such as adding and removing to/from families. 

two. Directly changing non-linkage inforamtion &ndash; for example name strings, details of times, dates, and places.