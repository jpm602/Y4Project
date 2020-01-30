#include <vector>
#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TString.h>
#include <iostream>
#include <string>

#include "TTreeReader.h"
#include "TFile.h"
#include "TTree.h"

class GetPosition
{
public:
  GetPosition(int gap, int start, int nfiles);
  ~GetPosition();

  double GetConverterPositions(int gap, int start, int nfiles);

private:
  int m_gap;
  int m_start;
  int m_nfiles;
  double mean;
};
