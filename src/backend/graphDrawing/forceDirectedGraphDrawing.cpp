#include "forceDirectedGraphDrawing.h"
#include <random>
#include <unsupported/Eigen/NonLinearOptimization>

struct Functor
{
	using InputType = Eigen::VectorXd;
	using ValueType = Eigen::VectorXd;
	using Matrix = Eigen::MatrixXd;
	using Scalar = double;
	using JacobianType = Matrix;

	enum {
		InputsAtCompileTime = Eigen::Dynamic,
		ValuesAtCompileTime = Eigen::Dynamic
	};

	const int m_inputs, m_values;

	Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}

	int inputs() const { return m_inputs; }
	int values() const { return m_values; }
};

class ForceDirectedFunctor : public Functor
{
	private:
		static constexpr double _alpha = 1;
		const ArticleGraph& _event_network;
		const std::vector<double>& _x_positions;
		const std::size_t _n;

	public:
		ForceDirectedFunctor(const ArticleGraph& event_network, const std::vector<double>& x_positions)
			:Functor(x_positions.size(), x_positions.size() + 2 * x_positions.size() * (x_positions.size() - 1)),
			_event_network(event_network),
			_x_positions(x_positions),
			_n(x_positions.size())
	{}

		double r(double s) const
		{
			return std::pow(s, -_alpha);
		}

		double norm(double x1, double x2, double y1, double y2) const
		{
			return std::sqrt(std::pow(x1-x2,2) + std::pow(y1-y2,2));
		}

		int operator()(const InputType& y, ValueType& fvec) const
		{
			fvec.setZero();

			for(std::size_t i = 0; i < _n; i++)
			{
				fvec(i) = r(y(i)) + r(1-y(i));
			}

			std::size_t cur_pos = _n;
			for(std::size_t i = 0; i < _n; i++)
				for(std::size_t j = 0; j < _n; j++)
					if(i != j)
					{
						if(boost::edge(i, j, _event_network).second)
							fvec(cur_pos) = std::pow(y(i) - y(j), 2.0);

						cur_pos++;
					}

			for(std::size_t i = 0; i < _n; i++)
				for(std::size_t j = 0; j < _n; j++)
					if(i != j)
					{
						fvec(cur_pos) = r(norm(_x_positions[i],_x_positions[j],y(i),y(j)));

						cur_pos++;
					}

			return 0;
		}

		// int df(const InputType& x, Matrix& fjac) const
		// {
			// return 0;
		// }
};

std::vector<double> force_directed_graph_drawing(const ArticleGraph& event_network, const std::vector<double>& x_positions)
{
	ForceDirectedFunctor f(event_network, x_positions);
	Eigen::NumericalDiff<ForceDirectedFunctor> f_numerical_diff(f);
	Eigen::LevenbergMarquardt<Eigen::NumericalDiff<ForceDirectedFunctor>, double> lm_solver(f_numerical_diff);

	ForceDirectedFunctor::InputType y(x_positions.size());
	y.setOnes();
	lm_solver.minimize(y);

	std::vector<double> rtn;
	rtn.resize(y.size());
	Eigen::VectorXd::Map(&rtn[0], y.size()) = y;

	return rtn;
}
