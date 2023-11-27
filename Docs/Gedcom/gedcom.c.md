# gedcom.c

|Component|Description|
|:---|:---|
|RecordType recordType(GNode *root)|Return the type of a Gedcom record (GNode tree).|
|int compareRecordKeys(String a, String b)|Compare record keys, based first on key length, then on lexicography.|