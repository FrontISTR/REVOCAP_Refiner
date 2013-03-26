/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : ListItem                                                #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/






#pragma once

namespace kmb{

template<typename T>
class ListItem : public T
{
protected:
	ListItem<T>* previousItem;
	ListItem<T>* nextItem;
public:
	ListItem(T &t) : T(t), previousItem(NULL), nextItem(NULL) {};
	virtual ~ListItem(void){};

	ListItem<T>* getPrevious(void) const { return previousItem; }
	void setPrevious(ListItem<T>* item){ previousItem = item; }
	ListItem<T>* getNext(void) const { return nextItem; }
	void setNext(ListItem<T>* item){ nextItem = item; }



	void detach(void){
		if( previousItem ){
			previousItem->nextItem = nextItem;
		}
		if( nextItem ){
			nextItem->previousItem = previousItem;
		}
		previousItem = NULL;
		nextItem = NULL;
	}

	void attach(ListItem<T>* last){
		if( last ){
			last->nextItem = this;
			previousItem = last;
		}
	}
};

}
