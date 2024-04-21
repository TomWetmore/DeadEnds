# sequence

A Sequence is a data type defined in the Interp library that provides the sequence and set of persons functionality. Sequences are used by the SET functions of the report programming language.

| Component | Description |
|:---|:---|
|Sequence *createSequence (void)| Create a new Sequence on the heap.|
|int lengthSequence (Sequence\*)|Return the length of a Sequence.|
|void deleteSequence (Sequence\*sequence, bool fvalue)|Remove a Sequence from the heap. *TODO*: May need a better system for freeing the elements. Boolean *fvalue* means that the values should be deleted when the elements are deleted.|
|Sequence *copySequence (Sequence\*)|Return a copy of a Sequence. *NOTE*: I don't think this is being called.|
|void appendToSequence (Sequence\*, String key, String name, PValue *val)|Create and append a new element to a Sequence.|
|void rename_indiseq (Sequence\*, String key)|Update element name with standard name. *I don't know what this is for, and the word indiseq is old*.|
|bool isInSequence (Sequence\*, String key)|See if an element with the given key is in the Sequence.|
|bool  (Sequence\*, String key, String name, int index)|Remove an element from the Sequence. If key is not null use it to find the element. Othewise use the index. The element must be a person.|
|bool element_indiseq (Sequence\*, int index, String* pkey, String* pname)|Return an element (via output arguments) from a sequence.|
|static int nameCompare (SequenceEl el1, SequenceEl el2)|Compare two Sequence elments by name.|
|static int keyCompare(SequenceEl el1, SequenceEl el2)|Compare two Sequence elements by integer key.|
|static int valueCompare (SequenceEl el1, SequenceEl el2)|Compare two Sequence elements by their values. *TODO: This must be converted to using PValues for the values*.|
|void nameSortSequence (Sequence\*)|Sort the Sequence by the name fields of the elements. *TODO: What if the name fields don't have values yet?*|
|void keySortSequence (Sequence*)|Sort the Sequence by key value.|
|void valueSortSequence(Sequence\*)|Sort the Sequence by associated value.|
|static void sequenceSort (Word* data, int length, int(*compare)(Word, Word))|Sort the Sequence using the given compare function.|
|Sequence *uniqueSequence(Sequence\*)|Create and return a new Sequence that contains the unique elements from the given Sequence. Uniqueness is defined by the key integer.|
|void uniqueSequenceInPlace (Sequence\*)|Remove duplicate (have the same key) elements from the Sequence. No new Sequence is created. *This has a MEMORY LEAK &mdash; the elements removed are not managed properly*.|
|Sequence *unionSequence (Sequence\*, Sequence\*)|Create and return the union (based on keys) of the two Sequences. The two Sequences are key sorted and uniqued if necessary.|
|Sequence \*intersectSequence (Sequence\*, Sequence\*)|Create and return the intersection (based on keys) of the two Sequences. The two Sequences are key sorted and uniqued if necessary.|
|Sequence \*differenceSequence (Sequence\*, Sequence\*)|Create and return the difference (based on keys) of the two Sequences. The two Sequences are key sorted and uniqued if necessary.|
|Sequence \*parentSequence (Sequence\*)|Create and return the Sequence with all the parents of the persons in the given Sequence.|
|Sequence \*childSequence (Sequence\*)|Create and return the Sequence with all the children of the persons in the given Sequence.|
|Sequence \*personToChildren (GNode\* person)|Create and return the Sequence of the person's children.|
|Sequence \*personToSpouses (GNode \*person)|Create and return the Sequence of the person's spouses.|
|Sequence \*personToFathers (GNode  \*person)|Create and return the Sequence of the person's fathers.|
|Sequence \*personToMothers (GNode \*person)|Create and return the Sequence of the person's mothers.|
|Sequence \*personToFamilies (GNode \*person, bool fams)|Create and return the Sequence of a person's families. If the boolean is true the families are the families the person is a spouse in, else they are the families the person is a child in. *This function creates a Sequence of families, not persons.*|
|Sequence \*familyToChildren(GNode* family)|Create and return the Sequence of children in the family.|
|Sequence \*familyToFathers(GNode* family)|Create and return the Sequence of persons (usually one) who are fathers in the family. Return null if there is no father in the family.|
|Sequence \*familyToMothers(GNode \*family)|Create and return the Sequence of persons (usually one) who are mothers in the family. Return null if there is no moather in the family.|
|Sequence \*siblingSequence(Sequence\*, bool close)|Create the sibling Sequence of the given Sequence. If *close* is true include the siblings in the given Sequence.|
|Sequence *ancestorSequence(Sequence\*)|Create the ancestor Sequence of a given Sequence. The persons in the original Sequence are not in the ancestor Sequence unless they are also an ancestor of someone in the original Sequence.|
|Sequence *descendentSequence(Sequence\*)|Create the descendant Sequence of the given Sequence. The persons in the original Sequence are not in the descendent Sequence unless they are also a descendent of someone in the original Sequence.|
|Sequence \*spouseSequence(Sequence\*)|Create the spouses Sequence of a Sequence|
|void sequenceToGedcom(Sequence\*, FILE*)|Generate Gedcom file from a sequence. Only persons in the sequence are written to the file. Families with at least two persons in the sequence are also written to the file. Other persons referred to in the families are not included.|
|Sequence \*nameToSequence(String name, NameIndex\*, Database\*)|Return the Sequence of persons who match a name. The name must be formatted as a Gedcom name. However, if the first letter of the given names is a '*', the given name is treated as a wild card, and the Sequencce will contain all persons that match the surname.|
|static void format_indiseq (Sequence\*, bool famp, bool marr)|Format print lines of Sequence. *Commented out*.|
|Sequence *refn_to_indiseq (ukey)|Return indiseq whose user references match. *Commented out*.|
|Sequence \*key_to_indiseq(String name)|Return person Sequence of the matching key. *Commented out*.|
|Sequence *str_to_indiseq (String name)|Return person Sequence matching a string. Search precedence is 1. named indiset; 2 key with or without leading 'I'; 3. REFN value; 4. name. *Commented out*.|