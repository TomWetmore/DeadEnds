# Key Sorting

Many Gedcom files use person keys like @Innn@ and family keys like @Fnnn@ where n is an integer digit. When these keys are sorted alphabetically the results are problematical; for example @I100@ sorts before @I2@ and so on.

One can imagine Gedcom files in which these pseudo-numeric keys co-exist with other keys. When sorting sets of keys that contain both pseudo-numeric keys and purely string keys, does it make sense to sort using a custom-defined sort function?

Here is the official definition of a key (called a *cross reference identifier* using *Xref* as the symbol in the Gedcom grammar):

|||
|--:|:---|
|key:|@ keychar+ @|
|keychar:| [A_Z] \| [0-9] \| _|

Note that the non-@ part of a key can be a single character; the only letters allowed are uppercase ASCII; the underscore (U005F) and ten ASCII digits are also allowed.
DeadEnds has the Sequence data type that implements lists of persons and several operations on those lists. To perform some of the operations the sequences must be sorted, and the sorting is usually based on the record keys. The following compare function is used so the psuedo-number keys are sorted propertly among themselves.
```
//  compareRecordKeys -- Compare function for record keys.
//   Longer keys sort after shorter keys.

int compareRecordKeys(String a, String b)
{
    if (strlen(a) != strlen(b)) return (int) (strlen(a) - strlen(b));
    for (int i = 0; i < strlen(a); i++) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}
```
