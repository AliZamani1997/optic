
with open("lib/Config/prameters.csv") as file :
    line = file.readline()
    line = line.strip()  # Remove any leading/trailing whitespace
    columns = line.split(",")
    table = {}
    line_n=0
    rfu_lines=[]
    while line :
        # Process the line
        line = file.readline()
        line_n+=1
        #filter out lines that are empty or contain "RFU"
        if line.find("RFU") >= 0:
            rfu_lines.append(line_n)
            continue
        line = line.strip()  # Remove any leading/trailing whitespace
        if line:
            # Split the line into columns and add to the table
            row = line.split(",")
            for i, value in enumerate(row):
                if table.get(columns[i] , None) is None:
                    table[columns[i]] = []
                table[columns[i]].append(value)

strip = lambda x : x.strip()  # Define a function to strip whitespace
table["Name"] = list(map(strip, table["Name"]))
table["Description"] = list(map(strip, table["Description"]))
table["Default"] = list(map(strip, table["Default"]))
table["Type"] = list(map(strip, table["Type"]))
table["ReadLevel"] = list(map(strip, table["ReadLevel"]))
table["WriteLevel"] = list(map(strip, table["WriteLevel"]))
table["Lib"] = list(map(strip, table["Lib"]))

table["ID"] = list(map(int, table["ID"]))
table["Size"] = list(map(int, table["Size"]))
# table["Address"] = list(map(int, table["Address"]))


#check if the ids are unique
if len(table["ID"]) != len(set(table["ID"])):
    raise ValueError("IDs are not unique in the parameters file.")

#address must be set
address=0
for i,size in enumerate(table["Size"]):
    if table["Address"][i]:
        if int(table["Address"][i]) < address:
            raise ValueError(f"Address {table['Address'][i]} at line {i+1} is less than the previous address {address}.")
        table["Address"][i] = int(table["Address"][i])
        address = table["Address"][i]
    
    else:
        table["Address"][i] = address

    address += size+2 #2 bytes for add crc

for l in set(table["WriteLevel"]+table["ReadLevel"]):
    if l not in ["","NONE", "USER", "ADMIN", "SYSTEM"]:
        raise ValueError(f"Invalid access level: {l}")
    
#convert back to csv
import csv
with open("lib/Config/prameters_standard.csv", "w", newline='') as file:
    writer = csv.writer(file)
    writer.writerow(columns)  # Write the header
    no_rfu_offset=0
    # add rfu lines
    for i in range(len(table["ID"]) + len(rfu_lines)):
        print(i, no_rfu_offset,rfu_lines)
        if (i+1) in rfu_lines:
            writer.writerow(["RFU"]+ [""] * (len(columns)-1))  # Write RFU line with empty values
            no_rfu_offset+=1
        else:
            row = [table[col][i-no_rfu_offset] for col in columns]
            writer.writerow(row)


codes="""#include "config_param.h"

config_t Parameters[CONFIGLIB_PARAMETERS_COUNT];

void initParamters(){
    uint16_t index=0;
##PARAMTERS##
}

config_t * getParameter(uint8_t key){
    for(uint8_t i=0; i<CONFIGLIB_PARAMETERS_COUNT; i++){
        if(Parameters[i].key == key){
            return &Parameters[i];
        }
    }
    return nullptr;
}

"""

header="""#ifndef config_param
#define config_param
#include <Arduino.h>

#define CONFIGLIB_PARAMETERS_DEFAULT_SIZE 16
#define CONFIGLIB_PARAMETERS_COUNT ##ParameterCount##
#define CONFIGLIB_TOTAL_PARAMETERS_SIZE ##TotalSize##

typedef enum {
##ParameterCK##
    MAX_CK
} config_key_t;

typedef enum {
    LOCK_LEVEL_NONE = 0, // No lock, key can be modified freely
    LOCK_LEVEL_USER, // Key can be modified by user-level access
    LOCK_LEVEL_ADMIN, // Key can only be modified by admin-level access
    LOCK_LEVEL_SYSTEM, // Key can only be modified by system-level access
    LOCK_LEVEL_MAX // Maximum lock level, no modifications allowed
} config_lock_level_t;

typedef struct {
    uint8_t key;
    uint8_t writeLockLevel = (uint8_t)LOCK_LEVEL_USER; // Lock level for the key, can be used to restrict access or modification
    uint8_t readLockLevel = (uint8_t)LOCK_LEVEL_NONE; // Lock level for reading the key
    uint8_t size = CONFIGLIB_PARAMETERS_DEFAULT_SIZE; // Lock level for the key, can be used to restrict access or modification
    uint16_t address; // Address in EEPROM or SPIFFS where the value is stored
    const char* Name;
    const char* Description = "";
    const char* Default = "";
    bool (*validation)(const char* value)= nullptr; // Pointer to a validation function, can be nullptr if no validation is needed
} config_t;

void initParamters();

config_t * getParameter(uint8_t key);

extern config_t Parameters[CONFIGLIB_PARAMETERS_COUNT];

#endif /* config_param */
"""
def ConvertedIDName(name:str):
    # Convert the name to a valid C identifier
    name = name.replace(" ", "_").replace("-", "_").replace("(", "").replace(")", "").replace("/", "_").replace(":", "_").replace(".", "_").replace("'", "").replace('"', '')
    
    return name.replace("_", " ").title().replace(" ", "")+"_CK"  # Capitalize the first letter to match the original code style

parameters = ""
parameters_ck = ""
for i in range(len(table["ID"])):

    p=f"\t/*{table['Name'][i]};*/\n"
    p+=f"\tParameters[index].Name = \"{table['Name'][i]}\" ;\n"
    if table['Description'][i]: 
        p+=f"\tParameters[index].Description = \"{table['Description'][i]}\" ;\n"
    if table['Default'][i]:
        p+=f"\tParameters[index].Default = \"{table['Default'][i]}\" ;\n"

    p+=f"\tParameters[index].key = (uint8_t){ConvertedIDName(table['Name'][i])} ;\n"

    if table['WriteLevel'][i]:
        p+=f"\tParameters[index].writeLockLevel = (uint8_t)LOCK_LEVEL_{table['WriteLevel'][i]} ;\n"
    if table['ReadLevel'][i]:
        p+=f"\tParameters[index].readLockLevel = (uint8_t)LOCK_LEVEL_{table['ReadLevel'][i]} ;\n"
    if table['Size'][i]!=16:
        p+=f"\tParameters[index].size = {table['Size'][i]} ;\n"
    p+=f"\tParameters[index].address = 0x{table['Address'][i]:04X} ;\n"
    p+=f"\tindex++ ;\n\n"

    parameters += p
    parameters_ck += f"\t{ConvertedIDName(table['Name'][i])}={table['ID'][i]}, //lib:{table['Lib'][i]} , type:{table['Type'][i]}\n"

codes = codes.replace("##PARAMTERS##", parameters)
with open("lib/Config/config_param.cpp", "w") as file:
    file.write(codes)

header = header.replace("##ParameterCount##", str(len(table["ID"])))
header = header.replace("##ParameterCK##", parameters_ck)
header = header.replace("##TotalSize##", f"0x{address:04X}")

with open("lib/Config/config_param.h", "w") as file:
    file.write(header)