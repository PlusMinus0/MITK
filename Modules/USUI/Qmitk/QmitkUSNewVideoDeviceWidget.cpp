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

//#define _USE_MATH_DEFINES
#include <QmitkUSNewVideoDeviceWidget.h>

//QT headers


//mitk headers


//itk headers


const std::string QmitkUSNewVideoDeviceWidget::VIEW_ID = "org.mitk.views.QmitkUSNewVideoDeviceWidget";

QmitkUSNewVideoDeviceWidget::QmitkUSNewVideoDeviceWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
  m_Controls = NULL;
  CreateQtPartControl(this);
}

QmitkUSNewVideoDeviceWidget::~QmitkUSNewVideoDeviceWidget()
{
}

//////////////////// INITIALIZATION /////////////////////


void QmitkUSNewVideoDeviceWidget::CreateQtPartControl(QWidget *parent)
{
  if (!m_Controls)
  {
    // create GUI widgets
    m_Controls = new Ui::QmitkUSNewVideoDeviceWidgetControls;
    m_Controls->setupUi(parent);
    this->CreateConnections();
  }
}

void QmitkUSNewVideoDeviceWidget::CreateConnections()
{
  if ( m_Controls )
  {
    connect( m_Controls->m_BtnDone,   SIGNAL(clicked()), this, SLOT(OnClickedDone()) );
    connect( m_Controls->m_BtnCancel, SIGNAL(clicked()), this, SLOT(OnClickedCancel()) );
  }
}


///////////// Methods & Slots Handling Direct Interaction /////////////////

void QmitkUSNewVideoDeviceWidget::OnClickedDone(){
   m_Active = false;
  MITK_INFO << "NewDeviceWidget: ClickedDone()";
  
  // Assemble Metadata
  mitk::USImageMetadata::Pointer metadata = mitk::USImageMetadata::New();
  metadata->SetDeviceComment(m_Controls->m_Comment->text().toStdString());
  metadata->SetDeviceModel(m_Controls->m_Model->text().toStdString());
  metadata->SetDeviceManufacturer(m_Controls->m_Manufacturer->text().toStdString());
  metadata->SetProbeName(m_Controls->m_Probe->text().toStdString());
  metadata->SetZoom(m_Controls->m_Zoom->text().toStdString());


  // Create Device
  mitk::USDevice::Pointer newDevice = mitk::USDevice::New(metadata, true);

  newDevice->Connect();

  emit Finished();
}

void QmitkUSNewVideoDeviceWidget::OnClickedCancel(){
  m_TargetDevice = 0;
  m_Active = false;
   MITK_INFO << "NewDeviceWidget: OnClickedCancel()";
  emit Finished();

}



///////////////// Methods & Slots Handling Logic //////////////////////////


void QmitkUSNewVideoDeviceWidget::EditDevice(mitk::USDevice::Pointer device)
{
  MITK_INFO << "NewDeviceWidget: EditDevice()()";
  m_TargetDevice = device;
  m_Active = true;
}


void QmitkUSNewVideoDeviceWidget::CreateNewDevice()
{
  MITK_INFO << "NewDeviceWidget: CreateNewDevice()";
  m_TargetDevice = 0;
  InitFields(mitk::USImageMetadata::New());
  m_Active = true;
}


/////////////////////// HOUSEHOLDING CODE /////////////////////////////////

QListWidgetItem* QmitkUSNewVideoDeviceWidget::ConstructItemFromDevice(mitk::USDevice::Pointer device){
  QListWidgetItem *result = new QListWidgetItem;
  std::string text = device->GetDeviceManufacturer() + "|" + device->GetDeviceModel();
  result->setText(text.c_str());
  return result;
}

void QmitkUSNewVideoDeviceWidget::InitFields(mitk::USImageMetadata::Pointer metadata){
  this->m_Controls->m_Manufacturer->setText (metadata->GetDeviceManufacturer().c_str());
  this->m_Controls->m_Model->setText (metadata->GetDeviceModel().c_str());
  this->m_Controls->m_Comment->setText (metadata->GetDeviceComment().c_str());
  this->m_Controls->m_Probe->setText (metadata->GetProbeName().c_str());
  this->m_Controls->m_Zoom->setText (metadata->GetZoom().c_str());
}
