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
#include <QmitkServiceListWidget.h>

// STL HEaders
#include <list>

//QT headers
#include <QColor>

//microservices
#include <usGetModuleContext.h>
#include "mitkModuleContext.h"
#include <usServiceProperties.h>


const std::string QmitkServiceListWidget::VIEW_ID = "org.mitk.views.QmitkServiceListWidget";

QmitkServiceListWidget::QmitkServiceListWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
  m_Controls = NULL;
  CreateQtPartControl(this);
}

QmitkServiceListWidget::~QmitkServiceListWidget()
{

}


//////////////////// INITIALIZATION /////////////////////

void QmitkServiceListWidget::CreateQtPartControl(QWidget *parent)
{
  if (!m_Controls)
  {
    // create GUI widgets
    m_Controls = new Ui::QmitkServiceListWidgetControls;
    m_Controls->setupUi(parent);
    this->CreateConnections();
  }
}

void QmitkServiceListWidget::CreateConnections()
{
  if ( m_Controls )
  {
    connect( m_Controls->m_ServiceList, SIGNAL(currentItemChanged( QListWidgetItem *, QListWidgetItem *)), this, SLOT(OnDeviceSelectionChanged()) );
  }
}

void QmitkServiceListWidget::Initialize(std::string interfaceName, std::string namingProperty,  std::string filter)
{
  m_Context = mitk::GetModuleContext();
  if (filter.empty())
    m_Filter = "(" + mitk::ServiceConstants::OBJECTCLASS() + "=" + interfaceName + ")";
  else
    m_Filter = filter;
  m_Interface = interfaceName;
  m_NamingProperty = namingProperty;
  m_Context->RemoveServiceListener(this,  &QmitkServiceListWidget::OnServiceEvent);
  m_Context->AddServiceListener(this, &QmitkServiceListWidget::OnServiceEvent, m_Filter);
    // Empty ListWidget
  this->m_ListContent.clear();
  m_Controls->m_ServiceList->clear();

  // get Services
  std::list<mitk::ServiceReference> services = this->GetAllRegisteredServices();
  // Transfer them to the List
  for(std::list<mitk::ServiceReference>::iterator it = services.begin(); it != services.end(); ++it)
    AddServiceToList(*it);
}


///////////////////////// Getter & Setter /////////////////////////////////

template <class T>
T QmitkServiceListWidget::GetSelectedService()
{
  return this->GetDeviceForListItem(this->m_Controls->m_ServiceList->currentItem());
}

///////////// Methods & Slots Handling Direct Interaction /////////////////

void QmitkServiceListWidget::OnServiceSelectionChanged(){
  mitk::ServiceReference ref = this->GetServiceForListItem(this->m_Controls->m_ServiceList->currentItem());
  if (! ref) return;

  emit (ServiceSelected(ref));
}


///////////////// Methods & Slots Handling Logic //////////////////////////

void QmitkServiceListWidget::OnServiceEvent(const mitk::ServiceEvent event){

  switch (event.GetType())
  {
    case event.MODIFIED:
      emit(ServiceModified(event.GetServiceReference()));
      RemoveServiceFromList(event.GetServiceReference());
      AddServiceToList(event.GetServiceReference());
      break;
    case event.REGISTERED:
      emit(ServiceRegistered(event.GetServiceReference()));
      AddServiceToList(event.GetServiceReference());
      break;
    case event.UNREGISTERING:
      emit(ServiceUnregistering(event.GetServiceReference()));
      RemoveServiceFromList(event.GetServiceReference());
      break;
  //default:
    // mitkThrow() << "ServiceListenerWidget recieved an unrecognized event. Please Update Implementation of QmitkServiceListWidget::OnServiceEvent()";
  }
}


/////////////////////// HOUSEHOLDING CODE /////////////////////////////////

QListWidgetItem* QmitkServiceListWidget::AddServiceToList(mitk::ServiceReference serviceRef){
  QListWidgetItem *newItem = new QListWidgetItem;
  std::string caption;
  //TODO allow more complex formatting
  if (m_NamingProperty.empty())
    caption = m_Interface;
  else
    caption = serviceRef.GetProperty(m_NamingProperty).ToString();

  newItem->setText(caption.c_str());

  //Add new item to QListWidget
  m_Controls->m_ServiceList->addItem(newItem);
  // Construct link and add to internal List for reference
  QmitkServiceListWidget::ServiceListLink link;
  link.service = serviceRef;
  link.item = newItem;
  m_ListContent.push_back(link);
  return newItem;
}

bool QmitkServiceListWidget::RemoveServiceFromList(mitk::ServiceReference serviceRef){
  for(std::vector<QmitkServiceListWidget::ServiceListLink>::iterator it = m_ListContent.begin(); it != m_ListContent.end(); ++it){
    if ( serviceRef == it->service )
    {
      int row = m_Controls->m_ServiceList->row(it->item);
      QListWidgetItem* oldItem = m_Controls->m_ServiceList->takeItem(row);
      delete oldItem;
      this->m_ListContent.erase(it);
      return true;
    }
  }
  return false;
}


mitk::ServiceReference QmitkServiceListWidget::GetServiceForListItem(QListWidgetItem* item)
{
  for(std::vector<QmitkServiceListWidget::ServiceListLink>::iterator it = m_ListContent.begin(); it != m_ListContent.end(); ++it)
    if (item == it->item) return it->service;
}


std::list<mitk::ServiceReference> QmitkServiceListWidget::GetAllRegisteredServices(){
  //Get Service References
  return m_Context->GetServiceReferences(m_Interface, m_Filter);
}