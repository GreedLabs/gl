#ifndef STRUCTURE_FACTORY_HH
# define STRUCTURE_FACTORY_HH

# include <vector>

struct StructureFactory {
  StructureFactory(const char *name);

  void add_member(const char *type, const char *name);
  char *generate();

  size_t len;
  char *name;
  std::vector<char *> members;
};

#endif
