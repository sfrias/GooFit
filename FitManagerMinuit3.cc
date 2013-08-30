PdfBase* pdfPointer; 
int numPars = 0; 
#ifdef OMP_ON
#pragma omp threadprivate (pdfPointer)
#pragma omp threadprivate (numPars)
std::vector<Variable*> vars[MAX_THREADS]; 
#else
std::vector<Variable*> vars; 
#endif

FitManager::FitManager (PdfBase* dat) {
  pdfPointer = dat;
} 

void FitManager::fit () {
  host_callnumber = 0; 
  pdfPointer->getParameters(vars); 

  numPars = vars.size();
  fitter = TVirtualFitter::Fitter(0, numPars);

  int maxIndex = 0; 
  int counter = 0; 
  for (std::vector<Variable*>::iterator i = vars.begin(); i != vars.end(); ++i) {
    fitter->SetParameter(counter, (*i)->name.c_str(), (*i)->value, (*i)->error, (*i)->lowerlimit, (*i)->upperlimit);
    if ((*i)->fixed) fitter->FixParameter(counter); 
    counter++; 
    if (maxIndex < (*i)->getIndex()) maxIndex = (*i)->getIndex();
  }

  numPars = maxIndex+1; 
  pdfPointer->copyParams();   

  fitter->SetFCN(FitFun); 
  fitter->ExecuteCommand("MIGRAD", 0, 0); 
}

void FitManager::getMinuitValues () const {
  int counter = 0; 
#ifdef OMP_ON
  int tid = omp_get_thread_num();
  for (std::vector<Variable*>::iterator i = vars[tid].begin(); i != vars[tid].end(); ++i) {
    (*i)->value = fitter->GetParameter(counter);
    (*i)->error = fitter->GetParError(counter);
    counter++;
  }
#else
  for (std::vector<Variable*>::iterator i = vars.begin(); i != vars.end(); ++i) {
    (*i)->value = fitter->GetParameter(counter);
    (*i)->error = fitter->GetParError(counter);
    counter++;
  }
#endif
}

void FitFun (int &npar, double *gin, double &fun, double *fp, int iflag) { // MINUIT 3 version 
  vector<double> pars; // Translates from Minuit to GooFit indices
  pars.resize(numPars); 
  int counter = 0; 
  for (std::vector<Variable*>::iterator i = vars.begin(); i != vars.end(); ++i) {
    pars[(*i)->getIndex()] = fp[counter++]; 
  }

  pdfPointer->copyParams(pars); 
  fun = pdfPointer->calculateNLL();
  host_callnumber++; 
}

