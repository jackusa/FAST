#ifndef VTK_LINE_SET_FILE_IMPORTER_HPP_
#define VTK_LINE_SET_FILE_IMPORTER_HPP_

#include "LineSet.hpp"
#include "Importer.hpp"

namespace fast {

class VTKLineSetFileImporter : public Importer {
    FAST_OBJECT(VTKLineSetFileImporter)
    public:
        void setFilename(std::string filename);
        LineSet::pointer getOutput();
    private:
        VTKLineSetFileImporter();
        void execute();

        std::string mFilename;
};

}

#endif