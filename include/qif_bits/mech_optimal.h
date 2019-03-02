
namespace mechanism {

template <typename eT> using ME = MatrixEntry<eT>;

// Returns the mechanism satisfying eps*d privacy and having the best utility wrt pi and loss
//
template<typename eT>
Chan<eT> optimal_utility(
	const Prob<eT>& pi,
	uint n_cols,
	Metric<eT, uint> d_priv,
	Metric<eT, uint> loss,
	eT inf = eT(std::log(1e200))	// ignore large distances to avoid numerical instability. infinity<eT>() could be used to disable
) {
	uint M = pi.n_cols,
		 N = n_cols;

	// C: M x N   unknowns
	// We have M x N variables
	lp::LinearProgram<eT> lp;
	auto vars = lp.make_vars(M, N, 0, 1);

	// cost function: minimize sum_xy pi_x C_xy loss(x,y)
	lp.maximize = false;
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			lp.set_obj_coeff(vars[x][y], pi(x) * loss(x, y));

	// Build equations for C_xy <= exp(eps d_priv(x,x')) C_x'y
	//
	for(uint x1 = 0; x1 < M; x1++) {
	for(uint x2 = 0; x2 < M; x2++) {
		if(x1 == x2 || d_priv.chainable(x1, x2)) continue;			// constraints for chainable inputs are redundant
		if(!less_than(d_priv(x1, x2), inf)) continue;				// inf distance, i.e. no constraint

		for(uint y = 0; y < N; y++) {
			auto con = lp.make_con(-infinity<eT>(), 0);

			lp.set_con_coeff(con, vars[x1][y], 1);
			lp.set_con_coeff(con, vars[x2][y], - std::exp(d_priv(x1, x2)));
		}
	}}

	// equalities for summing up to 1
	//
	for(uint x = 0; x < M; x++) {
		auto con = lp.make_con(1, 1);

		// coeff 1 for variable C[x,y]
		for(uint y = 0; y < N; y++)
			lp.set_con_coeff(con, vars[x][y], 1);
	}

	// solve program
	//
	if(!lp.solve())
		return Chan<eT>();

	// reconstrict channel from solution
	//
	Chan<eT> C(M, N);
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			C(x, y) = lp.get_solution(vars[x][y]);

	return C;
}

template<typename eT>
Chan<eT> dist_optimal_utility(Prob<eT> pi, uint n_cols, Metric<eT, uint> d_priv, Metric<eT, uint> loss) {

	uint M = pi.n_cols,
		 N = n_cols;

	// insert all distances in a std::set to keep unique ones.
	// Then collect in dists vector and sort
	//
	std::set<eT> dists_set;
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			dists_set.insert(d_priv(x, y));

	uint D = dists_set.size();
	Col<eT> dists(D);
	uint i = 0;
	for(eT v : dists_set)
		dists(i++) = v;
	dists = arma::sort(dists);

	// DI : MxN matrix, DIxy is the index of d_priv(x,y) in dists
	Mat<uint> DI(M, N);
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			DI(x, y) = arma::find(dists == d_priv(x, y), 1).eval()(0);

	// one var for each distinct distance and output
	lp::LinearProgram<eT> lp;
	auto vars = lp.make_vars(D, N, 0, 1);

	// cost function: minimize sum_xy pi_x X[d(x,y),y] loss(x,y)
	lp.maximize = false;
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			lp.set_obj_coeff(vars[DI(x,y)][y], pi(x) * loss(x, y), true);

	// Build equations for X_d_i <= exp(eps |d_i - d_i+1|) X_d_j
	//
	for(uint dist_i = 0; dist_i < D-1; dist_i++) {
		eT diff = dists(dist_i+1) - dists(dist_i);

		for(uint y = 0; y < N; y++) {
			auto con = lp.make_con(-infinity<eT>(), 0);

			lp.set_con_coeff(con, vars[dist_i  ][y], 1);
			lp.set_con_coeff(con, vars[dist_i+1][y], - std::exp(diff));

			con = lp.make_con(-infinity<eT>(), 0);

			lp.set_con_coeff(con, vars[dist_i+1][y], 1);
			lp.set_con_coeff(con, vars[dist_i  ][y], - std::exp(diff));
		}
	}

	// equalities for summing up to 1
	// Note: if the same distance appears multiple times in the same row, we're going to insert multiple 1's
	// for the same cell, which are summed due to add = true in set_con_coeff
	//
	for(uint x = 0; x < M; x++) {
		auto con = lp.make_con(1, 1);

		for(uint y = 0; y < N; y++)
			lp.set_con_coeff(con, vars[DI(x,y)][y], 1, true);
	}

	// solve program
	//
	if(!lp.solve())
		return Chan<eT>();

	// reconstrict channel from solution
	//
	Chan<eT> C(M, N);
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			C(x, y) = lp.get_solution(vars[DI(x,y)][y]);

	return C;
}

// This is the first version, that had variables X_d instead of X_d,y
//
template<typename eT>
Chan<eT> dist_optimal_utility_strict(Prob<eT> pi, uint n_cols, Metric<eT, uint> d_priv, Metric<eT, uint> loss) {

	uint M = pi.n_cols,
		 N = n_cols;

	// insert all distances in a std::set to keep unique ones.
	// Then collect in dists vector and sort
	//
	std::set<eT> dists_set;
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			dists_set.insert(d_priv(x, y));

	uint D = dists_set.size();
	Col<eT> dists(D);
	uint i = 0;
	for(eT v : dists_set)
		dists(i++) = v;
	dists = arma::sort(dists);

	// DI : MxN matrix, DIxy is the index of d_priv(x,y) in dists
	Mat<uint> DI(M, N);
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			DI(x, y) = arma::find(dists == d_priv(x, y), 1).eval()(0);

	// C: D variables
	lp::LinearProgram<eT> lp;
	auto vars = lp.make_vars(D, eT(0), eT(1));		// one var for each distinct distance

	// cost function: minimize sum_xy pi_x var<C_xy> loss(x,y)
	lp.maximize = false;
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			lp.set_obj_coeff(vars[DI(x,y)], pi(x) * loss(x, y), true);

	// Build equations for X_d_i <= exp(eps |d_i - d_i+1|) X_d_j
	//
	for(uint dist_i = 0; dist_i < D-1; dist_i++) {
		eT diff = dists(dist_i+1) - dists(dist_i);

		auto con = lp.make_con(-infinity<eT>(), 0);

		lp.set_con_coeff(con, vars[dist_i  ], 1);
		lp.set_con_coeff(con, vars[dist_i+1], - std::exp(diff));

		con = lp.make_con(-infinity<eT>(), 0);

		lp.set_con_coeff(con, vars[dist_i+1], 1);
		lp.set_con_coeff(con, vars[dist_i  ], - std::exp(diff));
	}

	// equalities for summing up to 1
	// Note: if the same distance appears multiple times in the same row, we're going to insert multiple 1's
	// for the same con/var, which are summed due to add = true in set_con_coeff
	//
	for(uint x = 0; x < M; x++) {
		auto con = lp.make_con(1, 1);

		for(uint y = 0; y < N; y++)
			lp.set_con_coeff(con, vars[DI(x,y)], 1, true);
	}

	// solve program
	//
	if(!lp.solve())
		return Chan<eT>();

	// reconstrict channel from solution
	//
	Chan<eT> C(M, N);
	for(uint x = 0; x < M; x++)
		for(uint y = 0; y < N; y++)
			C(x, y) = lp.get_solution(vars[DI(x,y)]);

	return C;
}

} // namespace mechanism
