# Set

A Set consists of a sortable List. The elements can be anything referred to by a Word. When a Set is created there must be a compare function that computes the order relationship between two Set elements. The elements are kept unique via the compare function. Set uses a sortable List to hold the elements. Elements are pointers to objects the caller defines. The caller may provide a delete function to dispose the elements when the set is removed. Duplicate elements, as detected by the compare function, are not added.
```
typedef struct Set {
    List *list;  // Sortable list.
} Set;
```

|Component|Description|
|:--|:-|
|Set \*createSet(int (\*compare)(Word, Word), void (\*delete)(Word), String(\*getKey)(Word))|Create a new Set.|
|void deleteSet(Set\*)|Delete a Set.|
|void addToSet(Set\*, Word)|Add an element to a Set if it is not already there.|
|bool isInSet(Set\*, Word)|Check if an element is in a Set.|
|void removeFromSet(Set\*, Word)|Remove an element from a set. _Shouldn't this use removeFromList_?|
|void iterateSet(Set\*, void (*iterate)(Word))|Iterate the elements of a set, calling a function on each.|
|int lengthSet(Set\*)|Return the number of elements in a set.|
|void showSet(Set\*, String (*describe)(Word))|Show the contents of a set using a describe function.|