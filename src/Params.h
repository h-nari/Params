#ifndef _params_h_
#define _params_h_

#include "Arduino.h"
#include "EEPROM.h"

#define PARAMS_EEPROM_SIZE	4096

#define PARAMS_SIGNATURE0 'P'
#define PARAMS_SIGNATURE1 'R'
#define PARAMS_SIGNATURE2 'M'
#define PARAMS_SIGNATURE3 'S'


struct ParamEntry {
  const char *name;
  const char *init_value;
  String value;
};

class Params {
 protected:
  ParamEntry *m_aEntries;
  uint16_t    m_cEntries;

 public:
  Params(ParamEntry *, unsigned int);
  void reset();
  bool load();
  bool save();
  bool changed();
  bool set(const char *name, const char *value);
  bool get(const char *name, const char **pValue);
  void dump();
  void dump_eeprom(size_t len=64);
  
 protected:
  bool findParam(const char *name, ParamEntry **pEntry);
};


#endif /* _params_h_ */
