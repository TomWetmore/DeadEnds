// DeadEnds
//
// splitjoin.c has the functions that split persons and families into parts and join them back
// together. Calling split and join formats GNode trees into standard form.
//
// Created by Thomas Wetmore on 7 November 2022.
// Last changed on 19 May 2024.

#include "standard.h"
#include "gnode.h"
#include "splitjoin.h"

// splitPerson splits a person GNode tree into parts.
void splitPerson(GNode* indi, GNode** pname, GNode** prefn, GNode** psex, GNode** pbody,
                 GNode** pfamc, GNode** pfams) {
    GNode *name, *lnam, *refn, *sex, *body, *famc, *fams, *last;
    GNode *lfmc, *lfms, *lref, *prev, *node;
    ASSERT(eqstr("INDI", indi->tag));
    name = sex = body = famc = fams = last = lfms = lfmc = lnam = null;
    refn = lref = null;
    node = indi->child;
    indi->child = indi->sibling = null;
    while (node) {
        String tag = node->tag;
        if (eqstr("NAME", tag)) {
            if (!name) name = lnam = node;
            else lnam = lnam->sibling = node;
        } else if (!sex && eqstr("SEX", tag)) {
            sex = node;
        } else if (eqstr("FAMC", tag)) {
            if (!famc) famc = lfmc = node;
            else lfmc = lfmc->sibling = node;
         } else if (eqstr("FAMS", tag)) {
            if (!fams) fams = lfms = node;
            else lfms = lfms->sibling = node;
         } else if (eqstr("REFN", tag)) {
            if (!refn) refn = lref = node;
            else lref = lref->sibling = node;
        } else {
            if (!body) body = last = node;
            else last = last->sibling = node;
        }
        prev = node;
        node = node->sibling;
        prev->sibling = null;
    }
    *pname = name;
    *prefn = refn;
    *psex = sex;
    *pbody = body;
    *pfamc = famc;
    *pfams = fams;
}

// joinPerson joins a person GNode tree from parts.
void joinPerson(GNode* indi, GNode* name, GNode* refn, GNode* sex, GNode* body, GNode* famc,
                 GNode* fams) {
    GNode *node = null;
    ASSERT(indi && eqstr("INDI", indi->tag));
    indi->child = null;
    if (name) {
        indi->child = node = name;
        while (node->sibling)
            node = node->sibling;
    }
    if (refn) {
        if (node) node = node->sibling = refn;
        else indi->child = node = refn;
        while (node->sibling) node = node->sibling;
    }
    if (sex) {
        if (node) node = node->sibling = sex;
        else indi->child = node = sex;
    }
    if (body) {
        if (node) node = node->sibling = body;
        else indi->child = node = body;
        while (node->sibling) node = node->sibling;
    }
    if (famc) {
        if (node) node = node->sibling = famc;
        else indi->child = node = famc;
        while (node->sibling) node = node->sibling;
    }
    if (fams) {
        if (node) node->sibling = fams;
        else indi->child = fams;
    }
}

// splitFamily splits a family GNode tree into parts.
void splitFamily(GNode* fam, GNode** prefn, GNode** phusb, GNode** pwife, GNode** pchil,
                  GNode** prest) {
    GNode *node, *rest, *last, *husb, *lhsb, *wife, *lwfe, *chil, *lchl;
    GNode *prev, *refn, *lref;
    String tag;
    rest = last = husb = wife = chil = lchl = lhsb = lwfe = null;
    prev = refn = lref = null;
    node = fam->child;
    fam->child = fam->sibling = null;
    while (node) {
        tag = node->tag;
        if (eqstr("HUSB", tag)) {
            if (husb)
                lhsb = lhsb->sibling = node;
            else
                husb = lhsb = node;
        } else if (eqstr("WIFE", tag)) {
            if (wife)
                lwfe = lwfe->sibling = node;
            else
                wife = lwfe = node;
        } else if (eqstr("CHIL", tag)) {
            if (chil)
                lchl = lchl->sibling = node;
            else
                chil = lchl = node;
        } else if (eqstr("REFN", tag)) {
            if (refn)
                lref = lref->sibling = node;
            else
                refn = lref = node;
        } else if (rest)
            last = last->sibling = node;
        else
            last = rest = node;
        prev = node;
        node = node->sibling;
        prev->sibling = null;
    }
    *prefn = refn;
    *phusb = husb;
    *pwife = wife;
    *pchil = chil;
    *prest = rest;
}

// joinFamily joins a family GNode tree from parts.
void joinFamily (GNode* fam, GNode* refn, GNode* husb, GNode* wife, GNode* chil, GNode* rest) {
    GNode *node = null;
    fam->child = null;
    if (refn) {
        fam->child = node = refn;
        while (node->sibling) node = node->sibling;
    }
    if (husb) {
        if (node) node = node->sibling = husb;
        else fam->child = node = husb;
        while (node->sibling) node = node->sibling;
    }
    if (wife) {
        if (node) node = node->sibling = wife;
        else fam->child = node = wife;
        while (node->sibling) node = node->sibling;
    }
    if (chil) {
        if (node) node = node->sibling = chil;
        else fam->child = node = chil;
		while (node->sibling) node = node->sibling;
    }
	if (rest) {
		if (node) node = node->sibling = rest;
		else fam->child = node = rest;
		while (node->sibling) node = node->sibling;
	}
}

// splitTree splits a non person or family tree into parts.
static void splitTree(GNode* root, GNode** prefn, GNode** prest) {
	GNode *node, *rest, *last;
	GNode *prev, *refn, *lref;
	String tag;
	rest = last = null;
	prev = refn = lref = null;
	node = root->child;
	root->child = root->sibling = null;
	while (node) {
		tag = node->tag;
		if (eqstr("REFN", tag)) {
			if (refn)
				lref = lref->sibling = node;
			else
				refn = lref = node;
		} else if (rest)
			last = last->sibling = node;
		else
			last = rest = node;
		prev = node;
		node = node->sibling;
		prev->sibling = null;
	}
	*prefn = refn;
	*prest = rest;
}

// joinTree joins a non person or family GNode tree from parts.
static void joinTree (GNode* root, GNode* refn, GNode* rest) {
	GNode *node = null;
	root->child = null;
	if (refn) {
		root->child = node = refn;
		while (node->sibling) node = node->sibling;
	}
	if (rest) {
		if (node) node = node->sibling = rest;
		else root->child = node = rest;
		while (node->sibling) node = node->sibling;
	}
}

// normalizeRecord normalizes GNode record trees to standard format.
GNode* normalizeRecord(GNode* root) {
	switch (recordType(root)) {
		case GRHeader: return root;
		case GRTrailer: return root;
		case GRPerson: return normalizePerson(root);
		case GRFamily:  return normalizeFamily(root);
		case GREvent: return normalizeEvent(root);
		case GRSource: return normalizeSource(root);
		case GROther: return normalizeOther(root);
		default: FATAL();
	}
	return null;
}

// normalizePerson puts a person GNode tree into standard format.
GNode* normalizePerson(GNode *indi) {
	GNode *names, *refns, *sex, *body, *famcs, *famss;
	splitPerson(indi , &names, &refns, &sex, &body, &famcs, &famss);
	joinPerson(indi, names, refns, sex, body, famcs, famss);
	return indi;
}

// normalizeFamily puts a family GNode tree into standard format.
GNode* normalizeFamily(GNode *fam) {
	GNode *refns, *husb, *wife, *chil, *body;
	splitFamily(fam, &refns, &husb, &wife, &chil, &body);
	joinFamily(fam, refns, husb, wife, chil, body);
	return fam;
}

// normalizeTree puts a non person or family tree into standard format.
static GNode* normalizeTree(GNode* root) {
	GNode* refns, *body;
	splitTree(root, &refns, &body);
	joinTree(root, refns, body);
	return root;
}

// normalizeEvent gets an event GNode tree into standard format; currently a no-op.
GNode* normalizeEvent(GNode* event) {
	return normalizeTree(event);
}

// normalizeSource gets a source GNode tree into standard format; currently a no-op.
GNode* normalizeSource(GNode* source) {
	return normalizeTree(source);
}

// normalizeOther gets an other GNode tree into to standard format; currently a no-op.
GNode* normalizeOther(GNode* other) {
	return normalizeTree(other);
}
