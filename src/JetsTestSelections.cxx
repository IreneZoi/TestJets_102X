#include "UHH2/JetsTest/include/JetsTestSelections.h"
#include "UHH2/core/include/Event.h"

#include <stdexcept>

using namespace uhh2examples;
using namespace uhh2;


DijetSelection::DijetSelection(float dphi_min_, float third_frac_max_): dphi_min(dphi_min_), third_frac_max(third_frac_max_){}

bool DijetSelection::passes(const Event & event){
    assert(event.jets); // if this fails, it probably means jets are not read in
    if(event.jets->size() < 2) return false;
    const auto & jet0 = event.jets->at(0);
    const auto & jet1 = event.jets->at(1);
    auto dphi = deltaPhi(jet0, jet1);
    if(dphi < dphi_min) return false;
    if(event.jets->size() == 2) return true;
    const auto & jet2 = event.jets->at(2);
    auto third_jet_frac = jet2.pt() / (0.5 * (jet0.pt() + jet1.pt()));
    return third_jet_frac < third_frac_max;
}

VBFdeltaEtajetSelection::VBFdeltaEtajetSelection(float deta_min_): deta_min(deta_min_){}

bool VBFdeltaEtajetSelection::passes(const Event & event){
  assert(event.jets); // if this fails, it probably means jets are not read in                                                                                                                                                                                                
  if(event.jets->size() < 2) return false;
  auto deltaeta = event.jets->at(0).eta()-event.jets->at(1).eta();
  if( fabs(deltaeta) < deta_min) return false;
  else return true;

}



invMassVBFjetSelection::invMassVBFjetSelection(float invM_min_): invM_min(invM_min_){}

bool invMassVBFjetSelection::passes(const Event & event){
  assert(event.jets); // if this fails, it probably means jets are not read in                                                                                                                                                                                                  
  if(event.jets->size() < 2) return false;

  auto invariantMass = (event.jets->at(0).v4() + event.jets->at(1).v4()).M();
  if( invariantMass < invM_min) return false;
  else return true;


}

