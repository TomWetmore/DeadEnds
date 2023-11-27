# Importing

These are issues I am worrying about concerning the importing of Gedcom files and the following processing required until there is a valid database ready for whatever comes next.

Importing in DeadEnds spans a number of steps.

First comes reading the Gedcom file into a database. In the current implementation only syntax checking is done at this point. I thought it might be ovderkill to put the data into a database before validation is done, but the in-RAM database is so lightweight that it is not an issue.

The key strings are accepted as-is during the first step, whatever they may be. They have to be unique, of course, but not restricted further. If there are lexical/syntax errors, the program cannot continue until the file is fixed.

I keep going back forth with the idea of rekeying the entire database with "normalized" keys. Is is a good idea? Is in unnecssary? The reason I keep thinking about it revolves are sorting keys that are "pseudo-numeric", that is of the form I### and F### and so on.
After reading the Gedcom file the Database holds record indexes of the major record types. A record index maps record keys (currently with the @-signs removed, [is this a reasonable idea?]) to the records themselves, which are GNode trees, one GNode per Gedcom line.

Next, all the semantic checks must be done, making sure all references from persons to families, and families to persons, and persons and families and others to sources, are valid and "closed." When problems are found they must are reported, and programs needing valid databases cannot continue.

Next, if being don, this is the point where the keys could be "normalized," that is, the person keys could be put in I### form, and the families put in F### form. (Keys "belong" to Gedcom, not to the user, so it is okay to change them.) I believe that if the keys are in this form certain operations, especially those on sequences, can be made more efficient. Maybe it's not time to worry about this.

When does name indexing occur? Now it is done in the first step, though it should not be done until all key changes have occurred, since name indexing does considerable key processing. It should be a standalone operation anyway, so where it is done is a matter of later concern.

What are new issues that occur when more than one Gedcom file is read? I can see two issues of concern.

First, overlapping key strings. If remapping keys becomes a regular part of Gedcom file importing, this is not much of an issue, as we can reassign keys from additional files by keeping track of the ###'s used so far.

Second, some files will contain sets of the same persons. One task when dealing with multiple Gedcom files might be to merge information taken from multiple Gedcom files. We can imagine things like just comparing Gedcom files (translation in the DeadEnds &mdash; comparing databasses.) to find likely matches, up to and including merging persons and families from different databases.