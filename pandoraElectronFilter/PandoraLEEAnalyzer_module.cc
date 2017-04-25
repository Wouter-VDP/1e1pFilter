////////////////////////////////////////////////////////////////////////
// Class:       PandoraLEEAnalyzer
// Module Type: analyzer
// File:        PandoraLEEAnalyzer_module.cc
//
// Generated at Thu Jun 23 00:24:52 2016 by Lorena Escudero Sanchez using artmod
// from cetpkgsupport v1_10_02.
////////////////////////////////////////////////////////////////////////


#include <fstream>

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"


//uncomment the lines below as you use these objects

#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/MCBase/MCShower.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Utilities/InputTag.h"
#include "larcore/Geometry/Geometry.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "THStack.h"

#include "TEfficiency.h"
#include "art/Framework/Services/Optional/TFileService.h"

#include "ElectronEventSelectionAlg.h"

using namespace lar_pandora;


namespace lee {
  class PandoraLEEAnalyzer;
}

class lee::PandoraLEEAnalyzer : public art::EDAnalyzer {
public:
  explicit PandoraLEEAnalyzer(fhicl::ParameterSet const & pset);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.
  virtual ~PandoraLEEAnalyzer();

  // Plugins should not be copied or assigned.
  PandoraLEEAnalyzer(PandoraLEEAnalyzer const &) = delete;
  PandoraLEEAnalyzer(PandoraLEEAnalyzer &&) = delete;
  PandoraLEEAnalyzer & operator = (PandoraLEEAnalyzer const &) = delete;
  PandoraLEEAnalyzer & operator = (PandoraLEEAnalyzer &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;
  void reconfigure(fhicl::ParameterSet const &pset) override;

private:

  lee::ElectronEventSelectionAlg fElectronEventSelectionAlg;

  TFile * myTFile;
  TTree * myTTree;
  TEfficiency * e_energy;
  TH1F * h_nu_e;
  TH1F * h_nu_mu;
  TH1F * h_dirt;
  TH1F * h_cosmic;
  TH1F * h_nc;

  THStack * h_e_stacked;


  bool m_printDebug;
  double m_fidvolXstart;
  double m_fidvolXend;

  double m_fidvolYstart;
  double m_fidvolYend;

  double m_fidvolZstart;
  double m_fidvolZend;

  double m_trackLength;

  const int k_cosmic = 1;
  const int k_nu_e = 2;
  const int k_nu_mu = 3;
  const int k_nc = 4;
  const int k_dirt = 5;

  double _energy;
  double _category;
  double _track_dir_z;
  double _track_length;

  int _n_tracks;
  int _n_showers;

  double _vx;
  double _vy;
  double _vz;

  bool is_fiducial(double x[3]) const;
  double distance(double a[3], double b[3]);
  bool is_dirt(double x[3]) const;
  void measure_energy(size_t ipf, const art::Event & evt, double & energy);
  size_t choose_candidate(std::vector<size_t> & candidates, const art::Event & evt);
  void get_daughter_tracks( std::vector < size_t > pf_ids, const art::Event & evt, std::vector< art::Ptr<recob::Track> > &tracks);
  void get_daughter_showers( std::vector < size_t > pf_ids, const art::Event & evt, std::vector< art::Ptr<recob::Shower> > &showers);
  double trackEnergy(const art::Ptr<recob::Track>& track, const art::Event & evt);

  art::Ptr<recob::Track> get_longest_track(std::vector< art::Ptr<recob::Track> > &tracks);
  art::Ptr<recob::Shower> get_most_energetic_shower(std::vector< art::Ptr<recob::Shower> > &showers);

};


lee::PandoraLEEAnalyzer::PandoraLEEAnalyzer(fhicl::ParameterSet const & pset)
  :
  EDAnalyzer(pset)  // ,
 // More initializers here.
{

  //create output tree
  art::ServiceHandle<art::TFileService> tfs;
  myTFile = new TFile("PandoraLEEAnalyzerOutput.root", "RECREATE");
  myTTree = tfs->make<TTree>("pandoratree","PandoraAnalysis Tree");

  e_energy = tfs->make<TEfficiency>("e_energy",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_cosmic = tfs->make<TH1F>("h_cosmic",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_nc = tfs->make<TH1F>("h_nc",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_nu_e = tfs->make<TH1F>("h_nu_e",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_nu_mu = tfs->make<TH1F>("h_nu_mu",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_dirt = tfs->make<TH1F>("h_dirt",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);

  h_e_stacked = tfs->make<THStack>("h_e_stacked",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV");

  myTTree->Branch("category",  &_category, "category/i");
  myTTree->Branch("E",  &_energy, "E/d");
  myTTree->Branch("trk_dir_z",  &_track_dir_z, "trk_dir_z/d");
  myTTree->Branch("trk_len",  &_track_length, "trk_len/d");
  myTTree->Branch("n_tracks",  &_n_tracks, "n_tracks/i");
  myTTree->Branch("n_showers",  &_n_showers, "n_showers/i");
  myTTree->Branch("vx",  &_vx, "vx/d");
  myTTree->Branch("vy",  &_vy, "vy/d");
  myTTree->Branch("vz",  &_vz, "vz/d");

  this->reconfigure(pset);

}

lee::PandoraLEEAnalyzer::~PandoraLEEAnalyzer()
{

  //store output tree
  myTFile->cd();
  myTTree->Write("pandoratree");

  h_cosmic->SetLineColor(1);
  h_cosmic->SetLineWidth(2);
  h_cosmic->SetFillColor(kRed-3);
  h_cosmic->Write();

  h_nc->SetLineColor(1);
  h_nc->SetLineWidth(2);
  h_nc->SetFillColor(kBlue-9);
  h_nc->Write();

  h_nu_e->SetLineColor(1);
  h_nu_e->SetLineWidth(2);
  h_nu_e->SetFillColor(kGreen-2);
  h_nu_e->Write();

  h_nu_mu->SetLineColor(1);
  h_nu_mu->SetLineWidth(2);
  h_nu_mu->SetFillColor(kBlue-5);
  h_nu_mu->Write();

  h_dirt->SetLineColor(1);
  h_dirt->SetLineWidth(2);
  h_dirt->SetFillColor(kGray);
  h_dirt->Write();

  h_e_stacked->Add(h_cosmic);
  h_e_stacked->Add(h_nc);
  h_e_stacked->Add(h_nu_e);
  h_e_stacked->Add(h_nu_mu);
  h_e_stacked->Add(h_dirt);
  h_e_stacked->Write();

  myTFile->Close();


  std::cout << "End!" << std::endl;
}

double lee::PandoraLEEAnalyzer::distance(double a[3], double b[3]) {
  double d = 0;

  for (int i = 0; i < 3; i++) {
    d += pow((a[i]-b[i]),2);
  }

  return sqrt(d);
}

art::Ptr<recob::Shower> lee::PandoraLEEAnalyzer::get_most_energetic_shower(std::vector< art::Ptr<recob::Shower> > &showers) {
  art::Ptr<recob::Shower> most_energetic_shower;

  double max_energy = std::numeric_limits<double>::lowest();
  for (auto const& shower: showers) {
    if (shower->Energy()[shower->best_plane()] > max_energy) {
      most_energetic_shower = shower;
    }
  }
  return most_energetic_shower;
}

art::Ptr<recob::Track> lee::PandoraLEEAnalyzer::get_longest_track(std::vector< art::Ptr<recob::Track> > &tracks) {
  art::Ptr<recob::Track> longest_track;

  double max_length = 0;
  for (auto const& track: tracks) {
    if (track->Length() > max_length) {
      longest_track = track;
    }
  }
  return longest_track;
}

void lee::PandoraLEEAnalyzer::get_daughter_tracks( std::vector < size_t > pf_ids, const art::Event & evt, std::vector< art::Ptr<recob::Track> > &tracks) {
  art::InputTag pandoraNu_tag { "pandoraNu" };

  auto const& pfparticle_handle = evt.getValidHandle< std::vector< recob::PFParticle > >( pandoraNu_tag );
  auto const& pfparticles(*pfparticle_handle);

  art::FindOneP< recob::Track > track_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);

  for (auto const& pf_id: pf_ids) {
    auto const& track_obj = track_per_pfpart.at(pf_id);
    tracks.push_back(track_obj);
  }
}

void lee::PandoraLEEAnalyzer::get_daughter_showers(std::vector < size_t > pf_ids, const art::Event & evt, std::vector< art::Ptr<recob::Shower> > &showers) {
  art::InputTag pandoraNu_tag { "pandoraNu" };

  auto const& pfparticle_handle = evt.getValidHandle< std::vector< recob::PFParticle > >( pandoraNu_tag );
  auto const& pfparticles(*pfparticle_handle);

  art::FindOneP< recob::Shower > shower_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);

  for (auto const& pf_id: pf_ids) {
    auto const& shower_obj = shower_per_pfpart.at(pf_id);
    showers.push_back(shower_obj);
  }

}

double lee::PandoraLEEAnalyzer::trackEnergy(const art::Ptr<recob::Track>& track, const art::Event & evt)
{
    art::InputTag pandoraNu_tag { "pandoraNu" };
    auto const& track_handle = evt.getValidHandle< std::vector< recob::Track > >( pandoraNu_tag );
    art::FindManyP<anab::Calorimetry> calo_track_ass(track_handle, evt, "pandoraNucalo");
    const std::vector<art::Ptr<anab::Calorimetry>> calos = calo_track_ass.at(track->ID());
    double E = 0;
    double Eapprox =0;

    for (size_t ical = 0; ical<calos.size(); ++ical)
    {
        if (E!=0) continue;
        if (!calos[ical]) continue;
        if (!calos[ical]->PlaneID().isValid) continue;
        int planenum = calos[ical]->PlaneID().Plane;
        if (planenum<0||planenum>2) continue;
        if (planenum != 2) continue;                           // Use informartion from collection plane only

        // Understand if the calo module flipped the track
        //double dqdx_start = (calos[ical]->dQdx())[0] + (calos[ical]->dQdx())[1] + (calos[ical]->dQdx())[2];
        //double dqdx_end   = (calos[ical]->dQdx())[calos[ical]->dQdx().size()-1] + (calos[ical]->dQdx())[calos[ical]->dQdx().size()-2] + (calos[ical]->dQdx())[calos[ical]->dQdx().size()-3];
        //bool caloFlippedTrack = dqdx_start < dqdx_end;

        double mean=0;
        double dedx=0;
        double prevresrange=0;

        if(calos[ical]->ResidualRange()[0] > track->Length()/2)
        {
            prevresrange=track->Length();
        }

        double currentresrange=0;

        for(size_t iTrkHit = 0; iTrkHit < calos[ical]->dEdx().size(); ++iTrkHit)
        {
            dedx = calos[ical]->dEdx()[iTrkHit];
            currentresrange = calos[ical]->ResidualRange()[iTrkHit];
            if(dedx>0 && dedx<10)
            {
                //std::cout << dedx << "\t" << currentresrange << "\t"<< prevresrange<<std::endl;
                mean+=dedx;
                E+=dedx*abs(prevresrange-currentresrange);
                prevresrange=currentresrange;
            }
        }
        //std::cout << "Length: " << track->Length() << "and Energy approximation is " << mean/calos[ical]->dEdx().size()*track->Length()<< "MeV"<<std::endl;
        Eapprox = mean/calos[ical]->dEdx().size()*track->Length();
    }
    return Eapprox/1000; // convert to GeV
}


void lee::PandoraLEEAnalyzer::measure_energy(size_t ipf, const art::Event & evt, double & energy) {

  art::InputTag pandoraNu_tag { "pandoraNu" };

  auto const& pfparticle_handle = evt.getValidHandle< std::vector< recob::PFParticle > >( pandoraNu_tag );
  auto const& pfparticles(*pfparticle_handle);

  art::FindManyP<recob::Shower > showers_per_pfparticle ( pfparticle_handle, evt, pandoraNu_tag );
  std::vector<art::Ptr<recob::Shower>> showers = showers_per_pfparticle.at(ipf);

  for(size_t ish = 0; ish < showers.size(); ish++) {
    energy += showers[ish]->Energy()[showers[ish]->best_plane()];
  }

  art::FindManyP<recob::Track > tracks_per_pfparticle ( pfparticle_handle, evt, pandoraNu_tag );
  std::vector<art::Ptr<recob::Track>> tracks = tracks_per_pfparticle.at(ipf);

  for(size_t itr = 0; itr < tracks.size(); itr++) {
    energy += trackEnergy(tracks[itr], evt);
  }

  for (auto const& pfdaughter: pfparticles[ipf].Daughters()) {
    measure_energy(pfdaughter, evt, energy);
  }
}

size_t lee::PandoraLEEAnalyzer::choose_candidate(std::vector<size_t> & candidates, const art::Event & evt) {

  art::InputTag pandoraNu_tag { "pandoraNu" };

  auto const& pfparticle_handle = evt.getValidHandle< std::vector< recob::PFParticle > >( pandoraNu_tag );

  art::FindOneP< recob::Shower > shower_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);
  art::FindOneP< recob::Track > track_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);

  size_t chosen_candidate = 0;
  double most_z = -1;

  for (auto const& ic: candidates) {

    double longest_track_dir = -1;

    std::vector<art::Ptr<recob::Track>> nu_tracks;

    std::vector< size_t > pfp_tracks_id = fElectronEventSelectionAlg.get_pfp_id_showers_from_primary().at(ic);
    get_daughter_tracks(pfp_tracks_id, evt, nu_tracks);
    longest_track_dir = get_longest_track(nu_tracks)->StartDirection().Z();

    if (longest_track_dir > most_z) {
      chosen_candidate = ic;
      most_z = longest_track_dir;
    }

  }

  return chosen_candidate;

}

bool lee::PandoraLEEAnalyzer::is_fiducial(double x[3]) const
{
  art::ServiceHandle<geo::Geometry> geo;
  double bnd[6] = {0.,2.*geo->DetHalfWidth(),-geo->DetHalfHeight(),geo->DetHalfHeight(),0.,geo->DetLength()};

  bool is_x = x[0] > (bnd[0]+m_fidvolXstart) && x[0] < (bnd[1]-m_fidvolXend);
  bool is_y = x[1] > (bnd[2]+m_fidvolYstart) && x[1] < (bnd[3]-m_fidvolYend);
  bool is_z = x[2] > (bnd[4]+m_fidvolZstart) && x[2] < (bnd[5]-m_fidvolZend);
  return is_x && is_y && is_z;
}


bool lee::PandoraLEEAnalyzer::is_dirt(double x[3]) const
{
  art::ServiceHandle<geo::Geometry> geo;
  double bnd[6] = {0.,2.*geo->DetHalfWidth(),-geo->DetHalfHeight(),geo->DetHalfHeight(),0.,geo->DetLength()};

  bool is_x = x[0] > bnd[0] && x[0] < bnd[1];
  bool is_y = x[1] > bnd[2] && x[1] < bnd[3];
  bool is_z = x[2] > bnd[4] && x[2] < bnd[5];
  return !(is_x && is_y && is_z);
}

void lee::PandoraLEEAnalyzer::analyze(art::Event const & evt)
{
  std::vector<size_t> nu_candidates;

  bool event_passed = fElectronEventSelectionAlg.eventSelected(evt);
  if (event_passed){

    for (size_t inu = 0; inu < fElectronEventSelectionAlg.get_n_neutrino_candidates(); inu++){
      if (fElectronEventSelectionAlg.get_neutrino_candidate_passed().at(inu)){
        nu_candidates.push_back(inu);
      }
    }
  } else {
    return;
  }

  //do the analysis
  art::InputTag pandoraNu_tag { "pandoraNu" };
  art::InputTag generator_tag { "generator" };

  int _category = 0;

  auto const& generator_handle = evt.getValidHandle< std::vector< simb::MCTruth > >( generator_tag );
  auto const& generator(*generator_handle);
  int ccnc = -1;
  double true_neutrino_vertex[3];
  std::vector<simb::MCParticle> nu_mcparticles;

  if (generator.size() > 0) {
    ccnc = generator[0].GetNeutrino().CCNC();
    if (ccnc == 1) {
      _category = k_nc;
    }

    true_neutrino_vertex[0] = generator[0].GetNeutrino().Nu().Vx();
    true_neutrino_vertex[1] = generator[0].GetNeutrino().Nu().Vy();
    true_neutrino_vertex[2] = generator[0].GetNeutrino().Nu().Vz();

    if (is_dirt(true_neutrino_vertex)) {
      _category = k_dirt;
    }

    for (int i = 0; i < generator[0].NParticles(); i++) {
      if (generator[0].Origin() == 1) {
        nu_mcparticles.push_back(generator[0].GetParticle(i));
      }
    }
  } else {
    _category = k_cosmic;
  }

  int protons = 0;
  int electrons = 0;
  int muons = 0;

  for (auto& mcparticle: nu_mcparticles) {
    if (mcparticle.Process() == "primary" and mcparticle.T() != 0 and mcparticle.StatusCode() == 1) {

      switch(mcparticle.PdgCode())
      {
        case (abs(2212)):
        protons++;
        break;

        case (abs(11)):
        electrons++;
        break;

        case (abs(13)):
        muons++;
        break;
      }

    }
  }

  _energy = 0;

  try {
    auto const& pfparticle_handle = evt.getValidHandle< std::vector< recob::PFParticle > >( pandoraNu_tag );

    art::FindOneP< recob::Shower > shower_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);
    art::FindOneP< recob::Track > track_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);

    size_t ipf_candidate = choose_candidate(nu_candidates, evt);

    measure_energy(ipf_candidate, evt, _energy);

    art::FindOneP< recob::Vertex > vertex_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);
    auto const& vertex_obj = vertex_per_pfpart.at(ipf_candidate);

    double reco_neutrino_vertex[3];
    vertex_obj->XYZ(reco_neutrino_vertex);
    _vx = reco_neutrino_vertex[0];
    _vy = reco_neutrino_vertex[1];
    _vz = reco_neutrino_vertex[2];

    if (generator.size() > 0) {
      if (distance(reco_neutrino_vertex,true_neutrino_vertex) > 10) {
        _category = k_cosmic;
      }
    }

    std::vector<art::Ptr<recob::Track>> chosen_tracks;
    std::vector< size_t > pfp_tracks_id = fElectronEventSelectionAlg.get_pfp_id_showers_from_primary().at(ipf_candidate);

    get_daughter_tracks(pfp_tracks_id, evt, chosen_tracks);

    _track_dir_z = get_longest_track(chosen_tracks)->StartDirection().Z();
    _track_length = get_longest_track(chosen_tracks)->Length();
    _n_tracks = fElectronEventSelectionAlg.get_n_tracks().at(ipf_candidate);
    _n_showers = fElectronEventSelectionAlg.get_n_showers().at(ipf_candidate);

    std::cout << "Chosen neutrino " << ipf_candidate << std::endl;

  } catch (...) {
    std::cout << "NO RECO DATA PRODUCTS" << std::endl;
  }

  if (_category != k_cosmic && _category != k_dirt && _category != k_nc) {
    if (protons != 0 && electrons != 0) {
      _category = k_nu_e;
    } else if (protons != 0 && muons != 0) {
      _category = k_nu_mu;
    }
  }

  myTTree->Fill();

} // end analyze function

//------------------------------------------------------------------------------------------------------------------------------------


void lee::PandoraLEEAnalyzer::reconfigure(fhicl::ParameterSet const & pset)
{

  //TODO: add an external fcl file to change configuration
  //add what you want to read, and default values of your labels etc. example:
  //  m_particleLabel = pset.get<std::string>("PFParticleModule","pandoraNu");
  fElectronEventSelectionAlg.reconfigure(pset.get<fhicl::ParameterSet>("ElectronSelectionAlg"));

  m_printDebug = pset.get<bool>("PrintDebug",false);
  m_trackLength = pset.get<int>("trackLength",100);

  m_fidvolXstart = pset.get<double>("fidvolXstart",10);
  m_fidvolXend = pset.get<double>("fidvolXstart",10);

  m_fidvolYstart = pset.get<double>("fidvolYstart",20);
  m_fidvolYend = pset.get<double>("fidvolYend",20);

  m_fidvolZstart = pset.get<double>("fidvolZstart",10);
  m_fidvolZend = pset.get<double>("fidvolZend",50);
}

//---------------------------------------------------------------------------------------------------------------------------
//add other functions here

DEFINE_ART_MODULE(lee::PandoraLEEAnalyzer)