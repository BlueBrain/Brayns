#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include <brayns/api.h>
#include <brayns/common/types.h>

namespace brayns
{
class MaterialManager
{
public:
    MaterialManager();
    BRAYNS_API virtual ~MaterialManager();

    BRAYNS_API void clear();

    /**
      Adds a material to the scene
      @param material Material to add
      @return index of the new material
      */
    BRAYNS_API size_t add(const Material& material);

    /**
        Returns the material object for a given index
        @return Reference to material object
    */
    BRAYNS_API Material& get(size_t index);
    BRAYNS_API Materials& getMaterials() { return _materials; }
    /**
        Set the material object for a given index
        @param index Index of material in the scene
        @param material Material object
    */
    BRAYNS_API void set(const size_t index, const Material& material);

    /**
        Resets materials and builds the system ones (Bounding box, skymap, etc)
    */
    BRAYNS_API void reset();

    /**
        Builds missing material up to specified index
    */
    BRAYNS_API void buildMissingMaterials(const size_t materialId);

private:
    std::vector<Material> _materials;
};
}

#endif // MATERIALFACTORY_H
