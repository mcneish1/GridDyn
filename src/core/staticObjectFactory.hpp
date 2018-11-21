#ifndef GD_STATIC_OBJECT_FACTORY_H_
#define GD_STATIC_OBJECT_FACTORY_H_

#include <string>
#include <memory>

namespace griddyn
{

class staticObjectFactory
{
  public:
    // forward to coreObjectFactory::instance()
    void registerFactory (const std::string &, std::shared_ptr<componentFactory>);
    void registerFactory (std::shared_ptr<componentFactory>);
    std::string<std::vector> getFactoryNames ();
    std::string<std::vector> getTypeNames (const std::string &);
    coreObject *createObject (const std::string &);
    coreObject *createObject (const std::string &, const std::string &);
    coreObject *createObject (const std::string &, const std::string &, const std::string &);
    std::shared_ptr<componentFactory> getFactory (const std::string &);
    bool isValidObject (const std::string &);
    bool isValidType (const std::string &, const std::string &);
    void prepObjects (const std::string &, const std::string &, count_t, coreObject *);
    void prepObjects (const std::string &, count_t, coreObject *);

    // we don't support createObject_3 here, since the only uses of that are for fncs and helics
    coreObject *createObject (griddyn_component component_type);
    coreObject *createObject (griddyn_component component_type, griddyn_component_subtype type);

  private:
    bool is_valid_subtype(griddyn_component component_type, griddyn_component_subtype type);
};

}  // namespace griddyn
#endif
