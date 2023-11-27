//
//  DeadEnds
//
//  nodeutils.c
//
//  Created by Thomas Wetmore on 12 November 2022.
//  Last changed on 8 February 2023.
//

#include "standard.h"
#include "gnode.h"
#include "nodeutils.h"


// unique_nodes -- Remove duplicates a from list of nodes. The original list is modified.
//--------------------------------------------------------------------------------------------------
GNode* unique_nodes(GNode* node, bool kids)
// Node node -- First Node in list of Nodes.
// bool kids -- Children matter.
{
	GNode *node0 = node, *prev, *this, *next;

	if (!node) return null;
	while (node) {
		prev = node;
		this = node->sibling;
		while (this) {
			if (iso_nodes(node, this, kids, false)) {
				prev->sibling = next = this->sibling;
				this->sibling = null;
				freeGNodes(this);
				this = next;
			} else {
				prev = this;
				this = this->sibling;
			}
		}
		node = node->sibling;
	}
	return node0;
}

// union_nodes -- Return the union of two Node trees.
//--------------------------------------------------------------------------------------------------
GNode* union_nodes(GNode* node1, GNode* node2, bool kids, bool copy)
//  node1, node2;
//  kids -- children matter
//  copy -- copy operands first
{
	GNode *curs1, *next1, *prev1, *curs2, *prev2;

	if (copy) node1 = copy_nodes(node1, true, true);
	if (copy) node2 = copy_nodes(node2, true, true);
	prev2 = null;
	curs2 = node2;
	while (curs2) {
		prev1 = null;
		curs1 = node1;
		while (curs1 && !iso_nodes(curs1, curs2, kids, false)) {
			prev1 = curs1;
			curs1 = curs1->sibling;
		}
		if (curs1) {
			next1 = curs1->sibling;
			curs1->sibling = null;
			freeGNodes(curs1);
			if (prev1)
				prev1->sibling = next1;
			else
				node1 = next1;
		}
		prev2 = curs2;
		curs2 = curs2->sibling;
	}
	if (prev2) {
		prev2->sibling = node1;
		return node2;
	}
	return node1;
}

//  intersect_nodes -- Return the intersection of two Node trees.
//--------------------------------------------------------------------------------------------------
GNode* intersect_nodes (GNode* node1, GNode* node2, bool kids)
// Node node1, node2;
// bool kids -- Children matter.
{
	GNode *prev1, *curs1, *next1, *prev2, *curs2, *next2;
	GNode *node3, *curs3;

	if (!node1 || !node2) return null;
	node1 = copy_nodes(node1, true, true);
	node2 = copy_nodes(node2, true, true);
	node3 = curs3 = null;

	prev1 = null;
	curs1 = node1;
	while (curs1) {
		prev2 = null;
		curs2 = node2;
        while (curs2 && !iso_nodes(curs1, curs2, kids, false)) {
			prev2 = curs2;
			curs2 = curs2->sibling;
		}
		if (curs2) {
			next2 = curs2->sibling;
			curs2->sibling = null;

			if (node3)
				curs3 = curs3->sibling = curs2;
			else
				node3 = curs3 = curs2;
			if (prev2)
				prev2->sibling = next2;
			else
				node2 = next2;

			next1 = curs1->sibling;
			curs1->sibling = null;
			freeGNodes(curs1);
			if (prev1)
				prev1->sibling = next1;
			else
				node1 = next1;
			curs1 = next1;

		} else {
			prev1 = curs1;
			curs1 = curs1->sibling;
		}
	}
	freeGNodes(node1);
	freeGNodes(node2);
	return node3;
}

// classify_nodes -- Convert two value lists to three lists. The first returned holds all values
// that were only the first list. The second returned list holds all values that were only in
// the second original list. The third returned list holds all the values that were in both
// original lists.
//--------------------------------------------------------------------------------------------------
void classify_nodes (GNode** pnode1, GNode** pnode2, GNode** pnode3)
// Node* pnode1 -- (in,out)
// Node* pnode2 -- (in, out)
// Node* pnode3 -- (out)
{
	GNode *node1, *node2, *node3, *curs1, *curs2, *curs3;
	GNode *prev1, *prev2, *next2;

	curs1 = node1 = unique_nodes(*pnode1, false);
	curs2 = node2 = unique_nodes(*pnode2, false);
	curs3 = node3 = prev1 = prev2 = null;

	while (curs1 && curs2) {
		if (eqstr(curs1->value, curs2->value)) {
			if (node3)
				curs3 = curs3->sibling = curs1;
			else
				node3 = curs3 = curs1;

			curs1 = curs1->sibling;
			curs3->sibling = null;

			if (prev1)
				prev1->sibling = curs1;
			else
				node1 = curs1;

			next2 = curs2->sibling;
			if (prev2)
				prev2->sibling = next2;
			else
				node2 = next2;
			curs2->sibling = null;
			freeGNodes(curs2);
			curs2 = next2;
			continue;
		}
		prev2 = curs2;
        if ((curs2 = curs2->sibling)) continue;
		prev1 = curs1;
		curs1 = curs1->sibling;
		prev2 = null;
		curs2 = node2;
	}
	*pnode1 = node1;
	*pnode2 = node2;
	*pnode3 = node3;
}

//  difference_nodes -- Return the difference of two node lists -- all in node1 that are not in
//    node2.
//--------------------------------------------------------------------------------------------------
GNode* difference_nodes (GNode* node1, GNode* node2, bool kids)
//  node1, node2;
//  kids;	/* children matter */
{
	GNode *prev1, *next1, *curs1, *curs2;
	node1 = copy_nodes(node1, true, true);
	prev1 = null;
	curs1 = node1;
	while (curs1) {
		curs2 = node2;
		while (curs2 && !iso_nodes(curs1, curs2, kids, false))
			curs2 = curs2->sibling;
		if (curs2) {
			next1 = curs1->sibling;
			curs1->sibling = null;
			freeGNodes(curs1);
			if (!prev1)
				node1 = next1;
			else
				prev1->sibling = next1;
			curs1 = next1;
		} else {
			prev1 = curs1;
			curs1 = curs1->sibling;
		}
	}
	return node1;
}

//  value_in_nodes -- See if a list of nodes contains a given value.
//--------------------------------------------------------------------------------------------------
bool value_in_nodes(GNode* node, String value)
//Node node;
//String value;
{
	while (node) {
		if (eqstr(value, node->value)) return true;
		node = node->sibling;
	}
    return false;
}


//  equal_tree -- See if two node trees are equal
//--------------------------------------------------------------------------------------------------
bool equal_tree (GNode* root1, GNode* root2)
//Node root1, root2;
{
    String str1, str2;
    if (!root1 && !root2) return true;
    if (!root1 || !root2) return false;
    if (length_nodes(root1) != length_nodes(root2)) return false;
    while (root1) {
        if (nestr(root1->tag, root2->tag)) return false;
        str1 = root1->value;
        str2 = root2->value;
        if (str1 && !str2) return false;
        if (str2 && !str1) return false;
        if (str1 && str2 && nestr(str1, str2)) return false;
        if (!equal_tree(root1->child, root2->child)) return false;
        root1 = root1->sibling;
        root2 = root2->sibling;
    }
    return true;
}

//  equal_node -- See if two GNodes are equal.
//--------------------------------------------------------------------------------------------------
bool equal_node (GNode* node1, GNode* node2)
// GNode node1, node2 -- Two GNodes to check for equality.
{
    String str1, str2;
    if (!node1 && !node2) return true;
    if (!node1 || !node2) return false;
    if (nestr(node1->tag, node2->tag)) return false;
    str1 = node1->value;
    str2 = node2->value;
    if (str1 && !str2) return false;
    if (str2 && !str1) return false;
    if (str1 && str2 && nestr(str1, str2)) return false;
    return true;
}

//  iso_list -- See if two node lists are isomorphic
//--------------------------------------------------------------------------------------------------
bool iso_list (GNode* root1, GNode* root2)
//Node root1, root2;
{
    int len1, len2;
    GNode *node1, *node2;
    if (!root1 && !root2) return true;
    if (!root1 || !root2) return false;
    len1 = length_nodes(root1);
    len2 = length_nodes(root2);
    if (len1 != len2) return false;
    if (len1 == 0) return true;
    node1 = root1;
    while (node1) {
        node2 = root2;
        while (node2) {
            if (equal_node(node1, node2))
                break;
            node2 = node2->sibling;
        }
        if (!node2) return false;
        node1 = node1->sibling;
    }
    return true;
}

//  equal_nodes -- See if two node structures are equal.
//--------------------------------------------------------------------------------------------------
bool equal_nodes (GNode* root1, GNode* root2, bool kids, bool sibs)
//Node root1, root2;
//bool kids, sibs;
{
    if (!root1 && !root2) return true;
    while (root1) {
        if (!equal_node(root1, root2)) return false;
        if (kids && !equal_nodes(root1->child, root2->child, 1, 1))
            return false;
        if (!sibs) return true;
        root1 = root1->sibling;
        root2 = root2->sibling;
    }
    return (root2 == null);
}

//  iso_nodes -- See if two node structures are isomorphic.
//--------------------------------------------------------------------------------------------------
bool iso_nodes (GNode* root1, GNode *root2, bool kids, bool sibs)
//Node root1, root2;
//bool kids, sibs;
{
    int len1, len2;
    GNode *node1, *node2;

    if (!root1 && !root2) return true;
    if (!root1 || !root2) return false;

    if (!kids && !sibs) return equal_node(root1, root2);
    if ( kids && !sibs)
        return equal_node(root1, root2) && iso_nodes(root1->child, root2->child, 1, 1);

    len1 = length_nodes(root1);
    len2 = length_nodes(root2);
    if (len1 != len2) return false;
    if (len1 == 0) return true;

    node1 = root1;
    while (node1) {
        node2 = root2;
        while (node2) {
            if (iso_nodes(node1, node2, kids, 0))
                break;
            node2 = node2->sibling;
        }
        if (!node2) return false;
        node1 = node1->sibling;
    }
    return true;
}

