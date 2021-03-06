/*
 *  See header file for a description of this class.
 *
 *  \author N. Amapane, G. Cerminara
 */

#include "CondFormats/DTObjects/interface/DTRecoConditions.h"
#include "DataFormats/MuonDetId/interface/DTWireId.h"
#include "FWCore/Utilities/interface/Exception.h"
#include <iostream>
#include <algorithm>

#include <TFormula.h>

using std::string;
using std::map;
using std::vector;
using std::cout;
using std::endl;


DTRecoConditions::DTRecoConditions() : 
  formula(0), 
  formulaType(0),  
  expression("[0]")
{}

DTRecoConditions::~DTRecoConditions(){
  delete formula;
}


float DTRecoConditions::get(const DTWireId& wireid, double* x) const {

  map<uint32_t, vector<double> >::const_iterator slIt = payload.find(wireid.superlayerId().rawId());
  if(slIt == payload.end()) {
    throw cms::Exception("InvalidInput") << "The SLId: " << wireid.superlayerId() << " is not in the paylaod map";
  }
  const vector<double>& par = slIt->second;

  // Initialize if this is the first call
  if (formulaType==0) {
    if (expression=="[0]") {
      formulaType = 1;
    } else if  (expression=="par[step]") {
      formulaType = 2;
    } else { 
      formulaType = 99;
      formula  = new TFormula("DTExpr",expression.c_str());
    }
  }
  
  if (formulaType==1 || par.size()==1) {
    // Return value is simply a constant. Assume this is the case also if only one parameter exists.
    return par[0];
  } else if (formulaType==2) {            
    // Special case: par[i] represents the value for step i
    return par[unsigned (x[0])];
  } else {
    // Use the formula corresponding to expression. 
    return formula->EvalPar(x,par.data());
  }
}


void DTRecoConditions::set(const DTWireId& wireid, const std::vector<double>& values) {
  payload[wireid.superlayerId()] = values;
}


DTRecoConditions::const_iterator DTRecoConditions::begin() const {
  return payload.begin();
}

DTRecoConditions::const_iterator DTRecoConditions::end() const {
  return payload.end();
}
