#include "UHH2/Plot94Jets/include/Plot94JetsHists.h"
#include "UHH2/core/include/Event.h"

#include "TH1F.h"
#include <iostream>

using namespace std;
using namespace uhh2;
using namespace uhh2examples;

Plot94JetsHists::Plot94JetsHists(Context & ctx, const string & dirname): Hists(ctx, dirname){
  // book all histograms here
  // primary vertices
  book<TH1F>("N_pv", "N^{PV}", 50, 0, 50);


  // jets
  book<TH1F>("N_jets", "N_{jets}", 20, 0, 20);  

  book<TH1F>("eta_jet", "#eta^{jet}", 80, -5., 5);

  book<TH1F>("eta_jet1", "#eta^{jet 1}", 80, -5., 5.);
  book<TH1F>("eta_jet2", "#eta^{jet 2}", 80, -5., 5.);

  book<TH1F>("pt_jet", "p_{T}^{jet} [GeV/c]", 100, 0., 500.);

  book<TH1F>("pt_jet1", "p_{T}^{jet 1} [GeV/c]", 100, 0., 500.);
  book<TH1F>("pt_jet2", "p_{T}^{jet 2} [GeV/c]", 100, 0., 500.);

  book<TH1F>("eta_jet_forward", "#eta^{jet}", 80, -5., 5);
  book<TH1F>("pt_jet_forward", "p_{T}^{jet} [GeV/c]", 100, 0., 500.);

  book<TH1F>("eta_jet_barrel", "#eta^{jet}", 80, -5., 5);
  book<TH1F>("pt_jet_barrel", "p_{T}^{jet} [GeV/c]", 100, 0., 500.);


  // Ak8 jets

  book<TH1F>("N_AK8jets", "N_{AK8jets}", 20, 0, 20);  

  book<TH1F>("eta_AK8jet", "#eta^{AK8jet}", 80, -5., 5);

  book<TH1F>("eta_AK8jet1", "#eta^{AK8jet 1}", 80, -5., 5.);
  book<TH1F>("eta_AK8jet2", "#eta^{AK8jet 2}", 80, -5., 5.);

  book<TH1F>("pt_AK8jet", "p_{T}^{AK8jet} [GeV/c]", 100, 0., 500.);

  book<TH1F>("pt_AK8jet1", "p_{T}^{AK8jet 1} [GeV/c]", 100, 0., 500.);
  book<TH1F>("pt_AK8jet2", "p_{T}^{AK8jet 2} [GeV/c]", 100, 0., 500.);

  if(ctx.get("algo") == "PUPPI")
    handleAK8Jets = ctx.get_handle<vector<Jet>>("patJetsAK8PFPUPPI");
  else if(ctx.get("algo") == "CHS")
    handleAK8Jets = ctx.get_handle<vector<Jet>>("patJetsAK8PFCHS");


}


void Plot94JetsHists::fill(const Event & event){
  // fill the histograms. Please note the comments in the header file:
  // 'hist' is used here a lot for simplicity, but it will be rather
  // slow when you have many histograms; therefore, better
  // use histogram pointers as members as in 'UHH2/common/include/ElectronHists.h'
  
  // Don't forget to always use the weight when filling.
  double weight = event.weight;
  int Npvs = event.pvs->size();
  hist("N_pv")->Fill(Npvs, weight);
  

  std::vector<Jet>* jets = event.jets;
  int Njets = jets->size();
  hist("N_jets")->Fill(Njets, weight);
  
  for(int i = 0; i<Njets; i++)
    {
      hist("eta_jet")->Fill(jets->at(i).eta(), weight);
      hist("pt_jet")->Fill(jets->at(i).pt(), weight);
      if(fabs(jets->at(i).eta()) < 3.)
	{
	  hist("eta_jet_barrel")->Fill(jets->at(i).eta(), weight);
	  hist("pt_jet_barrel")->Fill(jets->at(i).pt(), weight);
	}
      else 
	{
	  hist("eta_jet_forward")->Fill(jets->at(i).eta(), weight);
	  hist("pt_jet_forward")->Fill(jets->at(i).pt(), weight);
	}
    }

  if(Njets>=1){
    hist("eta_jet1")->Fill(jets->at(0).eta(), weight);
    hist("pt_jet1")->Fill(jets->at(0).pt(), weight);
  }
  if(Njets>=2){
    hist("eta_jet2")->Fill(jets->at(1).eta(), weight);
    hist("pt_jet2")->Fill(jets->at(1).pt(), weight);
  }


  vector<Jet> AK8Jets = event.get(handleAK8Jets);
  int NAK8jets = AK8Jets.size();
  hist("N_AK8jets")->Fill(NAK8jets, weight);

  for(int i = 0; i<NAK8jets; i++)
    {
      hist("eta_AK8jet")->Fill(AK8Jets.at(i).eta(), weight);
      hist("pt_AK8jet")->Fill(AK8Jets.at(i).pt(), weight);
    }

  if(NAK8jets>=1){
    hist("eta_AK8jet1")->Fill(AK8Jets.at(0).eta(), weight);
    hist("pt_AK8jet1")->Fill(AK8Jets.at(0).pt(), weight);
  }
  if(NAK8jets>=2){
    hist("eta_AK8jet2")->Fill(AK8Jets.at(1).eta(), weight);
    hist("pt_AK8jet2")->Fill(AK8Jets.at(1).pt(), weight);
  }

  
}

Plot94JetsHists::~Plot94JetsHists(){}
