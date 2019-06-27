
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

using namespace stare;
using namespace scidb;
using namespace boost::assign;

enum
  {
    HSTM_ERROR1 = SCIDB_USER_ERROR_CODE_START
  };


static STARE _stareIndex;

static void stareFromLevelLatLon(const scidb::Value** args, scidb::Value* res, void* v) {
    int     iarg       = 0;
    int64_t depth      = args[iarg++]->getInt64();
    float64 latDegrees = args[iarg++]->getDouble();
    float64 lonDegrees = args[iarg++]->getDouble();
        
    uint64 id = _stareIndex.ValueFromLatLonDegrees(latDegrees, lonDegrees, depth);
    

    // hIndex->range->addRange(1,1);
    //*(STARE*)res->data() = *hIndex;

}

vector<Type> _types;
EXPORTED_FUNCTION const vector<Type>& GetTypes() {
  return _types;
}

vector<FunctionDescription> _functionDescs;
EXPORTED_FUNCTION const vector<FunctionDescription>& GetFunctions() {
  return _functionDescs;
}

static class HstmIndexLibrary {
public:
  HstmIndexLibrary() {

    // BasicConfigurator::configure();
    LOG4CXX_INFO(stare::logger, "Entering constructor.");

    Type hstmType("stare",sizeof(STARE)*8); // size in bits
    _types.push_back(hstmType);

    // Functions registered with SciDB.
    // void funX(const scidb::Value** args, scidb::Value* res, void*) {}
    // REGISTER_FUNCTION(funX, list_of("argtype1")("argtype2"), "returntype", functionRef);

    // hstmFromLevelLatLon (level,degrees,degrees toh stm index)
    // REGISTER_FUNCTION(hstmFromLevelLatLon,list_of("int64")("double")("double"),"hstm",hstmFromLevelLatLon);
   _functionDescs.push_back(FunctionDescription("stareFromLevelLatLon",
                                                list_of(TID_INT64)(TID_DOUBLE)(TID_DOUBLE),
                                                TypeId("hstm"),
                                                &stareFromLevelLatLon));
   

    _errors[HSTM_ERROR1] = "HSTM construction error.";
    scidb::ErrorsLibrary::getInstance()->registerErrors("hstm",&_errors);
  }

  ~HstmIndexLibrary() {
    scidb::ErrorsLibrary::getInstance()->unregisterErrors("hstm");
  }

  private:
  scidb::ErrorsLibrary::ErrorsMessages _errors;

} _instance;


