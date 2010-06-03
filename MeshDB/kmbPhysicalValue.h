/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : PhysicalValue                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>

#include "MeshDB/kmbTypes.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

namespace kmb{

class PhysicalValue
{
public:
	enum valueType{
		NONE,
		BOOLEAN,
		INTEGER,
		SCALAR,
		VECTOR2,
		VECTOR3,
		POINT3VECTOR3,
		TENSOR6,
		NODEID,
		ELEMENTID,
		VECTOR2WITHINT,
		ARRAY,
		HASH,
		STRING,
		INTHASH,
		UNDEFINED,
		UNKNOWN
	};
	static valueType string2valueType(std::string str);
	static std::string valueType2string(valueType t);
public:
	PhysicalValue(void){}
	virtual ~PhysicalValue(void){}
	virtual valueType getType(void) const = 0;
	virtual double distance(PhysicalValue* other);
	virtual double distanceSq(PhysicalValue* other){
		if(this == other){
			return 0.0;
		}else{
			return 1.0;
		}
	}
	virtual kmb::PhysicalValue* clone(void) = 0;
	virtual bool getValue(double* v) const { return false; }
	virtual bool getValue(long* k) const { return false; }
};

class BooleanValue : public PhysicalValue
{
public:
	BooleanValue(bool v){ this->value = v; }
	virtual ~BooleanValue(void){}
	bool getValue() const { return value; }
	void setValue(bool v){ this->value = v; }
	virtual valueType getType(void) const { return BOOLEAN; }
	virtual double distanceSq(PhysicalValue* other);
	virtual kmb::PhysicalValue* clone(void){
		return new BooleanValue( this->value );
	}
private:
	bool value;
};

class IntegerValue : public PhysicalValue
{
public:
	IntegerValue(int v){ this->value = v; }
	virtual ~IntegerValue(void){}
	int getValue() const { return value; }
	void setValue(int v){ this->value = v; }
	virtual valueType getType(void) const { return INTEGER; }
	virtual double distanceSq(PhysicalValue* other);
	virtual kmb::PhysicalValue* clone(void){
		return new IntegerValue( this->value );
	}
private:
	int value;
};

class ScalarValue : public PhysicalValue
{
public:
	ScalarValue(double v){ this->value = v; }
	virtual ~ScalarValue(void){}
	double getValue() const { return value; }
	void setValue(double v){ this->value = v; }
	virtual valueType getType(void) const { return SCALAR; }
	virtual double distanceSq(PhysicalValue* other);
	virtual kmb::PhysicalValue* clone(void){
		return new ScalarValue( this->value );
	}
	virtual bool getValue(double* v) const {
		*v = value;
		return true;
	}
private:
	double value;
};

class Vector2Value : public PhysicalValue
{
public:
	Vector2Value(double v[2]){
		for(int i=0;i<2;++i){
			this->value[i] = v[i];
		}
	}
	Vector2Value(double u,double v){
		this->value[0] = u;
		this->value[1] = v;
	}
	virtual ~Vector2Value(void){}
	double getValue(int i) const{ return value[i]; }
	double getLength(void) const;
	void setValue(double v,int i){ this->value[i] = v; }
	virtual valueType getType(void) const { return VECTOR2; }
	virtual double distanceSq(PhysicalValue* other);
	virtual kmb::PhysicalValue* clone(void){
		return new Vector2Value( this->value );
	}
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		return true;
	}
private:
	double value[2];
};

class Vector3Value : public PhysicalValue
{
public:
	Vector3Value(double v[3]){
		for(int i=0;i<3;++i){
			this->value[i] = v[i];
		}
	}
	Vector3Value(double u,double v,double w){
		this->value[0] = u;
		this->value[1] = v;
		this->value[2] = w;
	}
	virtual ~Vector3Value(void){}
	double getValue(int i) const{ return value[i]; }
	double getLength(void) const;
	void setValue(double v,int i){ this->value[i] = v; }
	virtual valueType getType(void) const { return VECTOR3; }
	virtual double distanceSq(PhysicalValue* other);
	virtual kmb::PhysicalValue* clone(void){
		return new Vector3Value( this->value );
	}
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		v[2] = value[2];
		return true;
	}
private:
	double value[3];
};

class Vector2WithInt : public PhysicalValue
{
private:
	double value[2];
	long ival;
public:
	Vector2WithInt(double u, double v,long i){
		this->value[0] = u;
		this->value[1] = v;
		this->ival = i;
	}
	virtual ~Vector2WithInt(void){}
	double getValue(int i) const{ return value[i]; }
	long getIValue(void) const{ return ival; };
	void setValue(double u,double v){ this->value[0] = u; this->value[1] = v; }
	void setIValue(long id){ this->ival = ival; }
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		return true;
	}
	virtual bool getValue(long* k) const {
		*k = ival;
		return true;
	}
	virtual valueType getType(void) const { return VECTOR2WITHINT; }
	virtual kmb::PhysicalValue* clone(void){
		return new Vector2WithInt( this->value[0], this->value[1], this->ival );
	}
};






class Point3Vector3Value : public PhysicalValue
{
public:
	Point3Vector3Value(double p[3],double v[3]){
		for(int i=0;i<3;++i){
			this->point[i] = p[i];
			this->value[i] = v[i];
		}
	}
	Point3Vector3Value(double p0,double p1,double p2,double v0,double v1,double v2){
		this->point[0] = p0;
		this->point[1] = p1;
		this->point[2] = p2;
		this->value[0] = v0;
		this->value[1] = v1;
		this->value[2] = v2;
	}
	virtual ~Point3Vector3Value(void){}
	double getValue(int i) const { return value[i]; }
	void setValue(double v,int i){ this->value[i] = v; }
	double getPoint(int i) const { return point[i]; }
	void setPoint(double p,int i){ this->point[i] = p; }
	virtual valueType getType(void) const { return POINT3VECTOR3; }
	virtual kmb::PhysicalValue* clone(void){
		return new Point3Vector3Value(this->point,  this->value );
	}
	virtual bool getValue(double* v) const {
		v[0] = point[0];
		v[1] = point[1];
		v[2] = point[2];
		v[3] = value[0];
		v[4] = value[1];
		v[5] = value[2];
		return true;
	}
private:
	double point[3];
	double value[3];
};

class Tensor6Value : public PhysicalValue
{
public:
	Tensor6Value(double v[6]){
		for(int i=0;i<6;++i){
			this->value[i] = v[i];
		}
	}
	Tensor6Value(double t0,double t1,double t2,double t3,double t4,double t5){
		this->value[0] = t0;
		this->value[1] = t1;
		this->value[2] = t2;
		this->value[3] = t3;
		this->value[4] = t4;
		this->value[5] = t5;
	}
	virtual ~Tensor6Value(void){}
	double getValue(int i) const { return value[i]; }
	void setValue(double v,int i){ this->value[i] = v; }
	virtual valueType getType(void) const { return TENSOR6; }
	virtual kmb::PhysicalValue* clone(void){
		return new Tensor6Value( this->value );
	}
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		v[2] = value[2];
		v[3] = value[3];
		v[4] = value[4];
		v[5] = value[5];
		return true;
	}
private:
	double value[6];
};


class TextValue : public PhysicalValue
{
public:
	TextValue(){}
	TextValue(const char* t){
		str = t;
	}
	virtual ~TextValue(){}
	std::string getValue(void) const { return str; }
	void setValue(const char* t){ str = t; }
	virtual valueType getType(void) const { return STRING; }
	virtual kmb::PhysicalValue* clone(void){
		return new TextValue( str.c_str() );
	}
private:
	std::string str;
};

class ArrayValue : public PhysicalValue
{
public:
	ArrayValue(){}
	ArrayValue( std::vector< PhysicalValue* >& v){
		std::vector< PhysicalValue* >::iterator pIter = v.begin();
		while( pIter != v.end() ){
			values.push_back( (*pIter)->clone() );
			++pIter;
		}
	}
	virtual ~ArrayValue(void){
		std::vector< kmb::PhysicalValue* >::iterator vIter = values.begin();
		while( vIter != values.end() )
		{
			kmb::PhysicalValue* v = *vIter;
			if( v ){
				delete v;
				*vIter = NULL;
			}
			++vIter;
		}
	}
	kmb::PhysicalValue* getValue(int i) const { return values[i]; }
	void setValue(int i,kmb::PhysicalValue* v){ values[i] = v; }
	void appendValue(kmb::PhysicalValue* v){ values.push_back( v ); }
	virtual valueType getType(void) const { return ARRAY; }
	size_t getSize(void) const { return values.size(); }
	virtual kmb::PhysicalValue* clone(void){
		return new ArrayValue( this->values );
	}


	bool checkType(unsigned int i,valueType vType,size_t size=0) const {
		kmb::PhysicalValue* val = this->getValue(i);
		if( val && val->getType() == vType ){
			if( vType == ARRAY && size > 0 ){
				if( reinterpret_cast< kmb::ArrayValue* >(val)->getSize() == size ){
					return true;
				}
			}else{
				return true;
			}
		}
		return false;
	}
private:
	std::vector< kmb::PhysicalValue* > values;
};

class HashValue : public PhysicalValue
{
public:
	HashValue(){}
	HashValue( std::map< std::string, PhysicalValue* >& v){
		std::map< std::string, PhysicalValue* >::iterator pIter = v.begin();
		while( pIter != v.end() ){
			values.insert( std::pair< std::string, PhysicalValue* >(pIter->first,pIter->second->clone()) );
			++pIter;
		}
	}
	virtual ~HashValue(void){
		std::map< std::string, PhysicalValue* >::iterator vIter = values.begin();
		while( vIter != values.end() )
		{
			PhysicalValue* v = vIter->second;
			if(v){
				delete v;
				vIter->second = NULL;
			}
			++vIter;
		}
	}
	kmb::PhysicalValue* getValue(std::string k) const {
		std::map< std::string, kmb::PhysicalValue* >::const_iterator f = values.find(k);
		if( f != values.end() ){
			return f->second;
		}else{
			return NULL;
		}
	}
	void setValue(std::string k,kmb::PhysicalValue* v){
		if( v != NULL ){

			std::map< std::string, kmb::PhysicalValue* >::iterator f = values.find(k);
			if( f != values.end() ){
				delete f->second;
				f->second = NULL;
			}
			values.insert( std::pair<std::string, kmb::PhysicalValue* >(k,v) );
		}
	}
	virtual valueType getType(void) const { return HASH; }
	size_t getSize(void) const { return values.size(); }
	std::map< std::string, kmb::PhysicalValue* >::const_iterator begin(void) const{ return values.begin(); }
	std::map< std::string, kmb::PhysicalValue* >::const_iterator end(void) const{ return values.end(); }
	virtual kmb::PhysicalValue* clone(void){
		return new HashValue( this->values );
	}
	bool checkType(const char* key,valueType vType,size_t size=0) const {
		kmb::PhysicalValue* val = this->getValue(key);
		if( val && val->getType() == vType ){
			if( vType == ARRAY && size > 0 ){
				if( reinterpret_cast< kmb::ArrayValue* >(val)->getSize() == size ){
					return true;
				}
			}else{
				return true;
			}
		}
		return false;
	}
private:
	std::map< std::string, PhysicalValue* >	values;
};

class IntHashValue : public PhysicalValue
{
public:
	IntHashValue(){}
	IntHashValue( std::map< int, PhysicalValue* >& v){
		std::map< int, PhysicalValue* >::const_iterator pIter = v.begin();
		while( pIter != v.end () ){
			values.insert( std::pair< int, PhysicalValue* >(pIter->first,pIter->second->clone()) );
			++pIter;
		}
	}
	virtual ~IntHashValue(void){
		std::map< int, PhysicalValue* >::iterator vIter = values.begin();
		while( vIter != values.end() )
		{
			PhysicalValue* v = vIter->second;
			if(v){
				delete v;
				vIter->second = NULL;
			}
			++vIter;
		}
	}
	kmb::PhysicalValue* getValue(int n) const {
		std::map< int, kmb::PhysicalValue* >::const_iterator f = values.find(n);
		if( f != values.end() ){
			return f->second;
		}else{
			return NULL;
		}
	}
	void setValue(int n,kmb::PhysicalValue* v){
		if( v != NULL ){
			values.insert( std::pair<int, kmb::PhysicalValue* >(n,v) );
		}
	}
	virtual valueType getType(void) const { return INTHASH; }
	size_t getSize(void) const { return values.size(); }
	std::map< int, kmb::PhysicalValue* >::const_iterator begin(void) const{ return values.begin(); }
	std::map< int, kmb::PhysicalValue* >::const_iterator end(void) const{ return values.end(); }
	virtual kmb::PhysicalValue* clone(void){
		return new IntHashValue( this->values );
	}
	bool checkType(unsigned int i,valueType vType,size_t size=0) const {
		const kmb::PhysicalValue* val = this->getValue(i);
		if( val && val->getType() == vType ){
			if( vType == ARRAY && size > 0 ){
				if( reinterpret_cast< const kmb::ArrayValue* >(val)->getSize() == size ){
					return true;
				}
			}else{
				return true;
			}
		}
		return false;
	}
private:
	std::map< int, PhysicalValue* > values;
};

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

