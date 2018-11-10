#include "qif"

namespace qif {
namespace wrapper {


// OSQP methods /////////////////////////////////////////////////////////////////

using namespace osqp;

void osqp_set_default_settings(OSQPSettings *settings) {
	osqp::osqp_set_default_settings(settings);
}

OSQPWorkspace* osqp_setup(const OSQPData *data, OSQPSettings *settings) {
	return osqp::osqp_setup(data, settings);
}

c_int osqp_solve(OSQPWorkspace *work) {
	return osqp::osqp_solve(work);
}

c_int osqp_cleanup(OSQPWorkspace *work) {
	return osqp::osqp_cleanup(work);
}

csc* csc_matrix(c_int m, c_int n, c_int nzmax, c_float *x, c_int *i, c_int *p) {
	return osqp::csc_matrix(m, n, nzmax, x, i, p);
}


}}
