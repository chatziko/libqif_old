
namespace shannon {

// H(X) = - sum_x pi[x] log2(pi[x])
//
template<typename eT>
eT entropy(const Prob<eT>& pi) {
	eT sum_x = 0;
	for(uint x = 0; x < pi.n_cols; x++) {
		eT el = pi.at(x);
		sum_x -= el > 0 ? el * qif::log2(el) : 0;
	}

	return sum_x;
}

// computes H(X|Y)
//
// we use the formula
//    H(X|Y) = H(Y|X) + H(X) - H(Y)
// since H(Y|X) is easier to compute:
//    H(Y|X) = sum_x pi[x] H(C[x,-])   (entropy of row x)
//
template<typename eT>
eT post_entropy(const Prob<eT>& pi, const Chan<eT>& C) {
	check_prior_size(pi, C);

	eT Hyx = 0;
	for(uint x = 0; x < C.n_rows; x++)
		Hyx += pi.at(x) * entropy<eT>(C.row(x));

	return Hyx + entropy<eT>(pi) - entropy<eT>(pi * C);
}

template<typename eT>
eT add_leakage(const Prob<eT>& pi, const Chan<eT>& C) {
	return post_entropy(pi, C) - entropy(pi);
}

template<typename eT>
eT mult_leakage(const Prob<eT>& pi, const Chan<eT>& C) {
	return entropy(pi) / post_entropy(pi, C);
}

template<typename eT>
eT mulg_leakage(const Prob<eT>& pi, const Chan<eT>& C) {
	return real_ops<eT>::log2(mult_leakage(pi, C));
}

//Blahut-Arimoto Algorithm
//
template<typename eT>
eT add_capacity(const Chan<eT>& C, eT max_diff = def_max_diff<eT>(), eT max_rel_diff = def_max_rel_diff<eT>()) {
	uint m = C.n_rows;
	uint n = C.n_cols;

	Prob<eT> F(m), Px(m), Py(m);
	uniform(Px);

	while(1) {
		// Py = output dist
		Py = Px * C;

		// update F
		for(uint i = 0; i < m; i++) {
			eT s = 0;
			for(uint j = 0; j < n; j++) {
				eT el = C.at(i, j);
				s += el > 0 ? el * log(el / Py.at(j)) : 0;		// NOTE: this is e-base log, not 2-base!
			}
			F.at(i) = exp(s);
		}

		// check stop condition
		eT d = dot(F, Px);
		eT IL = qif::log2(d);
		eT IU = qif::log2(max(F));

		if(equal(IU, IL, max_diff, max_rel_diff))
			return IL;

		// update Px
		Px %= F / d;		// % is element-wise mult
	}
}

} // mechanism
