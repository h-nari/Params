#include "Params.h"

Params::Params( ParamEntry *entries, unsigned int count)
{
  EEPROM.begin(PARAMS_EEPROM_SIZE);
  m_aEntries = entries;
  m_cEntries = count;
}

void Params::reset()
{
  for(uint16_t i=0; i<m_cEntries; i++)
    m_aEntries[i].value = m_aEntries[i].init_value;
}

bool Params::load()
{
  int addr;
  String name, value;
  char c;
  if(EEPROM.read(0) != PARAMS_SIGNATURE0 ||
     EEPROM.read(1) != PARAMS_SIGNATURE1 ||
     EEPROM.read(2) != PARAMS_SIGNATURE2 ||
     EEPROM.read(3) != PARAMS_SIGNATURE3) {
    Serial.printf("params signature not found in EEPROM.\n");
    return false;
  }
  addr = 4;
  
  while(1){
    c = EEPROM.read(addr++);
    if(!c) break;
    name = c;
    value = "";
    while(1){
      c = EEPROM.read(addr++);
      if(!c) break;
      name += c;
    }

    c = EEPROM.read(addr++);
    while(c){
      value += c;
      c = EEPROM.read(addr++);
    }

    ParamEntry *entry;
    if(findParam(name.c_str(), &entry))
      entry->value = value;
    else
      Serial.printf("param:%s not found.\n", name.c_str());
  }

  return true;
}

bool Params::changed()
{
  int addr;
  byte c;
  
  if(EEPROM.read(0) != PARAMS_SIGNATURE0 ||
     EEPROM.read(1) != PARAMS_SIGNATURE1 ||
     EEPROM.read(2) != PARAMS_SIGNATURE2 ||
     EEPROM.read(3) != PARAMS_SIGNATURE3) {
    return true;
  }
  addr = 4;
  for(int i=0; i < m_cEntries; i++){
    const char *name  = m_aEntries[i].name;
    const char *value = m_aEntries[i].value.c_str();
    const char *p;

    for(p=name; *p; p++)
      if(*p != EEPROM.read(addr++)) return true;
    if(EEPROM.read(addr++))  return true;
    for(p=value; *p; p++)
      if(*p != EEPROM.read(addr++)) return true;
    if(EEPROM.read(addr++))  return true;
  }
  return !EEPROM.read(addr);
}

bool Params::save()
{
  int addr = 0;

  EEPROM.write(addr++, PARAMS_SIGNATURE0);
  EEPROM.write(addr++, PARAMS_SIGNATURE1);
  EEPROM.write(addr++, PARAMS_SIGNATURE2);
  EEPROM.write(addr++, PARAMS_SIGNATURE3);
  
  for(int i=0; i < m_cEntries; i++){
    const char *name  = m_aEntries[i].name;
    const char *value = m_aEntries[i].value.c_str();
    const char *p;

    for(p=name; *p; p++)
      EEPROM.write(addr++, *p);
    EEPROM.write(addr++, 0);
    for(p=value; *p; p++)
      EEPROM.write(addr++, *p);
    EEPROM.write(addr++, 0);
  }
  EEPROM.write(addr++, 0);
  EEPROM.commit();
  return true;
}

bool Params::findParam(const char *name, ParamEntry **pEntry)
{
  for(int i=0; i < m_cEntries; i++){
    if(strcmp(m_aEntries[i].name,name)==0){
      if(pEntry) *pEntry = &m_aEntries[i];
      return true;
    }
  }
  return false;
}

bool Params::set(const char *name, const char *value)
{
  ParamEntry *e;
  if(findParam(name, &e)){
    e->value = value;
    return true;
  }
  return false;
}

bool Params::get(const char *name, const char **pValue)
{
  ParamEntry *e;

  if(findParam(name, &e)){
    if(pValue) *pValue = e->value.c_str();
    return true;
  }
  return false;
}

void Params::dump()
{
  Serial.printf("Params::dump cEntries:%d\n", m_cEntries);
  for(int i=0; i<m_cEntries; i++){
    Serial.printf(" %s : %s\n", m_aEntries[i].name, m_aEntries[i].value.c_str());
  }
  Serial.printf("Params::dump end.\n");
}

void Params::dump_eeprom(size_t len)
{
  int i,j;
  char c;

  Serial.printf("dump_eeprom len=%d\n",len);
  for(i=0; i < len; i+=16){
    Serial.printf(" %03x:", i); 
    for(j=0; j < 16; j++){
      if(i+j < len)
	Serial.printf(" %02x", EEPROM.read(i+j));
      else
	Serial.printf("   ");
    }
    Serial.print(" : ");
    for(j=0; j < 16; j++){
      if(i+j >= len) c = ' ';
      else {
	c = EEPROM.read(i+j);
	if(c < ' ' || c > 0x7f) c = '.';
      }
      Serial.printf("%c",c);
    }
    Serial.println();
  }
  Serial.printf("dump_eeprom end.\n");
}
