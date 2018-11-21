#include "objectFactory.hpp"

#include "staticObjectFactory.hpp"

namespace griddyn
{

// non-static forwarding to coreObjectFactory
void staticObjectFactory::registerFactory (std::string const& name, std::shared_ptr<componentFactory> factory)
{
    return coreObjectFactory::instance()-> registerFactory(name, factory);
}

void staticObjectFactory::registerFactory (std::shared_ptr<componentFactory> factory)
{
    return coreObjectFactory::instance()-> registerFactory(factory);
}

std::string<std::vector> staticObjectFactory::getFactoryNames ()
{
    return coreObjectFactory::instance()-> getFactoryNames();
}

std::string<std::vector> staticObjectFactory::getTypeNames (std::string const& component)
{
    return coreObjectFactory::instance()-> getTypeNames(component);
}

coreObject* staticObjectFactory::createObject (std::string const& component)
{
    return coreObjectFactory::instance()-> createObject(component);
}

coreObject* staticObjectFactory::createObject (std::string const& component, std::string const& typeName)
{
    return coreObjectFactory::instance()-> createObject(component, typeName);
}

coreObject* staticObjectFactory::createObject (std::string const& component, std::string const& typeName, std::string const& objName)
{
    return coreObjectFactory::instance()-> createObject(component, typeName, objName);
}

std::shared_ptr<componentFactory> staticObjectFactory::getFactory (std::string const& component)
{
    return coreObjectFactory::instance()-> getFactory(component);
}

bool staticObjectFactory::isValidObject (std::string const& component)
{
    return coreObjectFactory::instance()-> isValidObject(component);
}

bool staticObjectFactory::isValidType (std::string const& component, std::string const& typeName)
{
    return coreObjectFactory::instance()-> isValidType(component, typeName);
}

void staticObjectFactory::prepObjects (std::string const& component, std::string const& typeName, count_t numObjects, coreObject* obj)
{
    return coreObjectFactory::instance()-> prepObjects(component, typeName, numObjects, obj);
}

void staticObjectFactory::prepObjects (std::string const& component, count_t numObjects, coreObject* obj) {}
{
    return coreObjectFactory::instance()-> prepObjects(component, numObjects, obj);
}

// statically create object from type list
coreObject* staticObjectFactory::createObject (griddyn_component component_type)
{

}

coreObject* staticObjectFactory::createObject (griddyn_component component_type, griddyn_component_subtype type)
{

}

bool staticObjectFactory::is_valid_subtype(griddyn_component component_type, griddyn_component_subtype type)
{
    const std::unordered_map<griddyn_component, std::unordered_set<griddyn_component_subtype>> valid; // TODO

    auto f = valid.find(component_type);
    if (f == valid.end()) return false;

    return f->find(type) != f->end();
}

} // namespace griddyn
