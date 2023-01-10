#include "SimGeometry.h"

#include "Rtypes.h"
#include "RtypesCore.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGeoNavigator.h"
#include "TGeoVolume.h"
#include "TCanvas.h"
#include "TString.h"
#include "TView3D.h"
#include "TAxis3D.h"
#include "TFile.h"
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

SimGeometry::SimGeometry()
{
    //init structures and materials
    //set units
    TGeoManager::LockDefaultUnits(false);
    TGeoManager::SetDefaultUnits(TGeoManager::kRootUnits);
    TGeoManager::LockDefaultUnits(true);
    manager = new TGeoManager("manager", "A simple ACTAR geometry");
    gGeoManager->SetVerboseLevel(1);

    //naive material since we dont compute physics here
    noneMaterial = new TGeoMaterial("none", 0.0, 0.0, 0.0);
    noneMedium = new TGeoMedium("none", 1, noneMaterial);

    //top volume
    //world shape as a box
    topVol = manager->MakeBox("Top",
                              noneMedium,
                              100.,
                              100.,
                              100.); // 2x2x2 m3
    manager->SetTopVolume(topVol);
}

SimGeometry::~SimGeometry()
{
    //we dont have to delete anything in principle
    //delete manager;
}

void SimGeometry::SetDriftSizes(double sizeX, double sizeY, double sizeZ)
{
    actar.X = sizeX;
    actar.Y = sizeY;
    actar.Z = sizeZ;
}

void SimGeometry::SetSiliconUnit(int type, double x, double y, double z)
{
    silicons.unitX[type] = x;
    silicons.unitY[type] = y;
    silicons.unitZ[type] = z;
}

void SimGeometry::SetSiliconAssemblyOffset(double offsetX, double offsetY)
{
    if(offsetX != 0.0)
    {
        silicons.XOffset = offsetX;
        silicons.IsXOffset = true;
        std::cout<<"Using an offset in X!"<<'\n';
    }
    if(offsetY != 0.0)
    {
        silicons.YOffset = offsetY;
        silicons.IsYOffset = true;
        std::cout<<"Using an offset in Y!"<<'\n';
    }
    
    if(silicons.IsXOffset && silicons.IsYOffset)
        throw std::runtime_error("Both axis offsets not implemented yet: take a look at Assembly placements if you want to simultaneously add X and Y offsets!");
}

void SimGeometry::Construct()
{
    //drift chamber
    driftVol = manager->MakeBox("Drift",
                                noneMedium,
                                actar.X, actar.Y, actar.Z);//at center of world
    driftVol->SetLineColor(kBlue);
    topVol->AddNode(driftVol, 1);

    //silicon unit (we have two types)
    unitSilVolMap[1] = manager->MakeBox("UnitSiliconType1",
                                        noneMedium,
                                        silicons.unitX[1],
                                        silicons.unitY[1],
                                        silicons.unitZ[1]);
    unitSilVolMap[1]->SetLineColor(kOrange);
    unitSilVolMap[2] = manager->MakeBox("UnitSiliconType2",
                                        noneMedium,
                                        silicons.unitX[2],
                                        silicons.unitY[2],
                                        silicons.unitZ[2]);
    unitSilVolMap[2]->SetLineColor(kMagenta);

    //and now assembly
    silAssembly = new TGeoVolumeAssembly("SiliconAssembly");
    //for rotations of silicons!!!
    //only allowed by theta = 90 degrees by now
    //a rotation of phi = 90 is also included for lateral assemblies
    TGeoRotation nullRotation {"nullRotation", 0.0, 0.0, 0.0};
    TGeoRotation siliconRot {"siliconRot", 0.0, 90.0, 0.0};
    TGeoRotation lateralRot {"lateralRot", 90.0, 0.0, 0.0};
    TGeoRotation sideRot    {"sideRot", 180.0, 0.0, 0.0};
    for(const auto& silType : silicons.AssemblyMap)
    {
        for(const auto& sil : silType.second)
        {
            std::cout<<"Si type: "<<silType.first<<" with index: "<<sil.first<<" first: "<<sil.second.first<<" second: "<<sil.second.second<<'\n';
            TGeoTranslation trans {};
            if(silicons.IsXOffset)
                trans = {0.0, sil.second.first, sil.second.second};
            if(silicons.IsYOffset)//if we work on lateral mode (Y), given values are (X,Z)
                trans = {sil.second.first, 0.0, sil.second.second};
            //place according to silicon rotation and lateral rotation (bc unitSilicon is defined along X)
            silAssembly->AddNode(unitSilVolMap[silType.first],
                                 sil.first,
                                 new TGeoCombiTrans(trans, ((silicons.IsYOffset) ? lateralRot : nullRotation) *
                                                    ((silicons.IsRotatedMap[silType.first][sil.first]) ? siliconRot : nullRotation)));
        }
    }
    //placement of assembly
    TGeoTranslation assemblyTrans {};
    if(silicons.IsXOffset)
        assemblyTrans = {actar.X + silicons.XOffset, 0.0, 0.0};
    if(silicons.IsYOffset)
        assemblyTrans = {0.0, actar.Y + silicons.YOffset, 0.0};//left silicons
    topVol->AddNode(silAssembly,
                    1,
                    new TGeoCombiTrans(assemblyTrans, nullRotation));
    //and second copy for YOffset (bc we have two panels at each side)
    if(silicons.IsYOffset)
    {//right silicons
        TGeoTranslation assemblyTrans2 {0.0, -1.0 * (actar.Y + silicons.YOffset), 0.0};
        topVol->AddNode(silAssembly,
                        2,
                        new TGeoCombiTrans(assemblyTrans2, sideRot));//so we have an especular image in left side
    }
    
    //and close geometry
    manager->CloseGeometry();

    //initialize navigator
    navigator = new TGeoNavigator(manager);
    manager->SetCurrentNavigator(0);

    //and print
    PrintGeometry();
}

void SimGeometry::PrintGeometry()
{
    std::cout<<std::fixed<<std::setprecision(3);
    std::cout<<"Drift sizes X: "<<actar.X<<" Y: "<<actar.Y<<" Z: "<<actar.Z<<'\n';
    std::cout<<"Unit silicon type 1 width: "<<silicons.unitX[1]<<" and plane "<<silicons.unitY[1]<<" x "<<silicons.unitZ[1]<<'\n';
    std::cout<<"Unit silicon type 2 width: "<<silicons.unitX[2]<<" and plane "<<silicons.unitY[2]<<" x "<<silicons.unitZ[2]<<'\n';
    std::cout<<"Silicons placed at driftX + "<<silicons.XOffset<<'\n';
}

void SimGeometry::Draw()
{
    canvas = new TCanvas("c1", "Drawing geometry", 1);
    canvas->cd();
    manager->GetMasterVolume()->Draw("ogle");
    canvas->Update();
    TAxis3D::ToggleRulers();
    canvas->cd();
    //canvas->WaitPrimitive();
    //canvas->Close();
}

std::tuple<int, int, int> SimGeometry::GetSilTypeAndIndexFromTString(TString& path)
{
    std::string pathString {path.Data()};
    auto assemblyString { pathString.substr(pathString.find("SiliconAssembly"), 17)};
    int assemblyIndex { std::stoi(std::string(1, assemblyString.back()))};
    auto silString { pathString.substr(pathString.find("UnitSilicon"))};
    auto underscore { silString.find("_")};
    int silType  { std::stoi(silString.substr(underscore - 1, 1))};
    int silIndex { std::stoi(silString.substr(underscore + 1))};

    return std::make_tuple(assemblyIndex, silType, silIndex);
}

void SimGeometry::PropagateTrackToSilicons(const XYZPoint& point,
                                           const XYZVector& direction,
                                           double& distance,
                                           int& assemblyIndex,
                                           int& silType,
                                           int& silIndex,
                                           bool debug)
{
    //initializing state
    manager->InitTrack(point.X(), point.Y(), point.Z(),
                       direction.X(), direction.Y(), direction.Z());
    TString path { manager->GetPath()};
    if(debug)std::cout<<"Path at 0: "<<path<<'\n';
    for(int i = 1; i < 4; i++)
    {
        manager->FindNextBoundaryAndStep();
        path = manager->GetPath();
        if(debug)std::cout<<"Path at "<<i<<" : "<<path<<'\n';
        if(debug)std::cout<<"Path size: "<<path.Length()<<'\n';
        distance += manager->GetStep();
        if(path.Contains("SiliconAssembly"))
        {
            auto values { GetSilTypeAndIndexFromTString(path)};
            assemblyIndex = std::get<0>(values);
            silType       = std::get<1>(values);
            silIndex      = std::get<2>(values);
            break;
        }
        else if(path.IsWhitespace())
        {
            assemblyIndex = -1;
            silIndex      = -1;
            silType       = -1;
            break;
        }
    }
    
}

void SimGeometry::CheckIfStepIsInsideDriftChamber(const XYZPoint &point,
                                                  const XYZVector &direction,
                                                  double step,
                                                  bool &isInside,
                                                  bool debug)
{
    manager->InitTrack(point.X(), point.Y(), point.Z(),
                       direction.X(), direction.Y(), direction.Z());
    manager->SetStep(step);
    manager->Step(false);//false bc is given by hand
    TString path { manager->GetPath()};
    if(debug)std::cout<<"Step in drift debug: "<<path<<'\n';
    if(path.Contains("Drift"))
        isInside = true;
    else
        isInside = false;
}

void SimGeometry::ReadGeometry(std::string path, std::string fileName)
{
    manager = TGeoManager::Import((path + fileName + ".root").c_str());
    if(!manager)
        throw std::runtime_error("Error reading TGeoManager from " + fileName);
    //also, read parameters
    auto* infile = new TFile((path + "parameters_" + fileName + ".root").c_str(), "read");
    infile->cd();
    silicons = *(infile->Get<SilInfo>("silicons"));
    actar    = *(infile->Get<DriftInfo>("drift"));
    infile->Close();
    delete infile;
}

void SimGeometry::WriteGeometry(std::string path, std::string fileName)
{
    manager->Export((path + fileName + ".root").c_str());

    //write parameters to file
    auto* outFile = new TFile((path + "parameters_" + fileName + ".root").c_str(), "recreate");
    outFile->cd();
    outFile->WriteObject(&silicons, "silicons");
    outFile->WriteObject(&actar, "drift");
    outFile->Close();
    delete outFile;
    
}
