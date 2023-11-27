# nodeutls.c
|Component|Description|
|:---|:---|
|GNode* unique_nodes(GNode* node, bool kids)|Remove duplicates a from list of GNodes. The original list is modified.|
|GNode* union_nodes(GNode* node1, GNode* node2, bool kids, bool copy)|Return the union of two GNode trees.|
|GNode* intersect_nodes (GNode* node1, GNode* node2, bool kids)|Return the intersection of two GNode trees.|
|void classify_nodes (GNode** pnode1, GNode** pnode2, GNode** pnode3)|Convert two value lists to three lists. The first returned holds all values that were only the first list. The second returned list holds all values that were only in the second original list. The third returned list holds all the values that were in both original lists.|
|GNode* difference_nodes (GNode* node1, GNode* node2, bool kids)|Return the difference of two GNode lists &mdash; all in the first list that are not in the second.|
|bool value_in_nodes(GNode* node, String value)|See if a list of GNodes contains a given value.|
|bool equal_tree (GNode* root1, GNode* root2)|See if two GNode trees are equal.|
|bool equal_node (GNode* node1, GNode* node2)|See if two GNodes are equal.|
|bool iso_list (GNode* root1, GNode* root2)|See if two GNode lists are isomorphic.|
|bool equal_nodes (GNode* root1, GNode* root2, bool kids, bool sibs)|See if two GNode structures are equal.|
|bool iso_nodes (GNode* root1, GNode *root2, bool kids, bool sibs)|See if two GNode structures are isomorphic.|