# DeadEnds

DeadEnds is a C-based genealogical software project.

## Introduction

DeadEnds is a collection of static libraries and command line programs that support genealogical operations. It is an eventual goal to provide a genealogical program with a graphical user interface that uses the DeadEnds library.

DeadEnds is based on the UNIX-based LifeLines program, which I wrote in the early 1990s, before there were  genealogical programs available for UNIX. Key features of LifeLines include:
- A B-Tree based database that stores genealogical records in Gedcom format.
- An *ncurses*-based user interface that provides a terminal-independent, simple windows-based interface.
- Capabilities to create, edit, merge and delete records of persons, families, sources and others. Linkages (to spouses, children, and sources) can be created, deleted and edited.
- A programming subsystem that allows users to write programs that generate reports from the data in the database, and more general purpose programs for processing the data.

As a followon system from LifeLines, DeadEnds has the following goals:
- Replace the LifeLines B-Tree database with an in-RAM system. When a DeadEnds based program starts up it creates a Database "on the fly" by reading a Gedcom file. There is no other persistant form of the genealogical data. There are a number of Gedcom standards, one for syntax, and a variety for semantic variants. DeadEnds enforces only syntax standards.
- Embrace Unicode.To fully meet this goal I may eventually port DeadEnds to Go or Swift.
- Implement and possibly extend the LifeLines programming subsystem.
- Provide a clean API to genealogical operations to provide developers flexibility. I am imagining a SwiftUI interface.

## Repository Structure

The home directory of the DeadEnds repository is named, unsurprisingly, *DeadEnds*. It contains a number of files and sub-directories. The following are most of the important subdirectories.

| SubDirectory | Description                                                  |
| ------------ | ------------------------------------------------------------ |
| GedcomLib    | The DeadEnds Gedcom library. See the next table for its contents. |
| MenuLib      | MenuLib is a library that supports simple user interfaces that use stdin and stdout to ask questions and present menus. |
| RunScript    | Command line program that reads a Gedcom file into a Database, reads a script file (program file) and runs the program using the data in the Database. |
| TestProgram  | Command line program used during development as the primary test tool. |
| MenuTest     | Command line program used during development to test the menu library. |
| PatchSex     | One off command line program that reads a Gedcom file and rewrites it after adding or modifying 1 SEX lines in 0 INDI records that don't adhere to standards but can be easily put into standard format. |
| Gedfiles     | Collection of Gedcom files used to test features during development. |
| Scripts      | Collection of DeadEnds scripts (programs) used to test the script subsystem. |
| Docs         | Collection of documentation files that are woefully out of date. |

The Gedcom library is currently implemented as eight \*.a files, one built in each subdirectory. I will eventually build a single \*.a file. Here are the eight subdirectories.

| Subdirectory | Description |
| :--- | :--- |
| Utils | Utility functions. Standard stuff. |
| DataTypes | Common data types, e.g. hash tables, lists, sets. Nothing specific to genealogy. Abstract data types useful in any context.  |
|Database|In-RAM genealogical database. A database consists of record indexes and a name index. A record index maps record keys to Gedcom record roots. A name index maps name keys to lists of record keys.|
|Parser | The lexer and parser for the report programming language. The lexer is hand-crafted; the parser is yacc generated. |
|Interp|The interpreter for the report programming language. The parser builds abstract syntax trees whose nodes are PNodes (program nodes). The interpreter runs  programs by interpreting PNode trees.|
|Gedcom|Gedcom library. Functions that deal with Gedcom and genealogical records constructed from trees of Gedcom nodes.|
|Validate|Functions that validate records in a DeadEnds Database.|
|Operations|Functions that operate on records in a DeadEnds Database.|



