
#include "hstmIndexLibrary.h"

// include log4cxx header files
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/helpers/exception.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "SpatialIndex.h"
#include "SpatialVector.h"
#include "SpatialInterface.h"

#include "HtmRangeIterator.h"
#include "BitShiftNameEncoding.h"
#include "EmbeddedLevelNameEncoding.h"


using namespace log4cxx;
using namespace log4cxx::helpers;
// LoggerPtr logger(Logger::getLogger("HstmIndexLibrary"));

using namespace hstm;
// using namespace std;
using namespace scidb;
using namespace boost::assign;

enum
  {
    HSTM_ERROR1 = SCIDB_USER_ERROR_CODE_START
  };

// define MOCK_HSTM 1

// The struct used to store the data inside SciDB.
  // Start with 0-length ranges (individual triangles)
  // Then 1-range ranges (an interval)
  // Then multi-range ranges (multiple intervals)
  // Consider multi-level ranges

  // For indexed points, it probably doesn't hurt too much to save all levels of resolution,
  // and then control the resolution via the level field. Complicates comparison and
  // first level of difference calculations.

  // resolution estimate is 0.25*2*RE*PI/2**level, if level=0 is for the top level, ~RE/4.
  // resolution-estimate(27) ~= 7.45 cm // I think with left-justified we lose a couple of levels.
  // resolution-estimate(23) ~= 1.2   m
  // resolution-estimate(14) ~= 0.6  km
  // resolution-estimate(13) ~= 1.2  km
  // resolution-estimate(6)  ~= 150  km

  // SpatialIndex index(23,5);
  // Wrong! HstmIndex(int level=23, int buildLevel=5): r(HtmRange(level,buildLevel) {}

// Number at each level 8*4**level


// Maybe have a second struct for HtmRange as opposed to just an index. Re: region vs. point.

// void funX(const Value** args, Value* res, void* v) {}

const static int _idLevel    = 23;
const static int _buildLevel = 5;
static SpatialIndex _index = SpatialIndex(_idLevel,_buildLevel);  // Should we make this static?
// 
static void hstmFromLevelLatLon(const scidb::Value** args, scidb::Value* res, void* v) {
  int     iarg       = 0;
  int64_t level      = args[iarg++]->getInt64();
  float64 latDegrees = args[iarg++]->getDouble();
  float64 lonDegrees = args[iarg++]->getDouble();

  HstmIndex *hIndex = new HstmIndex;

  SpatialVector x; x.setLatLonDegrees(latDegrees,lonDegrees);

  if(_index.getMaxlevel() != level)_index.setMaxlevel(level);
  uint64 htm_Id = _index.idByPoint(x); // TODO Need to convert/ensure we're in the correct format - LeftJustified.

  BitShiftNameEncoding rightJustified(htm_Id);
  hIndex->range->addRange(rightJustified.leftJustifiedId_NoDepthBit());

  // hIndex->range->addRange(1,1);
  *(HstmIndex*)res->data() = *hIndex;

}

// 
// //  void xxx (const scidb::Value** args, scidb::Value* res, void* v) {}
// 
// static void hstmFromLevelXYZ (const scidb::Value** args, scidb::Value* res, void* v) {
//   int     iarg  = 0;
//   int64_t level = args[iarg++]->getInt64(); // uint64?
//   float64 x     = args[iarg++]->getDouble();
//   float64 y     = args[iarg++]->getDouble();
//   float64 z     = args[iarg++]->getDouble();
// 
//   HstmIndex& htmId = *(HstmIndex*)res->data();
//   
//   SpatialVector point(x,y,z);
//   
//   if(_index.getMaxlevel() != level)_index.setMaxlevel(level);
//   uint64 htm_Id = _index.idByPoint(point); // TODO Need to convert/ensure we're in the correct format - LeftJustified.
// 
//   //  htmId.range.addRange(htm_Id,htm_Id);
//   htmId.range = htm_Id;
// }
// 
static void symbolFromHstm (const scidb::Value** args, scidb::Value* res, void* v) {
  int         iarg = 0;
  HstmIndex&  hIndex = *(HstmIndex*)args[iarg++]->data();
  stringstream ss;
  // Call on the embedded level encoding (left justified).  Or the "classic" symbol?
  hIndex.range->print(ss,true); // How many pairs will this print?
  res->setString(ss.str().c_str());
}

static void indexFromHstm (const scidb::Value** args, scidb::Value* res, void* v) {
  int         iarg = 0;
  HstmIndex&  hIndex = *(HstmIndex*)args[iarg++]->data();
  // stringstream ss;
  // hIndex.range->print(ss,true); // How many pairs will this print?
  // Call on the embedded level encoding (left justified).  Or the "classic" symbol?
  Key lo = 0x8000000000000000, hi = 0x8000000000000000; // MinValue for int64. Standing in for an Invalid ID.
  hIndex.range->reset();
  int status = hIndex.range->getNext(&lo,&hi);
  EmbeddedLevelNameEncoding leftJustified = EmbeddedLevelNameEncoding(lo);
  int64 leftID = leftJustified.getSciDBLeftJustifiedFormat();
  LOG4CXX_DEBUG(logger, L"hstm::indexFromHstm lo     " << hex << (lo) << dec);
  LOG4CXX_DEBUG(logger, L"hstm::indexFromHstm leftID " << hex << (leftID) << dec);
  res->setInt64(leftID);
}
static void hstmFromIndex (const scidb::Value** args, scidb::Value* res, void* v) {
  int         iarg = 0;
  int64_t     idx  = args[iarg++]->getInt64();

  EmbeddedLevelNameEncoding leftJustified;
  leftJustified.setIdFromSciDBLeftJustifiedFormat(idx);

  HstmIndex  *hIndex = new HstmIndex;
  hIndex->range->addRange(leftJustified.getId_NoLevelBit());

  *(HstmIndex*)res->data() = *hIndex;
}
// 
// static void hstmIntersect (const scidb::Value** args, scidb::Value* res, void* v) {}
// static void hstmContains (const scidb::Value** args, scidb::Value* res, void* v) {}
// static void hstmMergeRange (const scidb::Value** args, scidb::Value* res, void* v) {}
// static void hstmAddRange (const scidb::Value** args, scidb::Value* res, void* v) {}
// static void hstmNRanges (const scidb::Value** args, scidb::Value* res, void* v) {}
 
static void hstmLeftJustifiedNoLevel (const scidb::Value** args, scidb::Value* res, void* v) {
  int         iarg = 0;
  HstmIndex&  hIndex = *(HstmIndex*)args[iarg++]->data();
  HtmRangeMultiLevel    *range = hIndex.range;
  // TODO reimplement for ranges etc.
  uint64_t leftID = 0;
  // stringstream ss;
  // ss << showbase << hex << (*range) << dec;
  // LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel " << (ss.str().c_str()));
  if(range->nranges() > 0) {
    //    range->print(HtmRange::LOWS,ss,true); // How many pairs will this print?
    //    LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel name: " << (ss.str().c_str()));
    // TODO The following is wrong but it gets the ball rolling.
    Key lo,hi; // in SciDB
    range->reset();
    range->getNext(lo,hi);
    LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel lo,hi: " << lo << " " << hi);
    if(lo != 0){
      // EmbeddedLevelNameEncoding *name = new EmbeddedLevelNameEncoding(lo);
      // range->print(HtmRange::LOWS,ss,true); // How many pairs will this print?
      //      LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel name00: " << (ss.str().c_str()));
      //char* idName = new char[ss.str().length()+1]; //  = (ss.str().c_str());
      // char idName[80];
      // char idName[ss.str().length()+1];
      // strcpy(idName,ss.str().c_str());
      // LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel name10: " << (ss.str().c_str()));
      // ss.str(""); 
      // ss << idName;
      // LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel name11: " << (ss.str().c_str()));
      // TODO Why does idName seem to differ from getName below?
      //      BitShiftNameEncoding *rightJustified = new BitShiftNameEncoding(idName);
      // TODO Streamline the process of going between right and left justification.
      //old      BitShiftNameEncoding *rightJustified = new BitShiftNameEncoding(lo);
      //old      LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel name15: " << rightJustified->getName());
      //old      EmbeddedLevelNameEncoding *name = new EmbeddedLevelNameEncoding(rightJustified->getName());
      EmbeddedLevelNameEncoding leftJustified;
      leftJustified.setIdFromSciDBLeftJustifiedFormat(lo);
      //old      LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel name20: " << name->getName());

      //old      leftID = (uint64_t)name->getId_NoEmbeddedLevel(); // TODO Be careful about digits beyond the level.
      leftID = leftJustified.getId_NoEmbeddedLevel();
      //old      delete rightJustified, name;
    }
  }
  // ss.str("");
  // ss << hex << (uint64_t)leftID << dec;
  // LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel leftID ss: " << (ss.str().c_str()));
  // LOG4CXX_DEBUG(logger, L"hstm::leftJustifiedNoLevel leftID ui: " << (uint64_t)leftID);
  *(uint64_t*)res->data() = leftID;

}

static void hstmRightJustifiedNoLevel (const scidb::Value** args, scidb::Value* res, void* v) {
  int         iarg = 0;
  HstmIndex&  hIndex = *(HstmIndex*)args[iarg++]->data();
  HtmRangeMultiLevel    *range = hIndex.range;
  // TODO reimplement for ranges etc.
  uint64_t rightID = 0;
  stringstream ss;
  if(range->nranges() > 0) {
    Key lo, hi;
    range->reset();
    range->getNext(lo,hi);
    if(lo != 0){
      // BitShiftNameEncoding *rightJustified = new BitShiftNameEncoding(lo);
      // delete rightJustified;
      rightID = lo; // TODO handle difference between range and id
    }
    *(uint64_t*)res->data() = rightID;
  }
}

static void hstmLevel (const scidb::Value** args, scidb::Value* res, void* v) {
  int         iarg = 0;
  HstmIndex&  hIndex = *(HstmIndex*)args[iarg++]->data();
  HtmRangeMultiLevel    *range = hIndex.range;
  uint64_t level = -999;
  stringstream ss;
  if(range->nranges() > 0) {
    Key lo, hi;  // TODO need to treat ranges better.
    range->reset();
    range->getNext(lo,hi);
    if(lo != 0){
      level = levelOfId(lo); // TODO handle difference between range and id
    }
    *(uint64_t*)res->data() = level;
  }
}

static void hstmIdToLevel (const scidb::Value** args, scidb::Value* res, void* v) {
  int        iarg     = 0;
  HstmIndex& hIndex   = *(HstmIndex*)args[iarg++]->data();
  HtmRangeMultiLevel   *range   = hIndex.range;
  uint64_t   newLevel = args[iarg++]->getInt64();
  uint64_t  level     = -999;

  // TODO Question:  What is the native format of HtmRange?
  // What TODO here depends on what range is.  Currently range is from _index,
  // so it is in right justified bits.

  Key lo = 0, hi = 0;  // TODO need to treat ranges better.
  LOG4CXX_DEBUG(logger, L"hstm::idToLevel nr " << range->nranges());
  if(range->nranges() > 0) {
    range->reset();
    range->getNext(lo,hi);
    LOG4CXX_DEBUG(logger, L"hstm::idToLevel lo,hi 1 " << lo << " " << hi);
    if(lo != 0){
      level = levelOfId(lo); // TODO handle difference between range and id
    }

    // TODO Assume RightJustified
    if( level != newLevel ) {
      if( newLevel > level ) {
	lo = lo << (2*( newLevel - level ));
	hi = hi << (2*( newLevel - level ));
      } else {
	lo = lo >> (2*( level - newLevel ));
	hi = hi >> (2*( level - newLevel ));
      }
    }
    LOG4CXX_DEBUG(logger, L"hstm::idToLevel lo,hi 2 " << lo << " " << hi);
  }

  // TODO Let's assume for now HtmRange is in rightJustified for now.
  HstmIndex *newIndex;
  if( lo != 0 ) {
    newIndex = new HstmIndex(lo,hi);
    // *(HstmIndex*)res->data() = HstmIndex(id);
  } else {
    newIndex = new HstmIndex;
    // *(HstmIndex*)res->data() = HstmIndex();
  }
  LOG4CXX_DEBUG(logger, L"hstm::idToLevel " << lo << " " << hi << " " << " " << level << " " << newLevel);
  *(HstmIndex*)res->data() = *newIndex;
}

// static void hstmLT (const scidb::Value** args, scidb::Value* res, void* v) {}
// static void hstmGT (const scidb::Value** args, scidb::Value* res, void* v) {}
static void hstmEQ (const scidb::Value** args, scidb::Value* res, void* v) {
  int         iarg = 0;
  HstmIndex&  hIndex0 = *(HstmIndex*)args[iarg++]->data();
  HstmIndex&  hIndex1 = *(HstmIndex*)args[iarg++]->data();

  res->setBool(hIndex0.equalp(&hIndex1));

}
// static void hstmNE (const scidb::Value** args, scidb::Value* res, void* v) {}
// static void hstmEqualToLevel (const scidb::Value** args, scidb::Value* res, void* v) {}
// 
// /// Make a new hstmIndex from an int64.  A lot like construct... below.
static void int64ToHstm (const scidb::Value** args, scidb::Value* res, void* v) {
  int     iarg       = 0;
  int64_t id         = args[iarg++]->getInt64(); // uint64?
  HstmIndex *hIndex = new HstmIndex;
  LOG4CXX_DEBUG(logger, L"hstm::int64ToHstm " << 999);
  if( id != 0 ) {
    hIndex->range->addRange(id,id);
  }
  *(HstmIndex*)res->data() = *hIndex;
}
// 
void hstm::stringToHstm (const scidb::Value** args, scidb::Value* res, void* v) {
  // TODO Add some smarts to stringToHstm. Strings could be symbolic or hex...
  int     iarg       = 0;
  std::string symbol = args[iarg++]->getString();
  LOG4CXX_DEBUG(logger, L"hstm::stringToHstm " << "000 " << symbol.c_str());
  HstmIndex *hIndex = new HstmIndex;
  LOG4CXX_DEBUG(logger, L"hstm::stringToHstm " << 100);
  hIndex->range->parse(symbol);
  LOG4CXX_DEBUG(logger, L"hstm::stringToHstm " << 200);
  *(HstmIndex*)res->data() = *hIndex;
  LOG4CXX_DEBUG(logger, L"hstm::stringToHstm " << 999);
}
// 
static void hstmToString (const scidb::Value** args, scidb::Value* res, void* v) {
  HstmIndex& hIndex = *(HstmIndex*)args[0]->data();
  HtmRangeMultiLevel   *range = hIndex.range;
  stringstream ss;
  LOG4CXX_DEBUG(logger, L"hstm::hstmToString " << 999);
  int64_t nranges = -1;
  LOG4CXX_DEBUG(logger, L"hstm::hstmToString " << 998);
  // ss << range;
  // LOG4CXX_DEBUG(logger, L"hstm::hstmToString    ss: " << (ss.str().c_str()));
  nranges = range->nranges();
  // LOG4CXX_DEBUG(logger, L"hstm::hstmToString    ss: " << nranges);
  if(!nranges) {
    // res->setString("invalid"); // Poke in the invalid htmId.
    res->setString("x0 x0"); // Poke in the invalid htmId.
    //    res->setString("pig");
  } else {
    // ss << showbase << hex << (*range) << dec;
    ss << hex << (*range) << dec;
    LOG4CXX_DEBUG(logger, L"hstm::hstmToString default*r " << (ss.str().c_str()));
    // res->setString("poke");
    res->setString(ss.str().c_str());
  }
  LOG4CXX_DEBUG(logger, L"hstm::hstmToString " << 997);
}

// 
static void hstmToInt64 (const scidb::Value** args, scidb::Value* res, void* v) {
  HstmIndex& hIndex = *(HstmIndex*)args[0]->data();
  HtmRangeMultiLevel   *range = hIndex.range;
  int64_t id_left;
  LOG4CXX_DEBUG(logger, L"hstm::hstmToInt64 " << 999);
  int64_t nranges = -1;
  LOG4CXX_DEBUG(logger, L"hstm::hstmToInt64 " << 998);
  nranges = range->nranges();
  if(!nranges) {
    res->setInt64(0); // Poke in the invalid htmId.
  } else {
    // ss << showbase << hex << (*range) << dec;
    Key lo = -1, hi = -1;
    // TODO The following is wrong, but gets the ball rolling.

    // Okay, here's what we need to do.
    // 1. Eliminate depth bit.
    // 2. Move N/S bit to the sign bit.
    // 3. Move the tree bits to fit within the 62 bits minus the level bit field.
    // 4. Maybe move the level bits into the right hand bits of the index.

    range->reset();
    int status = range->getNext(&lo,&hi); // HtmRangeMultiLevel is already LJNL = LeftJustNoLevel
    LOG4CXX_DEBUG(logger, L"hstm::hstmToInt64 lo " << (lo));

    EmbeddedLevelNameEncoding name = EmbeddedLevelNameEncoding(lo);

    // Just get the first index of the first element of the range.
    id_left = name.getSciDBLeftJustifiedFormat(); // Put it into SciDB format for printing/indexing/etc.

    LOG4CXX_DEBUG(logger, L"hstm::hstmToInt64 ss " << (id_left));
    // Question:  What is the format of the intger here?
    res->setInt64(id_left);
  }
  LOG4CXX_DEBUG(logger, L"hstm::hstmToInt64 " << 997);
}

static void equal_Int64Hstm (const scidb::Value** args, scidb::Value* res, void* v) {
  int64_t       idx = args[0]->getInt64();
  HstmIndex& hIndex = *(HstmIndex*)args[1]->data();
  bool result = false;

  HtmRangeMultiLevel *range = hIndex.range;

  Key lo = -1, hi = -1;
  int status = range->getNext(&lo,&hi);
  int64_t hIdx = lo; // Just get the first index of the first element of the range.

  result = idx == hIdx;

  res->setBool(result);
}

static void constructDefaultHstmIndex (const scidb::Value** args, scidb::Value* res, void* v) {
  LOG4CXX_DEBUG(logger, L"hstm::constructDefaultHstmIndex " << 999);
  // *(HstmIndex*)res->data() = HstmIndex();
  // Is there a destructor somewhere that needs to be defined?
  stringstream ss;

  //  HstmIndex& resultId = *(HstmIndex*)res->data();

  HstmIndex *hIndex = new HstmIndex;
  LOG4CXX_DEBUG(logger, L"hstm::constructDefaultHstmIndex    hIndex-nr: " << hIndex->range->nranges());
  LOG4CXX_DEBUG(logger, L"hstm::constructDefaultHstmIndex " << 998);

  //  *(HstmIndex*)res->data() = HstmIndex();
  *(HstmIndex*)res->data() = *hIndex;
  ss << hex << ((HstmIndex*)res->data())->range << dec;
  LOG4CXX_DEBUG(logger, L"hstm::constructDefaultHstmIndex    ss: " << (ss.str().c_str()));
  int64_t nranges = ((HstmIndex*)res->data())->range->nranges();
  LOG4CXX_DEBUG(logger, L"hstm::constructDefaultHstmIndex    nr: " << (nranges));
}
static void constructHstmIndexFromInt64 (const scidb::Value** args, scidb::Value* res, void* v) {
  int     iarg       = 0;
  int64_t id         = args[iarg++]->getInt64(); // uint64?
  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromInt64 " << 999);
  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromInt64 " << id);

  HstmIndex *hIndex;
  if( id != 0 ) {
    hIndex = new HstmIndex(id);
    // *(HstmIndex*)res->data() = HstmIndex(id);
  } else {
    hIndex = new HstmIndex;
    // *(HstmIndex*)res->data() = HstmIndex();
  }
  *(HstmIndex*)res->data() = *hIndex;
  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromInt64 " << 100);
}
static void constructHstmIndexFromInt642 (const scidb::Value** args, scidb::Value* res, void* v) {
  int     iarg       = 0;
  int64_t lo         = args[iarg++]->getInt64(); // uint64?
  int64_t hi         = args[iarg++]->getInt64(); // uint64?
  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromInt642 " << 999);

  HstmIndex *hIndex = new HstmIndex;

  if( lo != 0 && hi != 0 ) {
    if(lo<=hi) {
      hIndex->range->addRange(lo,hi);
    } else {
      hIndex->range->addRange(hi,lo);
    }
  }
  *(HstmIndex*)res->data() = *hIndex;
  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromInt642 " << 100);
}
static void constructHstmIndexFromStringSymbol (const scidb::Value** args, scidb::Value* res, void* v) {
  int     iarg       = 0;
  //  int64_t id         = args[iarg++]->getInt64(); // uint64?
  std::string symbol = args[iarg++]->getString();

  HstmIndex *hIndex = new HstmIndex;
  hIndex->range->parse(symbol);

  *(HstmIndex*)res->data() = *hIndex;

  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromStringSymbol " << 100);
  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromStringSymbol " << symbol.c_str());
  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromStringSymbol " << 200);
  stringstream ss;
  hIndex->range->print(HtmRange::BOTH,ss,true);
  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromStringSymbol " << ss.str().c_str());
  LOG4CXX_DEBUG(logger, L"hstm::constructHstmIndexFromStringSymbol " << 999);

  // TODO Need deletes for the news? Not for hIndex. It's travelling along. Consider auto pointers...
}

// //  void xxx (const scidb::Value** args, scidb::Value* res, void* v) {}
// 
REGISTER_CONVERTER(int64,hstm,IMPLICIT_CONVERSION_COST,int64ToHstm);
REGISTER_CONVERTER(hstm,int64,IMPLICIT_CONVERSION_COST,hstmToInt64);
REGISTER_CONVERTER(string,hstm,EXPLICIT_CONVERSION_COST,stringToHstm);
REGISTER_CONVERTER(hstm,string,EXPLICIT_CONVERSION_COST,hstmToString);
// 
vector<Type> _types;
EXPORTED_FUNCTION const vector<Type>& GetTypes() {
  return _types;
}

vector<FunctionDescription> _functionDescs;
EXPORTED_FUNCTION const vector<FunctionDescription>& GetFunctions() {
  return _functionDescs;
}

vector<AggregatePtr> _aggregates;
EXPORTED_FUNCTION const vector<AggregatePtr>& GetAggregates() {
  return _aggregates;
}

static class HstmIndexLibrary {
public:
  HstmIndexLibrary() {

    // BasicConfigurator::configure();
    LOG4CXX_INFO(hstm::logger, "Entering constructor.");

    Type hstmType("hstm",sizeof(HstmIndex)*8); // size in bits
    _types.push_back(hstmType);

    

// Functions registered with SciDB.
// void funX(const scidb::Value** args, scidb::Value* res, void*) {}
// REGISTER_FUNCTION(funX, list_of("argtype1")("argtype2"), "returntype", functionRef);

    _functionDescs.push_back(FunctionDescription("hstm", ArgTypes(), TypeId("hstm"), &constructDefaultHstmIndex));
    _functionDescs.push_back(FunctionDescription("hstm", list_of(TID_INT64), TypeId("hstm"), &constructHstmIndexFromInt64));
    _functionDescs.push_back(FunctionDescription("hstm", list_of(TID_INT64)(TID_INT64), TypeId("hstm"), &constructHstmIndexFromInt642));
    _functionDescs.push_back(FunctionDescription("hstm", list_of(TID_STRING), TypeId("hstm"), &constructHstmIndexFromStringSymbol));

// // hstmFromLevelLatLon (level,degrees,degrees toh stm index)
// // REGISTER_FUNCTION(hstmFromLevelLatLon,list_of("int64")("double")("double"),"hstm",hstmFromLevelLatLon);
   _functionDescs.push_back(FunctionDescription("hstmFromLevelLatLon",list_of(TID_INT64)(TID_DOUBLE)(TID_DOUBLE),TypeId("hstm"),&hstmFromLevelLatLon));
// 
// // hstmFromLevelXYZ
// // REGISTER_FUNCTION(hstmFromLevelXYZ,list_of("int64")("double")("double")("double"),"hstm",hstmFromLevelXYZ);
//   _functionDescs.push_back(FunctionDescription("hstmFromLevelXYZ",list_of(TID_INT64)(TID_DOUBLE)(TID_DOUBLE)(TID_DOUBLE),TypeId("hstm"),&hstmFromLevelXYZ));

// 
// // construct the symbolic representation of an hstm index
// // REGISTER_FUNCTION(symbol,list_of("hstm"),"string",symbolFromHstm);
   _functionDescs.push_back(FunctionDescription("symbol",list_of("hstm"),TID_STRING,&symbolFromHstm));

   // calculate an int64 index
   _functionDescs.push_back(FunctionDescription("indexFromHstm",list_of("hstm"),TID_INT64,&indexFromHstm));
   _functionDescs.push_back(FunctionDescription("hstmFromIndex",list_of(TID_INT64),TypeId("hstm"),&hstmFromIndex));
// 
// // intersect a b => a.intersect(b)
// // REGISTER_FUNCTION(intersect,list_of("hstm")("hstm"),"bool",hstmIntersect);
//   _functionDescs.push_back(FunctionDescription("intersect",list_of("hstm")("hstm"),TID_BOOL,&hstmIntersect));
// 
// // contains a b => b is in a, or a.contains(b)
// // REGISTER_FUNCTION(contains,list_of("hstm")("hstm"),"bool",hstmContains);
//   _functionDescs.push_back(FunctionDescription("contains",list_of("hstm")("hstm"),TID_BOOL,&hstmContains));
// 
// // Support for multiple intervals in an hstm -- TODO how to handle multi-level ranges
// // REGISTER_FUNCTION(mergeRange,list_of("hstm")("hstm"),"hstm",hstmMergeRange);
//   _functionDescs.push_back(FunctionDescription("mergeRange",list_of("hstm")("hstm"),"hstm",&hstmMergeRange));
// 
//   // REGISTER_FUNCTION(addRange,list_of("hstm")("hstm"),"hstm",hstmAddRange);
//   _functionDescs.push_back(FunctionDescription("addRange",list_of("hstm")("hstm"),"hstm",&hstmAddRange));
// 
// // No remove-range yet.
// 
// // nranges: -> number of ranges in hstm // Just in case we have more than one interval or triangle
// // REGISTER_FUNCTION(nranges,list_of("hstm"),"int64",hstmNRanges);
//   _functionDescs.push_back(FunctionDescription("nranges",list_of("hstm"),TID_INT64,&hstmNRanges));
// 
// // Various output for different roles
// // Should we just do single triangles here?
// // REGISTER_FUNCTION(leftJustified,list_of("hstm"),"int64",hstmLeftJustifiedNoLevel); 
   _functionDescs.push_back(FunctionDescription("leftJustified",list_of("hstm"),TID_UINT64,&hstmLeftJustifiedNoLevel)); 
// 
// // Should we just do single triangles here?
// // REGISTER_FUNCTION(rightJustified,list_of("hstm"),"int64",hstmRightJustifiedNoLevel); 
   _functionDescs.push_back(FunctionDescription("rightJustified",list_of("hstm"),TID_UINT64,&hstmRightJustifiedNoLevel)); 
// 
// // Return intrinsic level of the hstm index--multiple valued if a set of indices?
// // REGISTER_FUNCTION(level,list_of("hstm"),"int64",hstmLevel); 
   _functionDescs.push_back(FunctionDescription("level",list_of("hstm"),TID_INT64,&hstmLevel)); 
   _functionDescs.push_back(FunctionDescription("idToLevel",list_of("hstm")(TID_INT64),TypeId("hstm"),&hstmIdToLevel)); 
// 
//   // REGISTER_FUNCTION(<,list_of("hstm")("hstm"),"bool",hstmLT); 
//   _functionDescs.push_back(FunctionDescription("<",list_of("hstm")("hstm"),TID_BOOL,&hstmLT)); 
// 
//   // REGISTER_FUNCTION(>,list_of("hstm")("hstm"),"bool",hstmGT);
//   _functionDescs.push_back(FunctionDescription(">",list_of("hstm")("hstm"),TID_BOOL,&hstmGT));
// 
//   // REGISTER_FUNCTION(=,list_of("hstm")("hstm"),"bool",hstmEQ);
   _functionDescs.push_back(FunctionDescription("=",list_of("hstm")("hstm"),TID_BOOL,&hstmEQ));
   _functionDescs.push_back(FunctionDescription("=",list_of("int64")("hstm"),TID_BOOL,&equal_Int64Hstm));
// 
//   // REGISTER_FUNCTION(<>,list_of("hstm")("hstm"),"bool",hstmNE);
//   _functionDescs.push_back(FunctionDescription("<>",list_of("hstm")("hstm"),TID_BOOL,&hstmNE));
// 
//   // Return the number of levels shared between two hstm indices -- DegreeOfEquality
//   _functionDescs.push_back(FunctionDescription("<>",list_of("hstm")("hstm"),TID_BOOL,&hstmEqualToLevel));

// Need a separate hstmRange?  Can we just have one hstm?

    _errors[HSTM_ERROR1] = "HSTM construction error.";
    scidb::ErrorsLibrary::getInstance()->registerErrors("hstm",&_errors);
  }

  ~HstmIndexLibrary() {
    scidb::ErrorsLibrary::getInstance()->unregisterErrors("hstm");
  }

  private:
  scidb::ErrorsLibrary::ErrorsMessages _errors;

} _instance;

/*

void test() {
  SpatialVector xHat = SpatialVector(1.0,0.0,0.0);
  std::cout << "test!" << std::endl << std::flush;
  std::cout << " xHat: " << xHat << std::endl << std::flush;
}

int main(int argc, char const *argv[]) {
  test();
  return 1;
}
*/

