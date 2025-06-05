#ifndef DFF31344_A18F_4515_8938_F0033363FED5
#define DFF31344_A18F_4515_8938_F0033363FED5

#include "FS.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

void createDir(fs::FS &fs, const char * path);

void removeDir(fs::FS &fs, const char * path);

void readFile(fs::FS &fs, const char * path, bool create = false);

void writeFile(fs::FS &fs, const char * path, const char * message, bool create = false);

void appendFile(fs::FS &fs, const char * path, const char * message, bool create = false);

void renameFile(fs::FS &fs, const char * path1, const char * path2);

void deleteFile(fs::FS &fs, const char * path);

void exportSPIFFS() ;

#endif /* DFF31344_A18F_4515_8938_F0033363FED5 */
