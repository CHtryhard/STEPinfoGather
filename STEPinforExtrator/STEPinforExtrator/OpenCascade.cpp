#include <windows.h>
#include <shobjidl.h>
#include <commdlg.h>
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
#include <filesystem>

namespace fs = std::filesystem;

void processStepFile(const std::string& filename) {
    std::cout << "Processing file: " << filename << std::endl;

    // Load STEP file
    STEPControl_Reader reader;
    if (reader.ReadFile(filename.c_str()) != IFSelect_RetDone) {
        std::cerr << "Error reading STEP file: " << filename << std::endl;
        return;
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
    std::cout << "Bounding Box Dimensions for " << filename << " (" << lengthUnit << "):" << std::endl;
    std::cout << "X: " << xMin << " to " << xMax << std::endl;
    std::cout << "Y: " << yMin << " to " << yMax << std::endl;
    std::cout << "Z: " << zMin << " to " << zMax << std::endl;

    // Calculate the volume
    GProp_GProps properties;
    BRepGProp::VolumeProperties(shape, properties);
    double volume = properties.Mass();
    std::cout << "Solid Volume for " << filename << " (" << volumeUnit << "): " << volume << std::endl;
}

std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

std::string selectFolder() {
    HRESULT hr;
    IFileDialog* pfd = nullptr;

    // CoInitialize and create the file dialog object.
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfd));
        if (SUCCEEDED(hr)) {
            // Set the options on the dialog.
            DWORD dwOptions;
            hr = pfd->GetOptions(&dwOptions);
            if (SUCCEEDED(hr)) {
                hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
                if (SUCCEEDED(hr)) {
                    // Show the dialog.
                    hr = pfd->Show(NULL);
                    if (SUCCEEDED(hr)) {
                        // Get the result.
                        IShellItem* psiResult = nullptr;
                        hr = pfd->GetResult(&psiResult);
                        if (SUCCEEDED(hr)) {
                            // Get the file path.
                            PWSTR pszFilePath = nullptr;
                            hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                            if (SUCCEEDED(hr)) {
                                std::wstring ws(pszFilePath);
                                std::string path(ws.begin(), ws.end());
                                CoTaskMemFree(pszFilePath);
                                psiResult->Release();
                                pfd->Release();
                                CoUninitialize();
                                return path;
                            }
                            psiResult->Release();
                        }
                    }
                }
            }
            pfd->Release();
        }
        CoUninitialize();
    }
    return "";
}

int main(int argc, char** argv) {
    std::string path;
    if (argc == 2) {
        path = argv[1];
    }
    else {
        path = selectFolder();
    }

    if (path.empty()) {
        std::cerr << "No folder selected or invalid path provided." << std::endl;
        return 1;
    }

    if (fs::is_directory(path)) {
        std::cout << "Processing directory: " << path << std::endl;
        bool foundFiles = false;
        for (const auto& entry : fs::directory_iterator(path)) {
            std::string extension = toLower(entry.path().extension().string());
            std::cout << "Found file: " << entry.path() << " with extension: " << extension << std::endl;
            if (extension == ".stp" || extension == ".step") {
                foundFiles = true;
                processStepFile(entry.path().string());
            }
        }
        if (!foundFiles) {
            std::cout << "No STEP files found in directory: " << path << std::endl;
        }
    }
    else if (fs::is_regular_file(path)) {
        std::cout << "Processing file: " << path << std::endl;
        processStepFile(path);
    }
    else {
        std::cerr << "Invalid path: " << path << std::endl;
        return 1;
    }

    return 0;
}