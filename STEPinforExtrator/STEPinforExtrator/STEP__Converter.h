#include <STEPControl_Reader.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <Interface_Static.hxx>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>


int processStepFile(const std::string& filename) {
    //initialize file to write
    std::ofstream myfile;
    std::time_t currentTime = std::time(nullptr);
    // Initialize a tm structure
    std::tm localTime;
    // Convert to local time using localtime_s
    localtime_s(&localTime, &currentTime);
    // Extract the year, month, day, hour, minute
    int year = localTime.tm_year + 1900; // tm_year is years since 1900
    int month = localTime.tm_mon + 1;    // tm_mon is 0-based, so add 1
    int day = localTime.tm_mday;
    int hour = localTime.tm_hour;
    int minute = localTime.tm_min;
    std::string outputname = std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day) + "__" + std::to_string(hour) + std::to_string(minute) + ".csv";
    myfile.open(outputname);

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
    Standard_Real BVolume = (xMax - xMin) * (yMax - yMin) * (zMax - zMin);

    // Calculate the volume
    GProp_GProps properties;
    BRepGProp::VolumeProperties(shape, properties);
    double volume = properties.Mass();
    //write file
    myfile << "X,Y,Z,BV/5000,SV,Density,Weight,QTY\n";
    myfile << xMax - xMin << ",";
    myfile << yMax - yMin << ",";
    myfile << zMax - zMin << ",";
    myfile << BVolume/5000 << ",";
    myfile << volume << ","<<"," << "," << std::endl;
    myfile.close();
    return 0;
}


