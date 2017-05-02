#include "forceDirectedGraphDrawing.h"
#include <random>
#include <iostream>
#include "forceDirectedFunctors.h"

namespace WikiMainPath {
	namespace GraphDrawing {
		std::string flag_to_string(int flag)
		{
			static std::map<int, std::string> m = {
				{ -2, "NotStarted" },
				{ -1, "Running" },
				{ 0, "ImproperInputParameters" },
				{ 1, "RelativeReductionTooSmall" },
				{ 2, "RelativeErrorTooSmall" },
				{ 3, "RelativeErrorAndReductionTooSmall" },
				{ 4, "CosinusTooSmall" },
				{ 5, "TooManyFunctionEvaluation" },
				{ 6, "FtolTooSmall" },
				{ 7, "XtolTooSmall" },
				{ 8, "GtolTooSmall" },
				{ 9, "UserAsked" }
			};

			return m.at(flag);
		}


		std::vector<double> force_directed_graph_drawing(const ArticleGraph& event_network, const std::vector<double>& x_positions)
		{
			ForceDirectedFunctor f(event_network, x_positions);
			// ForceDirectedSparseFunctor f_sparse(event_network, x_positions);
			// Eigen::NumericalDiff<ForceDirectedFunctor> f_numerical_diff(f);

			Eigen::LevenbergMarquardt<ForceDirectedFunctor, double> lm_solver(f);
			// Eigen::LevenbergMarquardt<ForceDirectedSparseFunctor, double> lm_solver(f_sparse);
			// Eigen::LevenbergMarquardt<Eigen::NumericalDiff<ForceDirectedFunctor>, double> lm_solver(f_numerical_diff);

			ForceDirectedSparseFunctor::InputType y(x_positions.size());

			std::random_device rd;
			std::mt19937 rand_gen(rd());
			std::uniform_real_distribution<double> unif_0_1;
			for(int i = 0; i < y.size(); i++)
				y(i) = unif_0_1(rand_gen);

			/*int flag = */lm_solver.minimize(y);
			// std::cout << "FLAG: " << flag_to_string(flag) << std::endl;

			std::vector<double> rtn;
			// rtn.resize(y.size());
			// Eigen::VectorXd::Map(&rtn[0], y.size()) = y;

			return rtn;
		}

	}
}
