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
    float64 latDegrees = args[0]->getDouble();
    float64 lonDegrees = args[1]->getDouble();
    int64_t resolution = args[2]->getInt64();

    STARE_ArrayIndexSpatialValue id = stareIndex.ValueFromLatLonDegrees(latDegrees, lonDegrees, resolution);
    *(STARE_ArrayIndexSpatialValue*)res->data() = id;
}


// Temporal
static void stareFromUTCDateTime(const scidb::Value** args, scidb::Value* res, void* v) {
    struct tm tm;
    resolution = args[0];
    time_t dt = args[1]->getDateTime(); // SciDB understands time_t as seconds since UNIX epoch
    gmtime_r(&dt, &tm);                 // gmtime_r converts to tm struct, which stores ...
    tm.tm_year += 1900;                 // years since 1900
    tm.tm_mon += 1;                     // and months 0-based, while STARE stores months 1-based

    STARE_ArrayIndexTemporalValue indexValue = stareIndex.ValueFromUTCDatetime(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, 0, resolution, 2);
    *(STARE_ArrayIndexTemporalValue*)res->data() = indexValue;
}

static void convDateTime2TimeT(const scidb::Value** args, scidb::Value* res, void*) {
    time_t dt = args[0]->getDateTime();
    res->setUint64(static_cast<uint64_t>(dt));
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
   
    _functionDescs.push_back(FunctionDescription("stareFromUTCDateTime",
                                                list_of(TID_INT64),
                                                TypeId("int64"),
                                                &stareFromTimeStamp));
    
    _functionDescs.push_back(FunctionDescription("convDateTime2TimeT",
                                                list_of(TID_DATETIME),
                                                TypeId("int64"),
                                                &strareFromDateTime);
   
    _errors[STARE_ERROR1] = "STARE construction error.";
    scidb::ErrorsLibrary::getInstance()->registerErrors("stare",&_errors);
  }

  ~stareLibrary() {
    scidb::ErrorsLibrary::getInstance()->unregisterErrors("stare");
  }

  private:
  scidb::ErrorsLibrary::ErrorsMessages _errors;

} _instance;


