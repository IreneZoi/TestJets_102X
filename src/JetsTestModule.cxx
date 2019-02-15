#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/JetIds.h"
#include <UHH2/common/include/MCWeight.h>
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/JetsTest/include/JetsTestSelections.h"
#include "UHH2/JetsTest/include/JetsTestHists.h"
#include "UHH2/common/include/PrintingModules.h"

using namespace std;
using namespace uhh2;

namespace uhh2examples {

/** \brief Basic analysis example of an AnalysisModule (formerly 'cycle') in UHH2
 * 
 * This is the central class which calls other AnalysisModules, Hists or Selection classes.
 * This AnalysisModule, in turn, is called (via AnalysisModuleRunner) by SFrame.
 */
class JetsTestModule: public AnalysisModule {
public:
    
    explicit JetsTestModule(Context & ctx);
    virtual bool process(Event & event) override;

private:
    
    std::unique_ptr<CommonModules> common;

    std::unique_ptr<JetCorrector> jet_corrector;

    std::unique_ptr<JetCorrector> jet_corrector_B;
    std::unique_ptr<JetCorrector> jet_corrector_C;
    std::unique_ptr<JetCorrector> jet_corrector_DE;
    std::unique_ptr<JetCorrector> jet_corrector_F;

    std::unique_ptr<GenericJetCorrector> AK8jet_corrector;
  
    std::unique_ptr<GenericJetCorrector> AK8jet_corrector_B;
    std::unique_ptr<GenericJetCorrector> AK8jet_corrector_C;
    std::unique_ptr<GenericJetCorrector> AK8jet_corrector_DE;
    std::unique_ptr<GenericJetCorrector> AK8jet_corrector_F;

    std::unique_ptr<JetCleaner> jetcleaner;
    std::unique_ptr<JetCleaner> ak4pfidfilter;
    std::unique_ptr<JetCleaner> AK8jetcleaner;

    std::unique_ptr<AnalysisModule> AK4jet_printer;

    Event::Handle<vector<Jet>> handleAK8Jets;

    // declare the Selections to use. Use unique_ptr to ensure automatic call of delete in the destructor,
    // to avoid memory leaks.
    std::unique_ptr<Selection> njet_sel, deltaEtaAK4_sel, invMAK4_sel;
    
    // store the Hists collection as member variables. Again, use unique_ptr to avoid memory leaks.
  std::unique_ptr<Hists> h_nocuts, h_common, h_jec, h_jetID,h_jetcleaner, h_AK8jetcleaner,h_2AK8, h_noOverlap, h_AK8invMass, h_AK8deta,h_AK4deta,h_AK4invM,h_2jet;
     
    bool isMC;
    bool PRINT = true;
    
    const int runnr_B = 299329;
    const int runnr_C = 302029;
    const int runnr_D = 303434;
    const int runnr_E = 304826;
    const int runnr_F = 306462;

    JetId AK4PFID;
    




};


JetsTestModule::JetsTestModule(Context & ctx){
    // In the constructor, the typical tasks are to initialize the
    // member variables, in particular the AnalysisModules such as
    // CommonModules or some cleaner module, Selections and Hists.
    // But you can do more and e.g. access the configuration, as shown below.
    
    cout << "Hello World from JetsTestModule!" << endl;
    
    // If needed, access the configuration of the module here, e.g.:
    string testvalue = ctx.get("TestKey", "<not set>");
    cout << "TestKey in the configuration was: " << testvalue << endl;
    
    // If running in SFrame, the keys "dataset_version", "dataset_type", "dataset_lumi",
    // and "target_lumi" are set to the according values in the xml file. For CMSSW, these are
    // not set automatically, but can be set in the python config file.
    for(auto & kv : ctx.get_all()){
        cout << " " << kv.first << " = " << kv.second << endl;
    }
    
    isMC = (ctx.get("dataset_type") == "MC");

    if(ctx.get("algo") == "PUPPI")
      handleAK8Jets = ctx.get_handle<vector<Jet>>("patJetsAK8PFPUPPI");
    else if(ctx.get("algo") == "CHS")
      handleAK8Jets = ctx.get_handle<vector<Jet>>("patJetsAK8PFCHS");
    

// 1. setup other modules. CommonModules and the JetCleaner:
    common.reset(new CommonModules());
    // TODO: configure common here, e.g. by 
    // calling common->set_*_id or common->disable_*
    common->switch_jetlepcleaner(false);
    common->disable_jetpfidfilter();
    common->disable_jersmear(); //irene                                                                                                                                                                                                                                       
    common->disable_jec(); //irene                                                                                                                                                                                                                                            

    common->init(ctx);

    // Jet correctors                                                                                                                                                                                                                                                      
    std::vector<std::string> JEC_AK4, JEC_AK8,JEC_AK4_B,JEC_AK4_C,JEC_AK4_DE,JEC_AK4_F,JEC_AK8_B,JEC_AK8_C,JEC_AK8_DE,JEC_AK8_F;

    if(isMC)
      {
	if(ctx.get("algo") == "PUPPI") 
	  {
	    JEC_AK4     = JERFiles::Fall17_17Nov2017_V32_L123_AK4PFPuppi_MC ;
	    JEC_AK8     = JERFiles::Fall17_17Nov2017_V32_L123_AK8PFPuppi_MC ;
	  }
	if(ctx.get("algo") == "CHS") 
	  {
	    JEC_AK4     = JERFiles::Fall17_17Nov2017_V32_L123_AK4PFchs_MC ;
	    JEC_AK8     = JERFiles::Fall17_17Nov2017_V32_L123_AK8PFchs_MC ;
	  }
      }
    else
      {
        if(ctx.get("algo") == "PUPPI")
          {
	    JEC_AK4_B = JERFiles::Fall17_17Nov2017_V32_B_L123_AK4PFPuppi_DATA;
	    JEC_AK4_C = JERFiles::Fall17_17Nov2017_V32_C_L123_AK4PFPuppi_DATA;
	    JEC_AK4_DE = JERFiles::Fall17_17Nov2017_V32_DE_L123_AK4PFPuppi_DATA;
	    JEC_AK4_F = JERFiles::Fall17_17Nov2017_V32_F_L123_AK4PFPuppi_DATA;

	    JEC_AK8_B = JERFiles::Fall17_17Nov2017_V32_B_L123_AK8PFPuppi_DATA;
	    JEC_AK8_C = JERFiles::Fall17_17Nov2017_V32_C_L123_AK8PFPuppi_DATA;
	    JEC_AK8_DE = JERFiles::Fall17_17Nov2017_V32_DE_L123_AK8PFPuppi_DATA;
	    JEC_AK8_F = JERFiles::Fall17_17Nov2017_V32_F_L123_AK8PFPuppi_DATA;
	  }
        if(ctx.get("algo") == "CHS")
          {
	    JEC_AK4_B = JERFiles::Fall17_17Nov2017_V32_B_L123_AK4PFchs_DATA;
	    JEC_AK4_C = JERFiles::Fall17_17Nov2017_V32_C_L123_AK4PFchs_DATA;
	    JEC_AK4_DE = JERFiles::Fall17_17Nov2017_V32_DE_L123_AK4PFchs_DATA;
	    JEC_AK4_F = JERFiles::Fall17_17Nov2017_V32_F_L123_AK4PFchs_DATA;

	    JEC_AK8_B = JERFiles::Fall17_17Nov2017_V32_B_L123_AK8PFchs_DATA;
	    JEC_AK8_C = JERFiles::Fall17_17Nov2017_V32_C_L123_AK8PFchs_DATA;
	    JEC_AK8_DE = JERFiles::Fall17_17Nov2017_V32_DE_L123_AK8PFchs_DATA;
	    JEC_AK8_F = JERFiles::Fall17_17Nov2017_V32_F_L123_AK8PFchs_DATA;
	  }
      }
    
    jet_corrector.reset(new JetCorrector(ctx, JEC_AK4));

    if(ctx.get("algo") == "PUPPI")    AK8jet_corrector.reset(new GenericJetCorrector(ctx, JEC_AK8,"patJetsAK8PFPUPPI"));
    else if(ctx.get("algo") == "CHS") AK8jet_corrector.reset(new GenericJetCorrector(ctx, JEC_AK8,"patJetsAK8PFCHS"));

    jet_corrector_B.reset(new JetCorrector(ctx, JEC_AK4_B));
    jet_corrector_C.reset(new JetCorrector(ctx, JEC_AK4_C));
    jet_corrector_DE.reset(new JetCorrector(ctx, JEC_AK4_DE));
    jet_corrector_F.reset(new JetCorrector(ctx, JEC_AK4_F));


    if(ctx.get("algo") == "PUPPI")
      {
	AK8jet_corrector_B.reset(new GenericJetCorrector(ctx, JEC_AK8_B,"patJetsAK8PFPUPPI"));
	AK8jet_corrector_C.reset(new GenericJetCorrector(ctx, JEC_AK8_C,"patJetsAK8PFPUPPI"));
	AK8jet_corrector_DE.reset(new GenericJetCorrector(ctx, JEC_AK8_DE,"patJetsAK8PFPUPPI"));
	AK8jet_corrector_F.reset(new GenericJetCorrector(ctx, JEC_AK8_F,"patJetsAK8PFPUPPI"));
      }
    else if(ctx.get("algo") == "CHS")
      { 
        AK8jet_corrector_B.reset(new GenericJetCorrector(ctx, JEC_AK8_B,"patJetsAK8PFCHS"));
        AK8jet_corrector_C.reset(new GenericJetCorrector(ctx, JEC_AK8_C,"patJetsAK8PFCHS"));
        AK8jet_corrector_DE.reset(new GenericJetCorrector(ctx, JEC_AK8_DE,"patJetsAK8PFCHS"));
        AK8jet_corrector_F.reset(new GenericJetCorrector(ctx, JEC_AK8_F,"patJetsAK8PFCHS"));
      } 

    jetcleaner.reset(new JetCleaner(ctx, 30.0, 5.)); 
    if(ctx.get("algo") == "PUPPI")    AK8jetcleaner.reset(new JetCleaner(ctx, 200.0, 2.5, "patJetsAK8PFPUPPI")); 
    else if(ctx.get("algo") == "CHS")    AK8jetcleaner.reset(new JetCleaner(ctx, 200.0, 2.5, "patJetsAK8PFCHS"));

    //if(ctx.get("algo") == "PUPPI")
    //AK4PFID=JetPFID(JetPFID::WP_TIGHT_PUPPI);
    //if(ctx.get("algo") == "CHS")
      AK4PFID=JetPFID(JetPFID::WP_TIGHT);
    
    ak4pfidfilter.reset(new JetCleaner(ctx,AK4PFID));
    
    // note that the JetCleaner is only kept for the sake of example;
    // instead of constructing a jetcleaner explicitly,
    // the cleaning can also be achieved with less code via CommonModules with:
    // common->set_jet_id(PtEtaCut(30.0, 2.4));
    // before the 'common->init(ctx)' line.
    
    AK4jet_printer.reset(new JetPrinter("AK4 jets",30.));


    // 2. set up selections
    njet_sel.reset(new NJetSelection(2)); // see common/include/NSelections.h
    deltaEtaAK4_sel.reset(new VBFdeltaEtajetSelection()); 
    invMAK4_sel.reset(new invMassVBFjetSelection());


    // 3. Set up Hists classes:
    h_nocuts.reset(new JetsTestHists(ctx, "NoCuts"));
    h_common.reset(new JetsTestHists(ctx, "common"));
    h_jec.reset(new JetsTestHists(ctx, "jec"));
    h_jetcleaner.reset(new JetsTestHists(ctx, "jetcleaner"));
    h_jetID.reset(new JetsTestHists(ctx, "jetID"));
    h_AK8jetcleaner.reset(new JetsTestHists(ctx, "AK8jetcleaner"));
    h_2AK8.reset(new JetsTestHists(ctx, "2AK8"));
    h_noOverlap.reset(new JetsTestHists(ctx, "noOverlap"));
    h_AK8invMass.reset(new JetsTestHists(ctx, "AK8invMass"));
    h_AK8deta.reset(new JetsTestHists(ctx, "AK8deta"));
    h_AK4deta.reset(new JetsTestHists(ctx, "AK4deta"));
    h_AK4invM.reset(new JetsTestHists(ctx, "AK4invM"));
    h_2jet.reset(new JetsTestHists(ctx, "2jets"));


}


bool JetsTestModule::process(Event & event) {
    // This is the main procedure, called for each event. Typically,
    // do some pre-processing by calling the modules' process method
    // of the modules constructed in the constructor (1).
    // Then, test whether the event passes some selection and -- if yes --
    // use it to fill the histograms (2).
    // Finally, decide whether or not to keep the event in the output (3);
    // this is controlled by the return value of this method: If it
    // returns true, the event is kept; if it returns false, the event
    // is thrown away.
    
  if(PRINT)  cout << "JetsTestModule: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << "); weight = " << event.weight << endl;
    
    // 1. run all modules other modules.
    h_nocuts->fill(event);
    if(PRINT)  cout << " input " << endl;

    bool pass_cm = common->process(event);
    if(!pass_cm) return false;
    h_common->fill(event);
    if(PRINT)  cout << " common " << endl;


    if(isMC)
      {
	jet_corrector->process(event);
	AK8jet_corrector->process(event);
	jet_corrector->correct_met(event);
	if(PRINT)  cout << " MC JEC " << endl;

      }
    else
      {
	if(event.run < runnr_C)  {
	  jet_corrector_B->process(event);
	  AK8jet_corrector_B->process(event);
	  jet_corrector_B->correct_met(event);
	}
	else if(event.run < runnr_D && event.run >= runnr_C){
	  jet_corrector_C->process(event);
	  AK8jet_corrector_C->process(event);
	  jet_corrector_C->correct_met(event);
	}
	else if(event.run >= runnr_D && event.run < runnr_F){
	  jet_corrector_DE->process(event);
	  AK8jet_corrector_DE->process(event);
	  jet_corrector_DE->correct_met(event);
	}
	else if(event.run >= runnr_F){
	  jet_corrector_F->process(event);
	  AK8jet_corrector_F->process(event);
	  jet_corrector_F->correct_met(event);
	}
	    if(PRINT)  cout << " DATA JEC " << endl;
      }


	h_jec->fill(event);
	if(PRINT)  cout << " JEC " << endl;


    jetcleaner->process(event);
    sort_by_pt<Jet>(*event.jets);
    h_jetcleaner->fill(event);
    if(PRINT)  cout << " jet cleaner " << endl;
    ak4pfidfilter->process(event);
    h_jetID->fill(event);
    if(PRINT)  cout << " jet ID " << endl;

    AK8jetcleaner->process(event);
    vector<Jet> AK8Jets = event.get(handleAK8Jets);
    sort_by_pt<Jet>(AK8Jets);
    h_AK8jetcleaner->fill(event);
    if(PRINT)  cout << "  AK8 jet cleaner " << endl;

    // 2. test selections and fill histograms

    
    if(PRINT)    cout << " NUMBER OF AK8 " << AK8Jets.size() << endl;
    if(AK8Jets.size() < 2.) return false;
    if(PRINT)    cout << " filling "  << endl;

    h_2AK8->fill(event);

    if(PRINT)
      {
	cout << " before AK4 celaning " << endl;
	AK4jet_printer->process(event);
	cout << " event.weight " << event.weight << endl;

      }
    //Cleaning(removing) AK4 if overlapping with AK8                                                                                                                                                                                                                                
    std::vector<Jet>* AK4Jets(new std::vector<Jet> (*event.jets));

    const Jet & tj_0 = AK8Jets[0];
    const Jet & tj_1 = AK8Jets[1];

    AK4Jets->clear();
    AK4Jets->reserve(event.jets->size());


    for(const Jet ak4:*event.jets)
      {
	bool bdeltaR=true;
	double deltar_0 = deltaR(ak4,tj_0);
	double deltar_1 = deltaR(ak4,tj_1);
	if((deltar_0 < 1.2) || (deltar_1 < 1.2)) bdeltaR=false;
	if(bdeltaR) AK4Jets->push_back(ak4);
      }

    sort_by_pt<Jet>(*AK4Jets);
    ////put cleaned AK4 jets in event.jet                                                                                                                                                                                                                                           
    event.jets->clear();
    event.jets->reserve(AK4Jets->size());
    for(const auto & j : *AK4Jets) event.jets->push_back(j);
    sort_by_pt<Jet>(*event.jets);
    sort_by_pt<Jet>(AK8Jets);

    /////////////////AK4 cleaning end ////////////////                                                                                                                                                                                                                              
    h_noOverlap->fill(event);

    if(PRINT && event.weight > 200)
      {
	cout << " after AK4 celaning " << endl;
	AK4jet_printer->process(event);
	cout << " event.weight " << event.weight << endl;
      }


    auto invariantMass = (AK8Jets[0].v4() + AK8Jets[1].v4()).M();
    if( invariantMass < 1050. ) return false;
    h_AK8invMass->fill(event);

    auto deltaeta = AK8Jets[0].eta()-AK8Jets[1].eta();                                                                                                                                                                                                   
    if( fabs(deltaeta) > 1.3) return false;                                                                                                                                                                                                                             
    h_AK8deta->fill(event);

    bool AK4deta_selection = deltaEtaAK4_sel->passes(event);
    if(!AK4deta_selection) return false;
    h_AK4deta->fill(event);

    bool AK4invM_selection = invMAK4_sel->passes(event);
    if(!AK4invM_selection) return false;
    h_AK4invM->fill(event);


    bool njet_selection = njet_sel->passes(event);
    if(njet_selection){
        h_2jet->fill(event);
    }

    // 3. decide whether or not to keep the current event in the output:
    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the JetsTestModule is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(JetsTestModule)

}
