#include <STEPControl_Reader.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <Interface_Static.hxx>


int processStepFile(const std::string& filename, std::ofstream& myfile) {

    // Load STEP file
    STEPControl_Reader reader;
    if (reader.ReadFile(filename.c_str()) != IFSelect_RetDone) {
        return -1;
    }

    // Transfer file contents to the OpenCASCADE data structures
    reader.TransferRoots();
    TopoDS_Shape shape = reader.OneShape();

    // Retrieve the units
    Handle(XSControl_WorkSession) session = reader.WS();
    Interface_Static::SetCVal("xstep.cascade.unit", "");
    std::string lengthUnit = Interface_Static::CVal("xstep.cascade.unit");
    std::string volumeUnit = lengthUnit + "^3"; // Assuming cubic units for volume

    // Calculate the bounding box
    Bnd_Box boundingBox;
    BRepBndLib::Add(shape, boundingBox);

    // Get the bounding box dimensions
    Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
    boundingBox.Get(xMin, yMin, zMin, xMax, yMax, zMax);
    Standard_Real BVolume = (xMax - xMin) * (yMax - yMin) * (zMax - zMin) / 1000;

    // Calculate the volume
    GProp_GProps properties;
    BRepGProp::VolumeProperties(shape, properties);
    double volume = properties.Mass() / 1000;
    //write file
    myfile << filename << ",";
    myfile << (xMax - xMin) / 10 << ",";
    myfile << (yMax - yMin) / 10 << ",";
    myfile << (zMax - zMin) / 10 << ",";
    myfile << BVolume / 5000 << ",";
    myfile << volume << "," << "," << "," << std::endl;
    return 0;
}
