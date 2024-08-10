//
//  personmenu.c
//  UseMenus
//
//  Created by Thomas Wetmore on 8/5/24.
//

#include "personmenu.h"

static int indiBrowse(GNode*);
static GNode* editIndi(GNode*);
static void message(String);

// personMenu is the active menu when viewing a person.
BrowseReturn personMenu(Database* database, GNode** pindi1, GNode** pindi2, GNode** pfam1, GNode** pfam2, Sequence* pseq) {
	String key, name, addstrings[2];
	int i, c, len, rc;
	GNode* node;
	GNode* save = null;
	GNode* person = *pindi1;
	//Sequence* sequence = null;
	char scratch[100];

	addstrings[0] = "Create a family with this person as a child.";
	addstrings[1] =  "Create a family with this person as a spouse/parent.";
	if (!person) return browseQuit;
	while (true) {
		c = indiBrowse(person);
		if (c != 'a') save = null;
		switch (c) {
		case 'e': // Edit this person.
			person = editIndi(person);
			break;
		case 'g': // Browse to person's family.
			//if (*pfam1 = choose_family(indi, ntprnt, idfbrs))
				//return browseFamily;
			//else
				//message(ntprnt);
			break;
		case 'f': // Browse to person's father.
			if (!(node = personToFather(person, database)))
				message("This person has no father in the database.");
			else
				person = node;
			break;
		case 'm':// Browse to person's mother */
			if (!(node = personToMother(person, database)))
				message("This person has no mother in the database.");
			else
				person = node;
			break;
		case 'z': // Zip browse another person.
			printf("Zip browse another person -- not implemented.\n");
			//node = ask_for_indi(idpnxt, false, false);
			//if (node) indi = node;
			break;
		case 's': // Browse to person's spouse.
			printf("Browse to a person's spouse -- not implemented.\n");
			//node = choose_spouse(indi, nospse, idsbrs);
			//if (node) indi = node;
			break;
		case 'c': // Browse to person's child.
			printf("Browse to a person's child -- not implemented.\n");
			//node = choose_child(indi, null, nocofp,
				//idcbrs, false);
			//if (node) indi = node;
			break;
		case 'p': // Switch to pedigree mode.
			*pindi1 = person;
			return browsePedigree;
		case 'o': // Browse to older sib.
			if (!(node = personToPreviousSibling(person, database)))
				message("This person has no next sibling in the database.");
			else
				person = node;
			break;
		case 'y':	/* Browse to younger sib */
			if (!(node = personToNextSibling(person, database)))
				message("This person has no previous sibling in the database.");
			else
				person = node;
			break;
		case 'u':	/* Browse to parents' family */
			if (!(*pfam1 = personToFamilyAsChild(person, database)))
				message("This person is not a child in any family.");
			else
				return  browseFamily;
			break;
		case 'b': 	/* Browse new list of persons */
//			seq = ask_for_indiseq(idplst, &rc);
//			if (!seq) break;
//			if ((len = length_indiseq(seq)) == 1) {
//				element_indiseq(seq, 0, &key, &name);
//				indi = key_to_indi(key);
//				remove_indiseq(seq, FALSE);
//				break;
//			}
//			*pseq = seq;
//			return BROWSE_LIST;
			break;
		case 'n':	/* Add new person */
			printf("Add new person -- not implemented.\n");
//			if (!(node = add_indi_by_edit())) break;
//			save = indi;
//			indi = node;
			break;
		case 'a':	/* Add family for current person */
			printf("Add family for current person -- not implemented.\n");
//			if (readonly) {
//				message(ronlya);
//				break;
//			}
//			i = choose_from_list(idfcop, 2, addstrings);
//			if (i == -1) break;
//			if (i == 0) node = add_family(NULL, NULL, indi);
//			else if (save) {
//				if (keyflag)
//					sprintf(scratch, "%s%s (%s)", issnew,
//							indi_to_name(save, 55),
//						rmvat(nxref(save))+1);
//				else
//					sprintf(scratch, "%s%s", issnew,
//						indi_to_name(save, 55));
//				if (ask_yes_or_no(scratch))
//					node = add_family(indi, save, NULL);
//				else
//					node = add_family(indi, NULL, NULL);
//			} else
//				node = add_family(indi, NULL, NULL);
//			save = NULL;
//			if (!node) break;
//			*pfam1 = node;
//			return BROWSE_FAM;
			break;
		case 't':	/* Switch to tandem browsing */
			printf("Switch to tandom browsing -- not implemented.\n");
//			node = ask_for_indi(idp2br, FALSE, FALSE);
//			if (node) {
//				*pindi1 = indi;
//				*pindi2 = node;
//				return browseTandem;
//			}
			break;
		case 'x': 	/* Swap families of current person */
			printf("Swap families of current person -- not implememented.\n");
			//swap_families(indi);
			break;
		case 'h': // Add person as spouse.
			printf("Add person as spouse -- not implemented.\n");
			//add_spouse(indi, NULL, TRUE);
			break;
		case 'i': // Add person as child.
			printf("Add person as child -- not implemented.\n");
			//add_child(indi, NULL);
			break;
		case 'r': // Remove person as spouse.
			printf("Remove person as spouse -- not implemented.\n");
			//remove_spouse(indi, NULL, FALSE);
			break;
		case 'd': // Remove person as child.
			printf("Remove person as child -- not implemened.\n");
			//remove_child(indi, FALSE);
			break;
		case 'q':
		default:
			return browseQuit;
		}
	}
}

static int indiBrowse(GNode* inti) {
	return 0;
}

static GNode* editIndi(GNode* indi) {
	return indi;
}

static void message(String msg) {
	fprintf(stderr, "%s\n", msg);
}
