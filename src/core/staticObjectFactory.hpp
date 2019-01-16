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

    coreObject *createObject (griddyn_component component_type);
};

}  // namespace griddyn
#endif