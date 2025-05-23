// DeadEnds
//
// nodeutils.c has GNode utility functions.
//
// Created by Thomas Wetmore on 12 November 2022.
// Last changed on 21 May 2024.

#include "standard.h"
#include "gnode.h"
#include "nodeutils.h"

// uniqueNodes removes duplicates a from list of nodes. If kids is true children matter.
GNode* uniqueNodes(GNode* node, bool kids) {
	GNode *node0 = node, *next;
	if (!node) return null;
	while (node) {
		GNode* prev = node;
		GNode* this = node->sibling;
		while (this) {
			if (isoGNodes(node, this, kids, false)) {
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

// unionNodes returns the union of two GNode trees. If kids is true children matter; if copy is
// true copy the operands.
GNode* unionNodes(GNode* node1, GNode* node2, bool kids, bool copy) {
	GNode *curs1, *next1, *prev1, *curs2, *prev2;
	if (copy) node1 = copyGNodes(node1, true, true);
	if (copy) node2 = copyGNodes(node2, true, true);
	prev2 = null;
	curs2 = node2;
	while (curs2) {
		prev1 = null;
		curs1 = node1;
		while (curs1 && !isoGNodes(curs1, curs2, kids, false)) {
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
	node1 = copyGNodes(node1, true, true);
	node2 = copyGNodes(node2, true, true);
	node3 = curs3 = null;

	prev1 = null;
	curs1 = node1;
	while (curs1) {
		prev2 = null;
		curs2 = node2;
        while (curs2 && !isoGNodes(curs1, curs2, kids, false)) {
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

// classifyNodes converts two GNode lists to three. The first holds the values that were only in
// the first list. The second holds the values that were only in the second list. The third holds
// all the values that were in both original lists. The input lists are modified.
void classifyNodes (GNode** pnode1, GNode** pnode2, GNode** pnode3) {
	GNode *node1, *node2, *node3, *curs3;
	GNode *prev1, *prev2, *next2;
	GNode* curs1 = node1 = uniqueNodes(*pnode1, false);
	GNode* curs2 = node2 = uniqueNodes(*pnode2, false);
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

// difference_nodes returns the difference of two GNode lists, all in node1 that are not in
// node2. If kids is true children matter.
GNode* difference_nodes (GNode* node1, GNode* node2, bool kids) {
	GNode *prev1, *next1, *curs1, *curs2;
	node1 = copyGNodes(node1, true, true);
	prev1 = null;
	curs1 = node1;
	while (curs1) {
		curs2 = node2;
		while (curs2 && !isoGNodes(curs1, curs2, kids, false))
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

// valueInNodes checks if a list of GNodes contains a given value. Not used.
bool valueInNodes(GNode* node, String value) {
	while (node) {
		if (eqstr(value, node->value)) return true;
		node = node->sibling;
	}
    return false;
}


// equalTree checks if two GNode trees are equal.
bool equalTree(GNode* root1, GNode* root2) {
    String str1, str2;
    if (!root1 && !root2) return true;
    if (!root1 || !root2) return false;
    if (lengthGNodes(root1) != lengthGNodes(root2)) return false;
    while (root1) {
        if (nestr(root1->tag, root2->tag)) return false;
        str1 = root1->value;
        str2 = root2->value;
        if (str1 && !str2) return false;
        if (str2 && !str1) return false;
        if (str1 && str2 && nestr(str1, str2)) return false;
        if (!equalTree(root1->child, root2->child)) return false;
        root1 = root1->sibling;
        root2 = root2->sibling;
    }
    return true;
}

// equalNode checks if two GNodes are equal.
bool equalNode (GNode* node1, GNode* node2) {
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

// isoList checks if two node lists are isomorphic.
bool isoList(GNode* root1, GNode* root2) {
    int len1, len2;
    GNode *node1, *node2;
    if (!root1 && !root2) return true;
    if (!root1 || !root2) return false;
    len1 = lengthGNodes(root1);
    len2 = lengthGNodes(root2);
    if (len1 != len2) return false;
    if (len1 == 0) return true;
    node1 = root1;
    while (node1) {
        node2 = root2;
        while (node2) {
            if (equalNode(node1, node2))
                break;
            node2 = node2->sibling;
        }
        if (!node2) return false;
        node1 = node1->sibling;
    }
    return true;
}

// equalNodes checks if two GNode structures are equal; if kids and sibs are true include them
// in the check.
bool equalNodes(GNode* root1, GNode* root2, bool kids, bool sibs) {
    if (!root1 && !root2) return true;
    while (root1) {
        if (!equalNode(root1, root2)) return false;
        if (kids && !equalNodes(root1->child, root2->child, 1, 1))
            return false;
        if (!sibs) return true;
        root1 = root1->sibling;
        root2 = root2->sibling;
    }
    return (root2 == null);
}

// isoGNodes checks if two GNode trees or forests are isomorphic; if kids and sibs are true
// include them in the check.
bool isoGNodes(GNode* root1, GNode *root2, bool kids, bool sibs) {
    if (!root1 && !root2) return true;
    if (!root1 || !root2) return false;
    if (!kids && !sibs) return equalNode(root1, root2);
    if (kids && !sibs) return equalNode(root1, root2) && isoGNodes(root1->child, root2->child, 1, 1);
    int len1 = lengthGNodes(root1);
    int len2 = lengthGNodes(root2);
    if (len1 != len2) return false;
    if (len1 == 0) return true;
    GNode* node1 = root1;
    while (node1) { // Can be in any order. N*N algorithm.
        GNode* node2 = root2;
        while (node2) {
            if (isoGNodes(node1, node2, kids, 0)) break;
            node2 = node2->sibling;
        }
        if (!node2) return false;
        node1 = node1->sibling;
    }
    return true;
}
