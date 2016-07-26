/*----------------------------------------------------------------------
#                                                                      #
# Software Name : Advance/REVOCAP version 3.2                          #
# Class Name : IntegrationPoint                                        #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/08/22     #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"

/**
 * 積分点での物理量を管理するための要素番号と積分点のインデックスの組
 */

namespace kmb{

class IntegrationPoint
{
public:
	kmb::elementIdType elementId;
	kmb::idType integrationIndex;
	IntegrationPoint(kmb::elementIdType _elementId=kmb::Element::nullElementId, kmb::idType _integrationIndex=kmb::nullId);
	~IntegrationPoint(void);
	// std::set や std::map に使うために operator< を定義しておく
	bool operator<(const IntegrationPoint &other)const{
		return (elementId < other.elementId) ||
			(elementId==other.elementId && integrationIndex < other.integrationIndex);
	}
	bool operator==(const IntegrationPoint &other)const{
		return (elementId == other.elementId) && (integrationIndex == other.integrationIndex);
	}
	bool operator!=(const IntegrationPoint &other)const{
		return (elementId != other.elementId) || (integrationIndex != other.integrationIndex);
	}
	IntegrationPoint& operator=(const IntegrationPoint &other){
		elementId = other.elementId;
		integrationIndex = other.integrationIndex;
		return *this;
	}
};

}
