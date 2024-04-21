# Notes on Sorting



### Two Datatypes: List and Sequence



DeadEnds has two data types of interest for this note on sorting and searching, the List and Sequence types. (I hope to unify the two eventually). Both can be sorted and binary searched. 

There are two ways available for sorting and I am highly conflicted as to which to use. Since the beginning of LifeLines all sorting and searching has been done using my custom quick sort and binary search functions. I hate debugging the sort code .

The C "syslib" library also provides a *qsort* function (as well as a heap and merge sort), and a *bsearch* binary search function.

Both sort functions require a compare function to be provided to determine the order relationship between any two elements.

There is a difference between the compare functions. In the List and Sequence types, the actual lists are sequences of pointers, called Words, which are void\* pointers. The different types of Lists are distinguised  by the data types the element pointers point to. The LifeLines/DeadEnds compare function is passed the values of a pair of elements to compare. So those values are pointers to data structures held by the List (e.g., roots of records, strings, names).

However the system qsort function requires a compare function this a level of indirection further away. It is passed the addresses of the two elements to be compared, not the values of those two elements. I didnt' think this would be much of a problem, other than the wholesale changes to be made, but I am now not so sure.

Searching of sorted Lists and Sequences is done with binary search. There is original LifeLines/DeadEnds version and the "syslib" version. I much prefer the DeadEnds version because when it fails to find the targe value it returns the index in the List where it would the element would have been, great for applciations that add elements to sorted lists when they are not there yet.

The binary search routines must also use a compare function. If I were to use the system sort and the DeadEnds binary search, the binary search would have to change in order to avoid having two compare routines. And frankly I don't want to do that. 

So the decision I have to make is whether to continue using the original quick sort and binary search routines, or to switch over to the "syslib" versions.

The advantage of the original versions boils down almost entirely to the much easier to comprehend and easier to write compare functions. I see two disadvantages of this approach. It may not be as debugged as the system version, and the system version may be optimized to better handle some of the worst cases that can be found with partition sorts when the elements are mostly sorted to begin with.

The advantages of the "syslib" approach are the fact that library functions are much more fully tested, which is a big one. Plus using system libraries is "what should be done." The disadvantage, which may be only a personal thing, is the complexity that I am going to have to put into the compare functions, and how much debugging I am going to have to do to get all the list types working.