# DeadEnds

DeadEnds is a C-based genealogical software project.

## Introduction

DeadEnds is a collection of libraries and command line programs that support  genealogical operations.

DeadEnds is based on the UNIX-based LifeLines program, which I wrote in the early 1990s, before there were  genealogical programs available for UNIX. Key features of LifeLines include:
- A B-Tree based database that stores genealogical records in Gedcom format.
- An *ncurses*-based user interface that provides a terminal-independent, simple windows-based interface.
- Capabilities to create, edit, merge and delete records of persons, families, sources and others. Linkages (to spouses, children, and sources) can be created, deleted and edited.
- LifeLines provides a programming subsystem that lets users write their own report generation programs.

As a followon system from LifeLines, DeadEnds has the following goals:
- Replace the LifeLines B-Tree database with an in-RAM system. When a DeadEnds based program starts up it reads a database from Gedcom files&mdash;these provide the *backing store* for DeadEnds. There are a number of Gedcom standards, one for syntax, and a variety for semantic variants. DeadEnds enforces only syntax standards.
- Embrace Unicode.
- Provide a clean API to genealogical operations to provide developers flexibility. I am imagining a SwiftUI interface.

## Repository Structure

The home directory of the DeadEnds repository is named, unsurprisingly, *DeadEnds*. It contains a number of files and sub-directories. The following sub-directories hold the C code libraries.

| Library | Description |
| :--- | :--- |
| Utils | Utility functions. Standard stuff. |
| DataTypes | Common data types, e.g. hash tables, lists, sets. Nothing specific to genealogy. Abstract data types useful in any context.  |
|Database|In-RAM genealogical database. A database consists of record indexes and a name index. A record index maps record keys to Gedcom record roots. A name index maps name keys to lists of record keys.|
|Parser | The lexer and parser for the report programming language. The lexer is hand-crafted; the parser is yacc generated. |
|Interp|The interpreter for the report programming language. The parser builds abstract syntax trees whose nodes are PNodes (program nodes). The interpreter runs  programs by interpreting these trees.|
|Gedcom|Gedcom library. Functions that deal with Gedcom and genealogical records constructed from trees of Gedcom nodes.|

Other subdirectories include:

| Directory | Description |
| :--- | :--- |
| Test | Test programs. Main programs that test the DeadEnds code base as it is developed.|
|Gedfiles|Gedcom files useful for testing.|
|Reports|Report programs useful for testing.
|Docs| Documentation.|
