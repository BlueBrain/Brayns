#include "OutlineShell.h"

#include <brayns/engine/components/IsosurfaceRendererComponent.h>
#include <brayns/engine/components/VolumeRendererComponent.h>
#include <brayns/engine/volumes/RegularVolume.h>

void OutlineShell::generate(brayns::Model &model, OutlineShellData data)
{
    brayns::RegularVolume volume;
    volume.dataType = brayns::VolumeDataType::UNSIGNED_CHAR;
    volume.size = data.gridSize;
    volume.spacing = data.gridSpacing;
    volume.data = std::move(data.data);

    model.addComponent<brayns::VolumeRendererComponent<brayns::RegularVolume>>(std::move(volume));
    // model.addComponent<brayns::IsosurfaceRendererComponent<brayns::RegularVolume>>(std::move(volume), 1.f);
}
