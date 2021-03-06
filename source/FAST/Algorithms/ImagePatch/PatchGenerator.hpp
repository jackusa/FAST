#pragma once

#include <FAST/ProcessObject.hpp>
#include <FAST/Streamers/Streamer.hpp>
#include <thread>

namespace fast {

class ImagePyramid;
class Image;

class FAST_EXPORT PatchGenerator : public Streamer {
    FAST_OBJECT(PatchGenerator)
    public:
        void setPatchSize(int width, int height, int depth = 1);
        void setPatchLevel(int level);
        ~PatchGenerator();
        void loadAttributes() override;
    protected:
        int m_width, m_height, m_depth;

        std::shared_ptr<ImagePyramid> m_inputImagePyramid;
        std::shared_ptr<Image> m_inputVolume;
        std::shared_ptr<Image> m_inputMask;
        int m_level;

        void execute() override;
        void generateStream() override;
    private:
        PatchGenerator();
};
}