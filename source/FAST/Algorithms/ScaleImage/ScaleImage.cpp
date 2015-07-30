#include "ScaleImage.hpp"
#include "FAST/Data/Image.hpp"

namespace fast {

ScaleImage::ScaleImage() {
    createInputPort<Image>(0);
    createOutputPort<Image>(0, OUTPUT_DEPENDS_ON_INPUT, 0);
    mLow = 0.0f;
    mHigh = 1.0f;
}

void ScaleImage::setLowestValue(float value) {
    mLow = value;
}

void ScaleImage::setHighestValue(float value) {
    mHigh = value;
}

void ScaleImage::execute() {
    if(mHigh <= mLow)
        throw Exception("The high value must be higher than the low value in ScaleImage.");

    Image::pointer input = getStaticInputData<Image>();
    Image::pointer output = getStaticOutputData<Image>();

    const uint width = input->getWidth();
    const uint height = input->getHeight();
    const uint depth = input->getDepth();
    cl::NDRange globalSize;

    float minimum = input->calculateMinimumIntensity();
    float maximum = input->calculateMaximumIntensity();

    OpenCLDevice::pointer device = getMainDevice();
    device->createProgramFromSourceWithName("ScaleImage", std::string(FAST_SOURCE_DIR) + "Algorithms/ScaleImage/ScaleImage.cl");
    cl::Program program = device->getProgram("ScaleImage");
    cl::Kernel kernel;

    if(input->getDimensions() == 2) {
        output->create2DImage(width, height, TYPE_FLOAT, input->getNrOfComponents(), device);
        globalSize = cl::NDRange(width, height);
        OpenCLImageAccess2D::pointer inputAccess = input->getOpenCLImageAccess2D(ACCESS_READ, device);
        OpenCLImageAccess2D::pointer outputAccess = output->getOpenCLImageAccess2D(ACCESS_READ_WRITE, device);
        kernel = cl::Kernel(program, "scaleImage2D");
        kernel.setArg(0, *(inputAccess->get()));
        kernel.setArg(1, *(outputAccess->get()));
    } else {
        output->create3DImage(width, height, depth, TYPE_FLOAT, input->getNrOfComponents(), device);
        globalSize = cl::NDRange(width, height, depth);
        OpenCLImageAccess3D::pointer inputAccess = input->getOpenCLImageAccess3D(ACCESS_READ, device);
        kernel = cl::Kernel(program, "scaleImage3D");
        kernel.setArg(0, *(inputAccess->get()));
        if(device->isWritingTo3DTexturesSupported()) {
            OpenCLImageAccess3D::pointer outputAccess = output->getOpenCLImageAccess3D(ACCESS_READ_WRITE, device);
            kernel.setArg(1, *(outputAccess->get()));
        } else {
            OpenCLBufferAccess::pointer outputAccess = output->getOpenCLBufferAccess(ACCESS_READ_WRITE, device);
            kernel.setArg(1, *(outputAccess->get()));
            kernel.setArg(6, output->getNrOfComponents());
        }
    }

    kernel.setArg(2, minimum);
    kernel.setArg(3, maximum);
    kernel.setArg(4, mLow);
    kernel.setArg(5, mHigh);

    cl::CommandQueue queue = device->getCommandQueue();

    queue.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            globalSize,
            cl::NullRange
    );
}

} // end namespace fast

