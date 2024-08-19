# Key Sorting

Gedcom files sometimes use person keys like @Innn@ and family keys like @Fnnn@ where n is an integer digit. When these keys are sorted alphabetically the results are problematical; for example @I100@ sorts before @I2@ and so on. Though this can easily be solved.

But Gedcom keys can have many formats. A key is called a *cross reference identifier*, using *Xref* as its symbol in the Gedcom grammar. Here is its lexical definition:

|||
|--:|:---|
|key:|@ keychar+ @|
|keychar:| [A_Z] \| [0-9] \| _|

The non-@ part of a key can be a single character; the characters allowed are uppercase ASCII letters, the underscore (U005F) and the ten ASCII digits.
DeadEnds has the Sequence data type that implements lists of persons and several operations on those lists. To perform some of the operations the sequences must be sorted, and the sorting is often based on the record keys. The following compare function is used so the psuedo-number keys are sorted propertly among themselves.

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
