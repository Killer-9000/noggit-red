// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_NOISECYLINDERSNODE_HPP
#define NOGGIT_NOISECYLINDERSNODE_HPP

#include "NoiseGeneratorBase.hpp"
#include <external/libnoise/src/noise/noise.h>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;


namespace noggit
{
    namespace Red::NodeEditor::Nodes
    {
        class NoiseCylindersNode : public NoiseGeneratorBase
        {
        Q_OBJECT

        public:
            NoiseCylindersNode();
            void compute() override;

        private:
            noise::module::Cylinders _module;
        };

    }

}

#endif //NOGGIT_NOISECYLINDERSNODE_HPP
