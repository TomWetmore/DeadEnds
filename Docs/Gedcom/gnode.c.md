# gnode.c

Description of gnode.c.

Components making up gnode.c:
|Component|Description|
|:---|:---|
|static StringTable *tagTable|Table of encountered tags. Used to store a single copy of each tag, shared by all GNodes with the tag.|
|static int nodeAllocs|The number of allocated nodes.|
|int numNodeAllocs(void)|Return the number of allocated nodes.|
|static int nodeFrees|The number of freed nodes.|
|int numNodeFrees(void)|Return the number of freed nodes.|
|static String fix_tag(String tag)|Return the copy of the tag from the tagTable. This allows all GNodes to share single copies of the tags.|
|void freeGNode(GNode\*)|Free a GNode.|
|GNode* createGNode(String key, String tag, String value, GNode* parent)|Create and initialize a GNode.|
|void freeGNodes(GNode* node)|Recursively free a tree of GNodes.|
|int gnodeLevel(GNode* node)|Return the level of the GNode in its tree.|
|GNode* fam_to_spouse(Database* database, GNode* fam, GNode* indi)|_Return the other spouse of a family. This function is out of place_.|
|String personToEvent(GNode* node, String tag, String head, int len, bool shorten)|Convert an event sub-tree into a string.|
|String eventToString (GNode* node, bool shorten)|Convert an event sub-tree to a string.|
|String eventToDate (GNode* node, bool shorten)|Return the date string of an event sub-tree.|
|String event_to_plac (GNode* node, bool shorten)|Return the place string of an event.|
|void show_node(GNode* node)|Show a GNode tree&mdash; for debugging.|
|void show_node_rec(int levl, GNode* node)|Recursive helper for show_node &mdash; for debugging.|
|int length_nodes(GNode* first)|Return the length of a list of nodes (using sibling links).|
|String shorten_date(String date)|Return the short form of a date.|
|String shorten_plac(String plac)|Return the short form of a place.|
|static bool all_digits(String s)|_Return if a string is all digits &mdash; commented out_.|
|GNode* copy_node(GNode\*)|Copy a GNode.|
|GNode* copy_nodes (GNode\*, bool kids, bool sibs)|Copy a GNode tree.|
|bool traverseNodes (GNode\*, bool (\*func)(GNode*))|Traverse a GNode tree calling a function on each node.|
|int countNodes(GNode* node)|Return the number of nodes in a GNode tree. _Recursive_.|
|GNode* find_node (GNode* parent, String tag, String value, GNode **plast)|Find child node with specific tag and value. Either or both of tag and value must be non-null.|
|GNode* fatherNodes (Database* database, GNode* faml)|Given a list of FAMS or FAMC nodes, return the list of HUSB nodes they contain.|
|GNode* mother_nodes (Database *database, GNode *faml)|Given a list of FAMS or FAMC nodes, return the list of WIFE nodes they contain.|
|GNode* children_nodes(Database\*, GNode *faml)|Given a list of FAMS or FAMC nodes, return the list of CHIL nodes they contain.|
|GNode* parents_nodes(Database\*, GNode* family)|Given a list of FAMS or FAMC nodes, return the list of HUSB and WIFE nodes they contain.|
|String addat (String)|Add at-signs to both ends of a string. *No longer used*.|
|String rmvat (String)|Remove at-signs from both ends of a string. *No longer used*.|
|bool isKey(String)|Return true if a string has at-signs at both ends.|
|GNode* findTag(GNode\*, String tag)|Search a list of nodes for the first with a specific tag. Return that node. If a node with the tag is not found, return null|
|SexType val_to_sex (GNode* node)|Convert value of node to enumerated sex value.|
|String full_value(GNode* node)|Return the full value of a GNode including continuation lines. _Doesn't work correctly._|






