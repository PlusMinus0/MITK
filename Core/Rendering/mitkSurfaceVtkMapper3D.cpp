/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Module:    $RCSfile$
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "mitkSurfaceVtkMapper3D.h"
#include "mitkDataTreeNode.h"
#include "mitkProperties.h"
#include "mitkColorProperty.h"
#include "mitkLookupTableProperty.h"
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>


//##ModelId=3E70F60301D5
const mitk::Surface* mitk::SurfaceVtkMapper3D::GetInput()
{
  if (this->GetNumberOfInputs() < 1)
  {
    return 0;
  }

  return static_cast<const mitk::Surface * > ( GetData() );
}

//##ModelId=3E70F60301E9
void mitk::SurfaceVtkMapper3D::GenerateData()
{
}

//##ModelId=3E70F60301F2
void mitk::SurfaceVtkMapper3D::GenerateOutputInformation()
{
}

//##ModelId=3E70F60301F4
mitk::SurfaceVtkMapper3D::SurfaceVtkMapper3D()
{
  m_VtkPolyDataMapper = vtkPolyDataMapper::New();
  m_VtkPolyDataNormals = vtkPolyDataNormals::New();

  m_Actor = vtkActor::New();
  m_Actor->SetMapper(m_VtkPolyDataMapper);

  m_Prop3D = m_Actor;
  m_Prop3D->Register(NULL);

  m_GenerateNormals = false;
}

//##ModelId=3E70F60301F5
mitk::SurfaceVtkMapper3D::~SurfaceVtkMapper3D()
{
  m_VtkPolyDataMapper->Delete();
  m_VtkPolyDataNormals->Delete();
  m_Actor->Delete();
}

//##ModelId=3EF19FA803BF
void mitk::SurfaceVtkMapper3D::Update(mitk::BaseRenderer* renderer)
{
  if(IsVisible(renderer)==false)
  {
    m_Actor->VisibilityOff();
    return;
  }
  m_Actor->VisibilityOn();

  
  //
  // get the TimeSlicedGeometry of the input object
  //
  mitk::Surface::Pointer input  = const_cast< mitk::Surface* >( this->GetInput() );
  const TimeSlicedGeometry* inputTimeGeometry = dynamic_cast< const TimeSlicedGeometry* >( input->GetUpdatedGeometry() );
  assert( inputTimeGeometry != NULL );

  //
  // get the world time
  //
  const Geometry2D* worldGeometry = renderer->GetCurrentWorldGeometry2D();
  assert( worldGeometry != NULL );
  ScalarType time = worldGeometry->GetTimeBoundsInMS()[ 0 ];

  //
  // convert the world time in time steps of the input object
  //
  int timestep=0;
  if( time > -ScalarTypeNumericTraits::max() )
    timestep = inputTimeGeometry->MSToTimeStep( time );
  if( inputTimeGeometry->IsValidTime( timestep ) == false )
    return;
//  std::cout << "time: "<< time << std::endl;
//  std::cout << "timestep: "<<timestep << std::endl;
  
  //
  // set the input-object at time t for the mapper
  //

  if ( m_GenerateNormals )
  {
    m_VtkPolyDataNormals->SetInput( input->GetVtkPolyData( timestep ) );
    m_VtkPolyDataMapper->SetInput( m_VtkPolyDataNormals->GetOutput() );
  }
  else
  {
    m_VtkPolyDataMapper->SetInput( input->GetVtkPolyData( timestep ) );
  }

  //
  // apply properties read from the PropertyList
  //
  ApplyProperties(m_Actor, renderer);

  StandardUpdate();
}

//##ModelId=3EF1B44001D5
void mitk::SurfaceVtkMapper3D::Update()
{
}

void mitk::SurfaceVtkMapper3D::ApplyProperties(vtkActor* actor, mitk::BaseRenderer* renderer)
{
  bool useCellData;
  if (dynamic_cast<mitk::BoolProperty *>(this->GetDataTreeNode()->GetProperty("useCellDataForColouring").GetPointer()) == NULL)
    useCellData = false;
  else
    useCellData = dynamic_cast<mitk::BoolProperty *>(this->GetDataTreeNode()->GetProperty("useCellDataForColouring").GetPointer())->GetValue();

  if (useCellData)
  {
    m_VtkPolyDataMapper->SetColorModeToDefault();
    m_VtkPolyDataMapper->SetScalarRange(0,255);
    m_VtkPolyDataMapper->ScalarVisibilityOn();
    m_VtkPolyDataMapper->SetScalarModeToUseCellData();
    m_Actor->GetProperty()->SetSpecular (1);
    m_Actor->GetProperty()->SetSpecularPower (50);
    m_Actor->GetProperty()->SetInterpolationToPhong();
  }
  else
  {
    Superclass::ApplyProperties(m_Actor, renderer);
    m_VtkPolyDataMapper->ScalarVisibilityOff();
  }

  int scalarMode = VTK_COLOR_MODE_DEFAULT;
  if(this->GetDataTreeNode()->GetIntProperty("scalar mode", scalarMode, renderer))
  {
    m_VtkPolyDataMapper->SetScalarMode(scalarMode);
    m_VtkPolyDataMapper->ScalarVisibilityOn();
    m_Actor->GetProperty()->SetSpecular (1);
    m_Actor->GetProperty()->SetSpecularPower (50);
    //m_Actor->GetProperty()->SetInterpolationToPhong();
  }

  mitk::LookupTableProperty::Pointer lookupTableProp;
  lookupTableProp = dynamic_cast<mitk::LookupTableProperty*>(this->GetDataTreeNode()->GetProperty("LookupTable", renderer).GetPointer());
	if (lookupTableProp.IsNotNull() )
	{
    m_VtkPolyDataMapper->SetLookupTable(lookupTableProp->GetLookupTable().GetVtkLookupTable());
  }

  mitk::LevelWindow levelWindow;
  if(this->GetDataTreeNode()->GetLevelWindow(levelWindow, renderer, "levelWindow"))
  {
    m_VtkPolyDataMapper->SetScalarRange(levelWindow.GetMin(),levelWindow.GetMax());
  }
  else
  if(this->GetDataTreeNode()->GetLevelWindow(levelWindow, renderer))
  {
    m_VtkPolyDataMapper->SetScalarRange(levelWindow.GetMin(),levelWindow.GetMax());
  }

  bool wireframe=false;
  GetDataTreeNode()->GetVisibility(wireframe, renderer, "wireframe");
  if(wireframe)
    m_Actor->GetProperty()->SetRepresentationToWireframe();
  else
    m_Actor->GetProperty()->SetRepresentationToSurface();
}
