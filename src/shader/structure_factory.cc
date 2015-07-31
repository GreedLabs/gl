#include <string.h>
#include <stdlib.h>
#include "structure_factory.hh"

StructureFactory::StructureFactory(const char *name): len(0) {
  int len1 = sizeof ("struct ") - 1;
  int len2 = strlen(name);
  int len3 = sizeof (" {") - 1;

  this->name = (char *) malloc(len1 + len2 + len3 + 1);
  this->name[len1 + len2 + len3] = '\0';

  strcpy(this->name, "struct ");
  strcpy(this->name + len1, name);
  strcpy(this->name + len1 + len2, " {");

  len += len1 + len2 + len3;
}

void StructureFactory::add_member(const char *type, const char *name) {

  int len1 = strlen(type);
  int len2 = strlen(name);

  /*
   * We need to allocate the space for two more
   * chars:
   *   * the space between the type and the name
   *   * the semicolon at the end of the instruction
   * member = "type name;\0"
   */
  char *member = (char *) malloc(len1 + len2 + 3);

  strcpy(member, type);
  strcpy(member + len1 + 1, name);

  member[len1] = ' ';
  member[len1 + len2 + 1] = ';';
  member[len1 + len2 + 2] = '\0';

  len += len1 + len2 + 2;
  members.push_back(member);
}

char *StructureFactory::generate() {
  size_t s = strlen(name);
  size_t len1 = sizeof ("};") - 1;
  char *code = (char *) malloc(len + len1 + 1);

  strcpy(code, name);
  for (size_t i = 0; i < members.size(); ++i) {
    strcpy(code + s, members[i]);
    s += strlen (members[i]);
  }

  strcpy(code + s, "};");
  code[s + len1] = '\0';

  return code;
}
