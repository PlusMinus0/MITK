/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef mitkSurface_h
#define mitkSurface_h

#include "mitkBaseData.h"
#include "itkImageRegion.h"

class vtkPolyData;

namespace mitk
{
  /**
    * \brief Class for storing surfaces (vtkPolyData).
    * \ingroup Data
    */
  class MITK_CORE_EXPORT Surface : public BaseData
  {
  public:
    typedef itk::ImageRegion<5> RegionType;

    mitkClassMacro(Surface, BaseData);
    itkNewMacro(Self);
    mitkCloneMacro(Surface);

    void CalculateBoundingBox();
    virtual void CopyInformation(const itk::DataObject *data);
    virtual void ExecuteOperation(Operation *operation);
    virtual void Expand( unsigned int timeSteps = 1 );
    const RegionType& GetLargestPossibleRegion() const;
    virtual const RegionType& GetRequestedRegion() const;
    unsigned int GetSizeOfPolyDataSeries() const;
    virtual vtkPolyData* GetVtkPolyData(unsigned int t = 0);
    virtual void Graft( const DataObject* data );
    virtual bool IsEmptyTimeStep(unsigned int t) const;
    virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;
    virtual bool RequestedRegionIsOutsideOfTheBufferedRegion();
    virtual void SetRequestedRegion(const itk::DataObject *data);
    virtual void SetRequestedRegion(Surface::RegionType *region);
    virtual void SetRequestedRegionToLargestPossibleRegion();
    virtual void SetVtkPolyData(vtkPolyData* polydata, unsigned int t = 0);
    virtual void Swap(Surface& other);
    virtual void Update();
    virtual void UpdateOutputInformation();
    virtual bool VerifyRequestedRegion();


  protected:
    Surface();
    virtual ~Surface();

    Surface(const Surface& other);
    Surface& operator=(Surface other);

    virtual void ClearData();
    virtual void InitializeEmpty();

  private:
    std::vector<vtkPolyData*> m_PolyDatas;
    mutable RegionType m_LargestPossibleRegion;
    RegionType m_RequestedRegion;
    bool m_CalculateBoundingBox;
  };

  /**
  * \brief Compare two surfaces for equality, returns true if found equal
  * @param eps Epsilon to use for floating point comparison. Most of the time mitk::eps will be sufficient.
  * @param verbose Flag indicating if the method should give a detailed console output.
  *
  * This checks if any surface is NULL, compares the TimeSlicedGeometry, the vtkPolyData and the number of timesteps.
  */
MITK_CORE_EXPORT  bool Equal( mitk::Surface* rightHandSide,  mitk::Surface* leftHandSide, mitk::ScalarType eps, bool verbose);

  /**
  * \brief Compare two vtk PolyDatas for equality, returns true if found equal.
  * @param eps Epsilon to use for floating point comparison. Most of the time mitk::eps will be sufficient.
  * @param verbose Flag indicating if the method should give a detailed console output.
  *
  * This will only check if the number of cells, vertices, polygons, stripes and lines is the same and whether
  * all the two poly datas have the same number of points with the same coordinates. It is not checked whether
  * all points are correctly connected.
  */
MITK_CORE_EXPORT  bool Equal( vtkPolyData* rightHandSide,  vtkPolyData* leftHandSide, mitk::ScalarType eps, bool verbose);

}

#endif
