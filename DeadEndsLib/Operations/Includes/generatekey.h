//  DeadEnds Library
//
//  generatekey.h
//
//  Created by Thomas Wetmore on 20 July 2024.
//  Last changed on 4 June 2025.
//

#ifndef generatekey_h
#define generatekey_h

typedef enum RecordType RecordType;

// Public interface.
// generateRecordKey generates a new random key for a record type.
String generateRecordKey(RecordType);


#endif // generatekey.h
