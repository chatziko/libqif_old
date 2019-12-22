#include <qif>
#include "pybind11_aux.h"

namespace py = pybind11;
using namespace py::literals;
using namespace qif;


void init_mechanism_bayes_vuln_module(py::module);
void init_mechanism_bayes_risk_module(py::module);
void init_mechanism_g_vuln_module(py::module);
void init_mechanism_l_risk_module(py::module);
void init_mechanism_shannon_module(py::module);
void init_mechanism_geo_module(py::module);
void init_mechanism_d_privacy_module(py::module);


void init_mechanism_module(py::module m) {

	m.doc() = R"pbdoc(
		Mechanism construction.

		.. autosummary::
			:toctree: _autosummary
			:template: template.rst

			bayes_vuln
			bayes_risk
			g_vuln
			l_risk
			shannon
			geo
			d_privacy
	)pbdoc";

	init_mechanism_bayes_vuln_module(m.def_submodule("bayes_vuln", ""));
	init_mechanism_bayes_risk_module(m.def_submodule("bayes_risk", ""));
	init_mechanism_g_vuln_module    (m.def_submodule("g_vuln",     ""));
	init_mechanism_l_risk_module    (m.def_submodule("l_risk",     ""));
	init_mechanism_shannon_module   (m.def_submodule("shannon",    ""));
	init_mechanism_geo_module       (m.def_submodule("geo",        ""));
	init_mechanism_d_privacy_module (m.def_submodule("d_privacy",  ""));

}