/*----------------------------------------------------------------------
#                                                                      #
# Software Name : Advance/REVOCAP version 3.2                          #
# Class Name : IntegrationPoint                                        #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2013/08/22     #
#                                                                      #
----------------------------------------------------------------------*/

#include "kmbIntegrationPoint.h"

kmb::IntegrationPoint::IntegrationPoint(kmb::elementIdType _elementId, kmb::idType _integrationIndex)
: elementId(_elementId), integrationIndex(_integrationIndex)
{}

kmb::IntegrationPoint::~IntegrationPoint(void)
{}
