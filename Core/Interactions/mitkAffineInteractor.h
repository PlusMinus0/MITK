#ifndef MITKAFFINEINTERACTOR_H_HEADER_INCLUDED_C188C29F
#define MITKAFFINEINTERACTOR_H_HEADER_INCLUDED_C188C29F

#include "mitkCommon.h"
#include "mitkInteractor.h"
#include "mitkVector.h"

namespace mitk {

//##Documentation
//## @brief Interactor for Affine transformations translate, rotate
//## @ingroup Interaction
//##
//## An object of this class can translate, rotate and scale the data objects
//## by modifying its geometry.
class AffineInteractor : public Interactor
{
public:
  mitkClassMacro(AffineInteractor,Interactor);

	//##Documentation
	//## @brief Constructor
	//##
	//## @params dataTreeNode is the node, this Interactor is connected to
	//## type is the type of StateMachine like declared in the XML-Configure-File
	//##
 	AffineInteractor(const char * type, DataTreeNode* dataTreeNode);

  //##Documentation
	//## @brief Destructor
  ~AffineInteractor(){};

protected:

	virtual bool ExecuteAction(int actionId, mitk::StateEvent const* stateEvent, int objectEventId, int groupEventId);

  bool CheckSelected(const mitk::Point3D& worldPoint);

  mitk::ITKVector3D m_LastTranslatePosition;
  mitk::ITKPoint3D m_LastScalePosition;
  mitk::ITKVector3D m_FirstScaleVector;
  mitk::ITKVector3D m_lastRotatePosition;
  bool m_Grow[3];
};

} // namespace mitk



#endif /* MITKAFFINEINTERACTOR_H_HEADER_INCLUDED_C188C29F */
