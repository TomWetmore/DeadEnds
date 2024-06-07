//
//  createfamily.c
//  CloneOne
//
//  Created by Thomas Wetmore on 5/30/24.
//

#include "gnode.h"
#include "gedcom.h"
#include "splitjoin.h"

static bool checkFamilyMember(GNode*, SexType);

// createFamily creates a new family record. It does not add the family to the database.
GNode* createFamily(GNode* husb, GNode* wife, GNode* chil, GNode* rest, Database* database) {
	if (!husb && !wife && !chil) return false;
	checkFamilyMember(husb, sexMale);
	checkFamilyMember(wife, sexFemale);
	checkFamilyMember(chil, sexUnknown);
	GNode* family = createGNode(generateFamilyKey(database), "FAM", null, null);
	joinFamily(family, null, husb, wife, chil, rest);
	return family;
}

// checkFamilyMember checks if a person can be added to a new family.
static bool checkFamilyMember(GNode* person, SexType sex) {
	if (!person) return true;
	if (nestr(person->tag, "INDI")) return false;
	if (sex == sexUnknown) return true;
	GNode* snode = findTag(person, "SEX)");
	if (!snode || !snode->value || nestr(snode->value, sexTypeToString(sex))) return false;
	return true;
}

//
///*=========================================
// * add_family -- Add new family to database
// *=======================================*/
//NODE add_family (spouse1, spouse2, child)
//NODE spouse1, spouse2, child;
//{
//	INT sex1, sex2;
//	NODE fam1, fam2, refn, husb, wife, chil, body;
//	NODE name, sex, famc, fams, node, prev, new, this;
//	TRANTABLE tti = tran_tables[MEDIN], tto = tran_tables[MINED];
//	STRING xref, msg, key;
//	BOOLEAN emp;
//	FILE *fp;
//
//	if (readonly) {
//		message(ronlya);
//		return NULL;
//	}
//
///* Handle case where child is known */
//
//	if (child)  {
//		spouse1 = spouse2 = NULL;
//		goto editfam;
//	}
//
///* Identify first spouse */
//
//	if (!spouse1) spouse1 = ask_for_indi(idsps1, FALSE, FALSE);
//	if (!spouse1) return NULL;
//	if ((sex1 = SEX(spouse1)) == SEX_UNKNOWN) {
//		message(unksex);
//		return NULL;
//	}
//
///* Identify optional spouse */
//
//	if (!spouse2) spouse2 = ask_for_indi(idsps2, FALSE, TRUE);
//	if (spouse2) {
//		if ((sex2 = SEX(spouse2)) == SEX_UNKNOWN || sex1 == sex2) {
//			message(notopp);
//			return NULL;
//		}
//	}
//
///* Create new family */
//
//editfam:
//	fam1 = create_node(NULL, "FAM", NULL, NULL);
//	husb = wife = chil = NULL;
//	if (spouse1) {
//		if (sex1 == SEX_MALE)
//			husb = create_node(NULL, "HUSB", nxref(spouse1), fam1);
//		else
//			wife = create_node(NULL, "WIFE", nxref(spouse1), fam1);
//	}
//	if (spouse2) {
//		if (sex2 == SEX_MALE)
//			husb = create_node(NULL, "HUSB", nxref(spouse2), fam1);
//		else
//			wife = create_node(NULL, "WIFE", nxref(spouse2), fam1);
//	}
//	if (child)
//		chil = create_node(NULL, "CHIL", nxref(child), fam1);
//
///* Prepare file for user to edit */
//
//	ASSERT(fp = fopen(editfile, LLWRITETEXT));
//	write_nodes(0, fp, tto, fam1, TRUE, TRUE, TRUE);
//	write_nodes(1, fp, tto, husb, TRUE, TRUE, TRUE);
//	write_nodes(1, fp, tto, wife, TRUE, TRUE, TRUE);
//	fprintf(fp, "1 MARR\n  2 DATE\n  2 PLAC\n  2 SOUR\n");
//	write_nodes(1, fp, tto, chil, TRUE, TRUE, TRUE);
//	fclose(fp);
//	join_fam(fam1, NULL, husb, wife, chil, NULL);
//
///* Have user edit family info */
//
//	do_edit();
//	while (TRUE) {
//		fam2 = file_to_node(editfile, tti, &msg, &emp);
//		if (!fam2) {
//			if (ask_yes_or_no_msg(msg, fredit)) {
//				do_edit();
//				continue;
//			}
//			break;
//		}
//		if (!valid_fam(fam2, &msg, fam1)) {
//			if (ask_yes_or_no_msg(msg, fredit)) {
//				do_edit();
//				continue;
//			}
//			free_nodes(fam2);
//			fam2 = NULL;
//			break;
//		}
//		break;
//	}
//
///* Confirm family add operation */
//
//	free_nodes(fam1);
//	if (!fam2 || !ask_yes_or_no(cffadd)) {
//		free_nodes(fam2);
//		return NULL;
//	}
//	nxref(fam2) = strsave(xref = getfxref());
//
///* Modify spouse/s and/or child */
//
//	if (spouse1) {
//		new = create_node(NULL, "FAMS", xref, spouse1);
//		prev = NULL;
//		node = nchild(spouse1);
//		while (node) {
//			prev = node;
//			node = nsibling(node);
//		}
//		if (prev)
//			nsibling(prev) = new;
//		else
//			nchild(spouse1) = new;
//	}
//	if (spouse2) {
//		new = create_node(NULL, "FAMS", xref, spouse2);
//		prev = NULL;
//		node = nchild(spouse2);
//		while (node) {
//			prev = node;
//			node = nsibling(node);
//		}
//		if (prev)
//			nsibling(prev) = new;
//		else
//			nchild(spouse2) = new;
//	}
//	if (child) {
//		split_indi(child, &name, &refn, &sex, &body, &famc, &fams);
//		new = create_node(NULL, "FAMC", xref, child);
//		prev = NULL;
//		this = famc;
//		while (this) {
//			prev = this;
//			this = nsibling(this);
//		}
//		if (prev)
//			nsibling(prev) = new;
//		else
//			famc = new;
//		join_indi(child, name, refn, sex, body, famc, fams);
//	}
//
///* Write updated records to database */
//
//	split_fam(fam2, &refn, &husb, &wife, &chil, &body);
//	key = rmvat(nxref(fam2));
//	for (node = refn; node; node = nsibling(node))
//		if (nval(node)) add_refn(nval(node), key);
//	join_fam(fam2, refn, husb, wife, chil, body);
//	resolve_links(fam2);
//	resolve_links(spouse1);
//	resolve_links(spouse2);
//	resolve_links(child);
//	fam_to_dbase(fam2);
//	fam_to_cache(fam2);
//	if (spouse1) indi_to_dbase(spouse1);
//	if (spouse2) indi_to_dbase(spouse2);
//	if (child) indi_to_dbase(child);
//	message(gdfadd);
//	return fam2;
//}
//
