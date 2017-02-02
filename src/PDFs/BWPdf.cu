#include "goofit/PDFs/BWPdf.h"

EXEC_TARGET fptype device_BW(fptype* evt, fptype* p, unsigned int* indices) {
    fptype x = evt[indices[2 + indices[0]]];
    fptype mean  = p[indices[1]];
    fptype gamma = p[indices[2]];
    fptype rootPi = -2.*ATAN2(-1.0, 0.0);
    fptype ret = (gamma/((x-mean)*(x-mean) + gamma*gamma/4))/(2*rootPi);
    return ret;
}

MEM_DEVICE device_function_ptr ptr_to_BW = device_BW;

__host__ BWPdf::BWPdf(std::string n, Variable* _x, Variable* mean, Variable* width)
    : GooPdf(_x, n) {
    std::vector<unsigned int> pindices;
    pindices.push_back(registerParameter(mean));
    pindices.push_back(registerParameter(width));
    GET_FUNCTION_ADDR(ptr_to_BW);
    initialise(pindices);
}
