Key Handling

Records have keys on their level 0 root nodes. Other nodes can have keys as values.

When reading Gedcom files one must make sure that ...



Data structure for key checking. A sorted List of elements where each element has the structure:

String key;

bool defined;

bool used;

When reading a 0 node. 