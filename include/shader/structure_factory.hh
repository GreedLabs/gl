#ifndef STRUCTURE_FACTORY_HH
# define STRUCTURE_FACTORY_HH

# include <vector>

struct StructureFactory {
  StructureFactory(const char *name);
  ~StructureFactory();

  void add_member(const char *type, const char *name);

  /* Will need to be freed */
  char *generate();

  size_t len;
  char *name;
  std::vector<char *> members;
};

#endif
