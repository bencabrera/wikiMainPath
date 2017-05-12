#pragma once

#include "../../core/graph.h"
#include <unsupported/Eigen/NonLinearOptimization>
#include <Eigen/SparseCore>

namespace WikiMainPath {
	namespace GraphDrawing {

		template<typename TJacobianType>
			struct Functor
			{
				using InputType = Eigen::VectorXd;
				using ValueType = Eigen::VectorXd;
				using Scalar = double;
				using JacobianType = TJacobianType;

				enum {
					InputsAtCompileTime = Eigen::Dynamic,
					ValuesAtCompileTime = Eigen::Dynamic
				};

				const int m_inputs, m_values;

				Functor(int inputs, int values);

				int inputs() const;
				int values() const;
			};

		class ForceDirectedFunctor : public Functor<Eigen::MatrixXd>
		{
			private:

				const EventNetwork& _event_network;
				const std::vector<double>& _x_positions;
				const std::size_t _n;

			public:
				ForceDirectedFunctor(const EventNetwork& event_network, const std::vector<double>& x_positions);

				int operator()(const InputType& y, ValueType& fvec) const;
				int df(const InputType& y, JacobianType& fjac) const;
		};


		using SparseMatrix = Eigen::SparseMatrix<double>;

		class ForceDirectedSparseFunctor : public Functor<SparseMatrix>
		{
			private:

				const EventNetwork& _event_network;
				const std::vector<double>& _x_positions;
				const std::size_t _n;

			public:
				ForceDirectedSparseFunctor(const EventNetwork& event_network, const std::vector<double>& x_positions);

				int operator()(const InputType& y, ValueType& fvec) const;
				int df(const InputType& y, JacobianType& fjac) const;
		};

	}
}
