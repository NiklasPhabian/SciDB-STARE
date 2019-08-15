#include "stareLibrary.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "SpatialIndex.h"
#include "SpatialVector.h"
#include "SpatialInterface.h"
#include "TemporalIndex.h"

#include "HtmRangeIterator.h"
#include "BitShiftNameEncoding.h"
#include "EmbeddedLevelNameEncoding.h"


using namespace log4cxx;
using namespace log4cxx::helpers;

using namespace scidb;
using namespace stare;
using namespace boost::assign;

enum
  {
    STARE_ERROR1 = SCIDB_USER_ERROR_CODE_START
  };


STARE stareIndex;

// Spatial
static void stareFromLevelLatLon(const scidb::Value** args, scidb::Value* res, void* v) {
    int     iarg       = 0;
    int64_t depth      = args[iarg++]->getInt64();
    float64 latDegrees = args[iarg++]->getDouble();
    float64 lonDegrees = args[iarg++]->getDouble();   
    STARE_ArrayIndexSpatialValue indexValue = stareIndex.ValueFromLatLonDegrees(latDegrees, lonDegrees, depth);            
    *(STARE_ArrayIndexSpatialValue*)res->data() = indexValue;
}

// Temporal
static void stareFromTimeStamp(const scidb::Value** args, scidb::Value* res, void* v) {
    int iarg        = 0;    
    int year        = 2015;
    int month       = 12;
    int day         = 3;
    int hour        = 4;
    int minute      = 5;
    int second      = 7;
    int millisecond = 30;
    int resolution  = 3;
    int type        = 2;    
    STARE_ArrayIndexTemporalValue indexValue = stareIndex.ValueFromUTCDatetime(year, month, day, hour, minute, second, millisecond, resolution, type);        
    *(STARE_ArrayIndexTemporalValue*)res->data() = indexValue ;    
}


// Converters
static void spatialIndexValueToString (const scidb::Value** args, scidb::Value* res, void* v) {      
    STARE_ArrayIndexSpatialValue& id = *(STARE_ArrayIndexSpatialValue*)args[0]->data();        
    LOG4CXX_INFO(stare::logger, "STARE idx: " << id);
    res->setString(to_string(id));  
}

static void temporalIndexValueToString (const scidb::Value** args, scidb::Value* res, void* v) {      
    STARE_ArrayIndexTemporalValue& id = *(STARE_ArrayIndexTemporalValue*)args[0]->data();        
    LOG4CXX_INFO(stare::logger, "STARE idx: " << id);
    res->setString(to_string(id));  
}

REGISTER_CONVERTER(spatialIndexValue, string, EXPLICIT_CONVERSION_COST, spatialIndexValueToString);
REGISTER_CONVERTER(temporalIndexValue, string, EXPLICIT_CONVERSION_COST, temporalIndexValueToString);


vector<Type> _types;
EXPORTED_FUNCTION const vector<Type>& GetTypes() {
  return _types;
}

vector<FunctionDescription> _functionDescs;
EXPORTED_FUNCTION const vector<FunctionDescription>& GetFunctions() {
  return _functionDescs;
}


static class stareLibrary {
public:
  stareLibrary() {

    // BasicConfigurator::configure();
    LOG4CXX_INFO(stare::logger, "Entering constructor.");

    Type spatialIndexValueType("stare", sizeof(STARE)*8);   // size in bits
    Type temporalIndexValueType("stare", sizeof(STARE)*8);  // size in bits
    _types.push_back(spatialIndexValueType);
    _types.push_back(temporalIndexValueType);
    
    _functionDescs.push_back(FunctionDescription("stareFromLevelLatLon",
                                                list_of(TID_INT64)(TID_DOUBLE)(TID_DOUBLE),
                                                TypeId("int64"),
                                                &stareFromLevelLatLon));
   
    _functionDescs.push_back(FunctionDescription("stareFromTimeStamp",
                                                list_of(TID_INT64),
                                                TypeId("int64"),
                                                &stareFromTimeStamp));
   
   
   
   
    _errors[STARE_ERROR1] = "STARE construction error.";
    scidb::ErrorsLibrary::getInstance()->registerErrors("stare",&_errors);
  }

  ~stareLibrary() {
    scidb::ErrorsLibrary::getInstance()->unregisterErrors("stare");
  }

  private:
  scidb::ErrorsLibrary::ErrorsMessages _errors;

} _instance;


